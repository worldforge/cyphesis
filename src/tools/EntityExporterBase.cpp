//
// Copyright (C) 2009 Alistair Riddoch
// Copyright (C) 2012 Erik Ogenvik
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "EntityExporterBase.h"

#include <Atlas/Codecs/XML.h>
#include <Atlas/Message/QueuedDecoder.h>
#include <Atlas/Message/MEncoder.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation.h>

#include <algorithm>
#include <iostream>
#include <iomanip>



//The following are adapters to make the Ember logging syntax work with the Cyphesis log.
#include "common/log.h"
#include "common/debug.h"

static const bool debug_flag = false;
#define S_LOG_VERBOSE(message) \
    { debug(std::stringstream ss;\
    ss << message;\
    log(NOTICE, ss.str());) }
#define S_LOG_INFO(message) \
    { std::stringstream ss;\
    ss << message;\
    log(INFO, ss.str()); }
#define S_LOG_WARNING(message) \
    { std::stringstream ss;\
    ss << message;\
    log(WARNING, ss.str()); }
#define S_LOG_FAILURE(message) \
    { std::stringstream ss;\
    ss << message;\
    log(ERROR, ss.str()); }
#define S_LOG_CRITICAL(message) \
    { std::stringstream ss;\
    ss << message;\
    log(CRITICAL, ss.str()); }


using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Talk;
using Atlas::Objects::Operation::Get;
using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;


long integerId(const std::string& id);

namespace {
    bool idSorter(const std::string& lhs, const std::string& rhs)
    {
        return integerId(lhs) < integerId(rhs);
    }

    std::vector<std::string> ignoredProperties{"_modifiers", "_minds", "velocity", "loc", "stamp", "contains"};
}

EntityExporterBase::EntityExporterBase(const std::string& accountId, const std::string& avatarId, const std::string& currentTimestamp) :
        mAccountId(accountId),
        mAvatarId(avatarId),
        mCurrentTimestamp(currentTimestamp),
        mStats({}),
        mComplete(false),
        mCancelled(false),
        mOutstandingGetRequestCounter(0),
        mExportTransient(false),
        mPreserveIds(false)
{
}

void EntityExporterBase::setExportTransient(bool exportTransient)
{
    mExportTransient = exportTransient;
}

bool EntityExporterBase::getExportTransient() const
{
    return mExportTransient;
}

void EntityExporterBase::setPreserveIds(bool preserveIds)
{
    mPreserveIds = preserveIds;
}

bool EntityExporterBase::getPreserveIds() const
{
    return mPreserveIds;
}

const EntityExporterBase::Stats& EntityExporterBase::getStats() const
{
    return mStats;
}

void EntityExporterBase::cancel()
{
    mCancelled = true;
}

void EntityExporterBase::dumpEntity(RootEntity ent)
{
    std::string loc;
    if (!ent->isDefaultLoc()) {
        loc = ent->getLoc();
    }
    auto id = ent->getId();
    auto persistedId = id;
    if (!mPreserveIds && persistedId != "0") {
        std::stringstream ss;
        ss << mEntityMap.size();
        persistedId = ss.str();
        ent->setId(persistedId);
    }
    mIdMapping.emplace(id, persistedId);


    Atlas::Message::MapType entityMap;
    ent->addToMessage(entityMap);

    //Check if any attribute is the same as the type default, and if so don't persist it.
    std::vector<std::string> attributesToRemove = ignoredProperties;
    Atlas::Message::MapType attributesToAdd;
    auto typeAttributesI = mTypeProperties.find(ent->getParent());
    if (typeAttributesI != mTypeProperties.end()) {
        auto& parentAttributes = typeAttributesI->second;
        for (auto& entry : entityMap) {
            auto parentAttributeI = parentAttributes.find(entry.first);
            if (parentAttributeI != parentAttributes.end()) {
                auto& typeElement = parentAttributeI->second;
                auto& entityElement = entry.second;
                //If the property is unchanged from the type's just ignore it for the entity.
                if (typeElement == entityElement) {
                    attributesToRemove.emplace_back(entry.first);
                } else {
                    //Check if we can describe the property using "append" or "prepend", if it's a list or map.
                    //First just make sure both properties have the same type.
                    if (entityElement.getType() == typeElement.getType()) {
                        if (entityElement.isList()) {
                            auto& entityList = entityElement.List();
                            auto& typeList = typeElement.List();
                            auto result = extractListPrependAppend(typeList, entityList);

                            if (!result.prepend.empty() || !result.append.empty()) {
                                //The sublist matches the data in the type; we can remove it and add "!append" and "!prepend" properties
                                attributesToRemove.emplace_back(entry.first);
                                if (!result.prepend.empty()) {
                                    attributesToAdd.emplace(entry.first + "!prepend", std::move(result.prepend));
                                }
                                if (!result.append.empty()) {
                                    attributesToAdd.emplace(entry.first + "!append", std::move(result.append));
                                }
                            }
                        } else if (entityElement.isMap()) {
                            auto& entityElementMap = entityElement.Map();
                            auto& typeElementMap = entityElement.Map();
                            auto result = extractMapPrepend(typeElementMap, entityElementMap);
                            if (!result.empty()) {
                                attributesToRemove.emplace_back(entry.first);
                                attributesToAdd.emplace(entry.first + "!prepend", std::move(result));
                            }
                        }
                    }
                }
            } else {
                //Also remove properties that aren't in the type and are empty.
                if (entry.second.isNone() || (entry.second.isMap() && entry.second.Map().empty()) || (entry.second.isList() && entry.second.List().empty())) {
                    attributesToRemove.emplace_back(entry.first);
                }
            }
        }
    }

    //Check for empty "contains" property and remove if so.
    auto containsI = entityMap.find("contains");
    if (containsI != entityMap.end() && containsI->second.isList() && containsI->second.List().empty()) {
        attributesToRemove.emplace_back("contains");
    }

    //Check for a zero "angular" and remove it if so.
    auto angularI = entityMap.find("angular");
    if (angularI != entityMap.end()) {
        if (angularI->second.isNone() || (angularI->second.isList() && angularI->second.List() == ListType{0.0, 0.0, 0.0})) {
            attributesToRemove.emplace_back("angular");
        }
    }

    //Check for a zero "_propel" and remove it if so.
    auto propelI = entityMap.find("_propel");
    if (propelI != entityMap.end()) {
        if (propelI->second.isNone() || (propelI->second.isList() && propelI->second.List() == ListType{0.0, 0.0, 0.0})) {
            attributesToRemove.emplace_back("_propel");
        }
    }

    for (auto& attribute : attributesToRemove) {
        entityMap.erase(attribute);
    }

    for (auto& entry : attributesToAdd) {
        entityMap.insert(std::move(entry));
    }
   // mEntities.emplace_back(entityMap);
    mEntityMap[id].entity = entityMap;
    mEntityMap[id].loc = loc;
    if (!loc.empty()) {
        mEntityMap[loc].children.emplace_back(id);
    }

//    auto entityMapI = mEntityMap.find(loc);
//    if (entityMapI != mEntityMap.end()) {
//        auto I = entityMapI->second->find("$contains");
//        if (I != entityMapI->second->end()) {
//            auto result = I->second.asMap().emplace(id, entityMap);
//            mEntityMap[id] = &result.first->second.asMap();
//        }else {
//            auto result = entityMapI->second->emplace("$contains", MapType{{id, entityMap}});
//            mEntityMap[id] = &result.first->second.asMap();
//        }
//    } else {
//        auto result = mTopLevelEntities.emplace(id, entityMap);
//        mEntityMap[id] = &result.first->second;
//    }

}

void EntityExporterBase::pollQueue()
{
    //When we've queried, and gotten responses for all entities, and all types are bound,
    //and there are no more thoughts we're waiting to receive; then we're done.
    if (mEntityQueue.empty() && mOutstandingGetRequestCounter == 0) {
        complete();
        return;
    }

    //Make sure that no more than 5 outstanding get requests are currently sent to the server.
    //The main reason for us not wanting more is that we then run the risk of overflowing the server connection (which will then be dropped).
    while (mOutstandingGetRequestCounter < 5 && !mEntityQueue.empty()) {
        Get get;

        Anonymous get_arg;
        get_arg->setObjtype("obj");
        get_arg->setId(mEntityQueue.front());
        get_arg->setAttr("archive", 1);

        get->setArgs1(get_arg);
        get->setFrom(mAccountId);
        get->setSerialno(newSerialNumber());

        sigc::slot<void(const Operation&)> slot = sigc::mem_fun(*this, &EntityExporterBase::operationGetResult);
        sendAndAwaitResponse(get, slot);
        S_LOG_VERBOSE("Requesting info about entity with id " << get_arg->getId())

        mOutstandingGetRequestCounter++;

        mEntityQueue.pop_front();
        mStats.entitiesQueried++;
    }
    EventProgress.emit();
}

void EntityExporterBase::infoArrived(const Operation& op)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        mStats.entitiesError++;
        EventProgress.emit();
        return;
    }
    RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
    if (!ent.isValid()) {
        S_LOG_WARNING("Malformed OURS when dumping.")
        mStats.entitiesError++;
        EventProgress.emit();
        return;
    }
    S_LOG_VERBOSE("Got info when dumping about entity " << ent->getId() << ". Outstanding requests: " << mOutstandingGetRequestCounter)
    mStats.entitiesReceived++;
    EventProgress.emit();
    //If the entity is transient and we've been told not to export transient ones, we should skip this one (and all of its children).
    bool shouldSkip = false;
    if (!mExportTransient) {
        //First check if there's a "transient" attribute set, and it's positive
        if (ent->hasAttr("transient")) {
            if (ent->getAttr("transient").isNum() && ent->getAttr("transient").asNum() != 0) {
                shouldSkip = true;
            }
        } else {
            //If there's no "transient" attribute set, check if the type has it set.
            if (mTransientTypes.find(ent->getParent()) != mTransientTypes.end()) {
                shouldSkip = true;
            }
        }
    }

    if (!shouldSkip) {
        //Make a copy so that we can sort the contains list and update it in the
        //entity
        RootEntity entityCopy(ent->copy());
        auto contains = ent->getContains();
        //Sort the contains list so it's deterministic
        std::sort(contains.begin(), contains.end(), idSorter);
        entityCopy->setContains(contains);

        //Remove attributes which shouldn't be persisted
        dumpEntity(std::move(entityCopy));
        for (auto& id : contains) {
            mEntityQueue.push_back(id);
        }
    }
    pollQueue();
}

void EntityExporterBase::requestRule(const std::string& rule)
{
    Get get;
    Anonymous arg;
    arg->setId(rule);
    arg->setObjtype("class");
    get->setArgs1(arg);
    get->setSerialno(newSerialNumber());
    get->setFrom(mAccountId);

    sigc::slot<void(const Operation&)> slot = sigc::mem_fun(*this, &EntityExporterBase::operationGetRuleResult);
    sendAndAwaitResponse(get, slot);

    mStats.rulesQueried++;

    mOutstandingGetRequestCounter++;
}

void EntityExporterBase::adjustReferencedEntities()
{
    S_LOG_VERBOSE("Adjusting referenced entity ids.")
    if (!mPreserveIds) {
//		for (auto& mind : mMinds) {
//			//We know that mMinds only contain maps, and that there's always a "thoughts" list
//			auto& thoughts = mind.asMap().find("thoughts")->second.asList();
//			for (auto& thought : thoughts) {
//				//If the thought is a list of things the entity owns, we should adjust it with the new entity ids.
//				if (thought.isMap()) {
//				    auto& thoughtMap = thought.Map();
//					if (thoughtMap.count("things") > 0) {
//						auto& thingsElement = thoughtMap.find("things")->second;
//						if (thingsElement.isMap()) {
//							for (auto& thingI : thingsElement.asMap()) {
//								if (thingI.second.isList()) {
//									Atlas::Message::ListType newList;
//									for (auto& thingId : thingI.second.asList()) {
//										if (thingId.isString()) {
//											auto entityIdLookupI = mIdMapping.find(thingId.asString());
//											//Check if the owned entity has been created with a new id. If so, replace the data.
//											if (entityIdLookupI != mIdMapping.end()) {
//												newList.emplace_back(entityIdLookupI->second);
//											} else {
//												newList.push_back(thingId);
//											}
//										} else {
//											newList.push_back(thingId);
//										}
//									}
//									thingI.second = newList;
//								}
//							}
//						}
//					}
//
//					if (thoughtMap.count("pending_things") > 0) {
//						//things that the entity owns, but haven't yet discovered are expressed as a list of entity ids
//						auto& pendingThingsElement = thoughtMap.find("pending_things")->second;
//						if (pendingThingsElement.isList()) {
//							Atlas::Message::ListType newList;
//							for (auto& thingId : pendingThingsElement.asList()) {
//								if (thingId.isString()) {
//									auto entityIdLookupI = mIdMapping.find(thingId.asString());
//									//Check if the owned entity has been created with a new id. If so, replace the data.
//									if (entityIdLookupI != mIdMapping.end()) {
//										newList.emplace_back(entityIdLookupI->second);
//									} else {
//										newList.push_back(thingId);
//									}
//								} else {
//									newList.push_back(thingId);
//								}
//							}
//							pendingThingsElement = newList;
//						}
//					}
//
//					if (thoughtMap.count("object") > 0) {
//                        auto& objectElement = thoughtMap.find("object")->second;
//                        if (objectElement.isString()) {
//                            std::string& objectString = objectElement.String();
//                            //Other entities are referred to using the syntax "'$eid:...'".
//                            //For example, the entity with id 2 would be "'$eid:2'".
//                            auto pos = objectString.find("$eid:");
//                            if (pos != std::string::npos) {
//                                auto quotePos = objectString.find('\'', pos);
//                                if (quotePos != std::string::npos) {
//                                    auto id = objectString.substr(pos + 5, quotePos - pos - 5);
//                                    auto I = mIdMapping.find(id);
//                                    if (I != mIdMapping.end()) {
//                                        objectString.replace(pos + 5, quotePos - 7, I->second);
//                                    }
//                                }
//                            }
//                        }
//					}
//
//				}
//			}
//		}
    }
    for (auto& entry : mEntityMap) {
        auto& entityMap = entry.second.entity;
        //We know that mEntities only contain maps
        auto containsIElem = entityMap.find("contains");
        if (containsIElem != entityMap.end()) {
            auto& containsElem = containsIElem->second;
            if (containsElem.isList()) {
                auto& contains = containsElem.asList();
                Atlas::Message::ListType newContains;
                newContains.reserve(contains.size());
                for (auto& entityElem : contains) {
                    //we can assume that it's string
                    auto I = mIdMapping.find(entityElem.asString());
                    if (I != mIdMapping.end()) {
                        newContains.emplace_back(I->second);
                    }
                }
                contains = newContains;
            }
        }
        for (auto I = entityMap.begin(); I != entityMap.end();) {
            // If we started with a non-empty map or list, and after resolving references have an empty such
            //  it means that we removed a transient entity ref. In that case we'll remove the whole entry.
            auto& propertyElement = I->second;
            bool hadElements = (propertyElement.isList() && !propertyElement.List().empty()) || (propertyElement.isMap() && !propertyElement.Map().empty());
            resolveEntityReferences(propertyElement);
            bool hasNoElementsAnymore = (propertyElement.isList() && propertyElement.List().empty()) || (propertyElement.isMap() && propertyElement.Map().empty());
            if (hadElements && hasNoElementsAnymore) {
                I = entityMap.erase(I);
            } else {
                I++;
            }
        }
    }
}

void EntityExporterBase::resolveEntityReferences(Atlas::Message::Element& element)
{
    if (element.isMap()) {
        auto& elementMap = element.asMap();
        auto entityRefI = elementMap.find("$eid");
        if (entityRefI != elementMap.end() && entityRefI->second.isString()) {
            auto I = mIdMapping.find(entityRefI->second.asString());
            if (I != mIdMapping.end()) {
                entityRefI->second = I->second;
            } else {
                //If the reference is to a transient entity, we shouldn't store it, and instead remove it completely.
                elementMap.erase(entityRefI);
            }
        }
        //If it's a map we need to process all child elements too
        for (auto I = elementMap.begin(); I != elementMap.end();) {
            auto& childElement = I->second;
            bool hadElements = (childElement.isList() && !childElement.List().empty()) || (childElement.isMap() && !childElement.Map().empty());
            resolveEntityReferences(childElement);
            bool hasNoElementsAnymore = (childElement.isList() && childElement.List().empty()) || (childElement.isMap() && childElement.Map().empty());
            if (hadElements && hasNoElementsAnymore) {
                I = elementMap.erase(I);
            } else {
                I++;
            }
        }
    } else if (element.isList()) {
        //If it's a list we need to process all child elements too.
        //If we started with a non-empty map or list, and after resolving references have an empty such
        // it means that we removed a transient entity ref. In that case we'll remove the whole entry.
        auto& elementList = element.asList();
        for (auto I = elementList.begin(); I != elementList.end(); ) {
            if (I->isList()) {
                auto& childElementList = I->List();
                if (!childElementList.empty()) {
                    resolveEntityReferences(*I);
                    if (childElementList.empty()) {
                        I = elementList.erase(I);
                        continue;
                    }
                }
            } else if (I->isMap()) {
                auto& childElementMap = I->Map();
                if (!childElementMap.empty()) {
                    resolveEntityReferences(*I);
                    if (childElementMap.empty()) {
                        I = elementList.erase(I);
                        continue;
                    }
                }
            }
            ++I;
        }
    }
}

void EntityExporterBase::complete()
{

    adjustReferencedEntities();

    Anonymous root;
    Atlas::Message::MapType meta;

    meta["timestamp"] = mCurrentTimestamp;
    meta["transients"] = mExportTransient;
    meta["preserved_ids"] = mPreserveIds;

    Atlas::Message::MapType server;
    fillWithServerData(server);

    meta["server"] = server;

    root->setAttr("meta", meta);


    std::vector<Atlas::Message::Element> entities;
    //First find all top level entities.
    std::vector<EntityEntry*> topLevelEntities;

    for (auto& entry: mEntityMap) {
        if (entry.second.loc.empty()) {
            auto containsResult = entry.second.entity.emplace("~contains", ListType{});
            populateChildEntities(containsResult.first->second.asList(), entry.second.children);
            entities.emplace_back(std::move(entry.second.entity));
        }
    }

    root->setAttr("entities", std::move(entities));

    std::fstream filestream(mFilename, std::ios::out);
    Atlas::Message::QueuedDecoder decoder;
    Atlas::Codecs::XML codec(filestream, filestream, decoder);
    std::unique_ptr<Atlas::Formatter> formatter(createMultiLineFormatter(filestream, codec));

    Atlas::Objects::ObjectsEncoder encoder(*formatter);

    encoder.streamBegin();
    encoder.streamObjectsMessage(root);
    encoder.streamEnd();

    filestream.close();

    //Clear the lists to release the memory allocated
    mEntityMap.clear();

    mComplete = true;
    EventCompleted.emit();
    S_LOG_INFO("Completed exporting " << mStats.entitiesReceived << " entities and " << mStats.rulesReceived << " rules.")
}

void EntityExporterBase::populateChildEntities(Atlas::Message::ListType& contains, const std::vector<std::string>& children) {
    for (auto& childId : children) {
        auto I = mEntityMap.find(childId);
        if (I == mEntityMap.end()) {
            S_LOG_CRITICAL("Could not find child with id " << childId)
            throw std::runtime_error("Could not find child.");
        }
        auto& childEntity = I->second;
        if (!childEntity.children.empty()) {
            auto containsResult = childEntity.entity.emplace("~contains", ListType{});
            populateChildEntities(containsResult.first->second.asList(), childEntity.children);
        }
        contains.emplace_back(childEntity.entity);
    }
}


void EntityExporterBase::start(const std::string& filename, const std::string& entityId)
{
    if (mComplete || mCancelled) {
        S_LOG_FAILURE("Can not restart an already completed or cancelled export instance.")
        return;
    }

    S_LOG_INFO("Starting entity dump to file '" << filename << "'.")
    mFilename = filename;
    mRootEntityId = entityId;

    //Start by getting all rules, since we need them to
    // 1) check for transients
    // 2) if we're also exporting rules
    // 3) remove entity attributes that are the same as the defaults
    requestRule("root");

}

void EntityExporterBase::startRequestingEntities()
{
    // Send a get for the requested root entity
    mOutstandingGetRequestCounter++;
    Get get;

    Anonymous get_arg;
    get_arg->setObjtype("obj");
    get_arg->setId(mRootEntityId);
    get_arg->setAttr("archive", 1);
    get->setArgs1(get_arg);

    get->setFrom(mAccountId);
    get->setSerialno(newSerialNumber());

    sigc::slot<void(const Operation&)> slot = sigc::mem_fun(*this, &EntityExporterBase::operationGetResult);
    sendAndAwaitResponse(get, slot);

    mStats.entitiesQueried++;
    EventProgress.emit();
}

void EntityExporterBase::operationGetResult(const Operation& op)
{
    mOutstandingGetRequestCounter--;
    if (!mCancelled) {
        if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
            infoArrived(op);
        } else {
            std::string errorMessage;
            if (op->getClassNo() == Atlas::Objects::Operation::ERROR_NO) {
                if (!op->getArgs().empty()) {
                    auto arg = op->getArgs().front();
                    if (arg->hasAttr("message")) {
                        const Atlas::Message::Element messageElem = arg->getAttr("message");
                        if (messageElem.isString()) {
                            errorMessage = messageElem.asString();
                        }
                    }
                }
            }
            S_LOG_WARNING("Got unexpected response on a GET request with operation of type " << op->getParent())
            S_LOG_WARNING("Error message: " << errorMessage)
            mStats.entitiesError++;
            EventProgress.emit();
        }
    }
}

void EntityExporterBase::operationGetRuleResult(const Operation& op)
{
    if (!mCancelled) {
        if (op->getArgs().empty()) {
            S_LOG_WARNING("Got response to GET for rule with no args.")
            mStats.rulesError++;
            cancel();
            return;
        }

        Root ent = smart_dynamic_cast<Root>(op->getArgs().front());
        if (!ent.isValid()) {
            S_LOG_WARNING("Malformed rule arg when dumping.")
            mStats.rulesError++;
            cancel();
            return;
        }

        std::vector<std::string> children;
        if (ent->hasAttr("children")) {
            Element childrenElement;
            if (ent->copyAttr("children", childrenElement) == 0) {
                if (childrenElement.isList()) {
                    ListType& childrenList = childrenElement.asList();
                    for (auto& childElem : childrenList) {
                        if (childElem.isString()) {
                            children.push_back(childElem.asString());
                        } else {
                            S_LOG_WARNING("Child was not a string.")
                        }
                    }
                }
            }
        }

        MapType ruleMap;
        ent->addToMessage(ruleMap);

        std::string ruleId = ruleMap.find("id")->second.asString();

        Element attributesElem;
        bool foundTransientProperty = false;
        if (ent->copyAttr("properties", attributesElem) == 0 && attributesElem.isMap()) {
            mTypeProperties[ruleId] = attributesElem.Map();
            auto transientI = attributesElem.Map().find("transient");
            if (transientI != attributesElem.Map().end() && transientI->second.isInt()) {
                foundTransientProperty = true;
                if (transientI->second.isNum() && transientI->second.asNum() != 0) {
                    mTransientTypes.insert(ruleId);
                }
            }
        }

        if (!foundTransientProperty) {
            auto parentI = ruleMap.find("parent");
            if (parentI != ruleMap.end() && parentI->second.isString()) {
                const std::string& parent = parentI->second.String();
                if (mTransientTypes.find(parent) != mTransientTypes.end()) {
                    mTransientTypes.insert(ruleId);
                }
            }
        }

        for (auto& child : children) {
            requestRule(child);
        }

        mOutstandingGetRequestCounter--;
        mStats.rulesReceived++;
        EventProgress.emit();

        if (mOutstandingGetRequestCounter == 0) {
            startRequestingEntities();
        }
    }
}

EntityExporterBase::ListPrependAppendResult EntityExporterBase::extractListPrependAppend(const ListType& typeList, const ListType& entityList)
{
    EntityExporterBase::ListPrependAppendResult result;

    //We only handle additions of new entries (for now), so if the size of the entity list is equal or less than the type list we can break out already.
    if (typeList.size() >= entityList.size()) {
        return result;
    }

    //Check if we can prepend to the list
    size_t entityPrependIndex = 0;
    size_t entityAppendIndex = entityList.size() - 1;
    //See how element in the front and in the back are different from the typeList
    if (!typeList.empty()) {
        for (; entityPrependIndex < entityList.size(); ++entityPrependIndex) {
            if (entityList[entityPrependIndex] == typeList.front()) {
                break;
            }
        }
        for (; entityAppendIndex > 0; --entityAppendIndex) {
            if (entityList[entityAppendIndex] == typeList.back()) {
                break;
            }
        }
    }


    //Now check if the entityList minus both the prepended and appended elements is the same as the type one; if so we can prepend and append. Otherwise we need to just replace it fully.
    if ((entityAppendIndex != entityList.size() - 1 || entityPrependIndex != 0) && entityPrependIndex <= entityAppendIndex) {
        auto sublist = Atlas::Message::ListType(entityList.begin() + entityPrependIndex, entityList.begin() + entityAppendIndex + 1);
        if (sublist == typeList) {
            if (entityAppendIndex != entityList.size() - 1) {
                result.append = Atlas::Message::ListType(entityList.begin() + entityAppendIndex + 1, entityList.end());
            }
            if (entityPrependIndex != 0) {
                result.prepend = Atlas::Message::ListType(entityList.begin(), entityList.begin() + entityPrependIndex);
            }
        }
    }

    return result;
}

Atlas::Message::MapType EntityExporterBase::extractMapPrepend(const MapType& typeMap, const MapType& entityMap)
{
    //We only handle additions of new entries (for now), so if the size of the entity map is equal or less than the type map we can break out already.
    if (typeMap.size() >= entityMap.size()) {
        return {};
    }

    //First check if all entries in the type map also are present in the entity map. If so, any extra entries in the entity map should be prepended.
    for (auto& typeEntry : typeMap) {
        auto entityI = entityMap.find(typeEntry.first);
        if (entityI == entityMap.end()) {
            return {};
        }
        if (entityI->second != typeEntry.second) {
            return {};
        }
    }

    //We now know that all entries in the type map exists in the entity map; now we just need to find those that are extra
    MapType result;
    for (auto& entityEntry: entityMap) {
        if (typeMap.find(entityEntry.first) == typeMap.end()) {
            result.insert(entityEntry);
        }
    }
    return result;
}

