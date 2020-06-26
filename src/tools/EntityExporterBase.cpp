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

#include <sstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <common/TypeNode.h>



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


long integerId(const std::string & id);
//{
//	long intId = strtol(id.c_str(), 0, 10);
//	if (intId == 0 && id != "0") {
//		intId = -1L;
//	}
//
//	return intId;
//}
namespace {
    bool idSorter(const std::string& lhs, const std::string& rhs)
    {
        return integerId(lhs) < integerId(rhs);
    }
}

EntityExporterBase::EntityExporterBase(const std::string& accountId, const std::string& avatarId, const std::string& currentTimestamp) :
		mAccountId(accountId),
        mAvatarId(avatarId),
        mCurrentTimestamp(currentTimestamp),
        mStats( { }),
        mComplete(false),
        mCancelled(false),
        mOutstandingGetRequestCounter(0),
        mExportTransient(false),
        mPreserveIds(false),
        mExportRules(false)
{
}

void EntityExporterBase::setDescription(const std::string& description)
{
	mDescription = description;
}

void EntityExporterBase::setName(const std::string& name)
{
	mName = name;
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

void EntityExporterBase::setExportRules(bool exportRules)
{
	mExportRules = exportRules;
}

bool EntityExporterBase::getExportRules() const
{
	return mExportRules;
}

const EntityExporterBase::Stats& EntityExporterBase::getStats() const
{
	return mStats;
}

void EntityExporterBase::cancel()
{
	mCancelled = true;
}

void EntityExporterBase::dumpEntity(const RootEntity & ent)
{
	Atlas::Message::MapType entityMap;
	ent->addToMessage(entityMap);

    //Check if any attribute is the same as the type default, and if so don't persist it.
    std::vector<std::string> attributesToRemove;
    auto typeAttributesI = mTypeAttributes.find(ent->getParent());
    if (typeAttributesI != mTypeAttributes.end()) {
        auto& parentAttributes = typeAttributesI->second;
        for (auto& entry : entityMap) {
            auto parentAttributeI = parentAttributes.find(entry.first);
            if (parentAttributeI != parentAttributes.end()) {
                auto defaultAttributeI = parentAttributeI->second.Map().find("default");
                if (defaultAttributeI->second == entry.second) {
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

    for (auto& attribute : attributesToRemove) {
        entityMap.erase(attribute);
    }
	mEntities.emplace_back(entityMap);
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

		get->setArgs1(get_arg);
		get->setFrom(mAccountId);
		get->setSerialno(newSerialNumber());

		sigc::slot<void, const Operation&> slot = sigc::mem_fun(*this, &EntityExporterBase::operationGetResult);
		sendAndAwaitResponse(get, slot);
		S_LOG_VERBOSE("Requesting info about entity with id " << get_arg->getId());

		mOutstandingGetRequestCounter++;

		mEntityQueue.pop_front();
		mStats.entitiesQueried++;
	}
	EventProgress.emit();
}

void EntityExporterBase::infoArrived(const Operation & op)
{
	const std::vector<Root> & args = op->getArgs();
	if (args.empty()) {
		mStats.entitiesError++;
		EventProgress.emit();
		return;
	}
	RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
	if (!ent.isValid()) {
		S_LOG_WARNING("Malformed OURS when dumping.");
		mStats.entitiesError++;
		EventProgress.emit();
		return;
	}
	S_LOG_VERBOSE("Got info when dumping about entity " << ent->getId() << ". Outstanding requests: " << mOutstandingGetRequestCounter);
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
		std::list<std::string> contains = ent->getContains();
		//Sort the contains list so it's deterministic
		contains.sort(idSorter);
		entityCopy->setContains(contains);

		std::string persistedId = entityCopy->getId();

		if (!mPreserveIds && persistedId != "0") {
			std::stringstream ss;
			ss << mEntities.size();
			persistedId = ss.str();
			entityCopy->setId(persistedId);
		}
		mIdMapping.insert(std::make_pair(ent->getId(), persistedId));

		//Remove attributes which shouldn't be persisted
		entityCopy->removeAttr(Atlas::Objects::Entity::VELOCITY_ATTR);
		entityCopy->removeAttr(Atlas::Objects::Entity::LOC_ATTR);
		entityCopy->removeAttr(Atlas::Objects::STAMP_ATTR);
		dumpEntity(entityCopy);
		std::list<std::string>::const_iterator I = contains.begin();
		std::list<std::string>::const_iterator Iend = contains.end();
		for (; I != Iend; ++I) {
			mEntityQueue.push_back(*I);
		}
	}
	pollQueue();
}

void EntityExporterBase::requestRule(const std::string& rule)
{
	Get get;
	Anonymous arg;
	arg->setId(rule);
	get->setArgs1(arg);
	get->setObjtype("op");
	get->setSerialno(newSerialNumber());

	sigc::slot<void, const Operation&> slot = sigc::mem_fun(*this, &EntityExporterBase::operationGetRuleResult);
	sendAndAwaitResponse(get, slot);

	mStats.rulesQueried++;

	mOutstandingGetRequestCounter++;
}

void EntityExporterBase::adjustReferencedEntities()
{
	S_LOG_VERBOSE("Adjusting referenced entity ids.");
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
	for (auto& entity : mEntities) {
		auto& entityMap = entity.asMap();
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
		resolveEntityReferences(entity);
	}
}

void EntityExporterBase::resolveEntityReferences(Atlas::Message::Element& element)
{
    if (element.isMap()) {
        auto entityRefI = element.asMap().find("$eid");
        if (entityRefI != element.asMap().end() && entityRefI->second.isString()) {
            auto I = mIdMapping.find(entityRefI->second.asString());
            if (I != mIdMapping.end()) {
                entityRefI->second = I->second;
            }
        }
        //If it's a map we need to process all child elements too
        for (auto& I : element.asMap()) {
            resolveEntityReferences(I.second);
        }
    } else if (element.isList()) {
        //If it's a list we need to process all child elements too
        for (auto& I : element.asList()) {
            resolveEntityReferences(I);
        }
    }
}

void EntityExporterBase::complete()
{

	adjustReferencedEntities();

	//Make sure the rules are stored in a deterministic fashion
	std::sort(mRules.begin(), mRules.end(), [](Atlas::Message::Element const & a, Atlas::Message::Element const &b) {
		return a.asMap().find("id")->second.asString() < b.asMap().find("id")->second.asString();
	});

	Anonymous root;
	Atlas::Message::MapType meta;

	meta["name"] = mName;
	meta["description"] = mDescription;
	meta["timestamp"] = mCurrentTimestamp;
	meta["transients"] = mExportTransient;
	meta["preserved_ids"] = mPreserveIds;

	Atlas::Message::MapType server;
	fillWithServerData(server);

	meta["server"] = server;

	root->setAttr("meta", meta);

	root->setAttr("entities", mEntities);
	if (!mRules.empty()) {
		root->setAttr("rules", mRules);
	}

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
	mEntities.clear();

	mComplete = true;
	EventCompleted.emit();
	S_LOG_INFO("Completed exporting " << mStats.entitiesReceived << " entities and " << mStats.rulesReceived << " rules.");
}

void EntityExporterBase::start(const std::string& filename, const std::string& entityId)
{
	if (mComplete || mCancelled) {
		S_LOG_FAILURE("Can not restart an already completed or cancelled export instance.");
		return;
	}

	S_LOG_INFO("Starting entity dump to file '" << filename << "'.");
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
	get->setArgs1(get_arg);

	get->setFrom(mAccountId);
	get->setSerialno(newSerialNumber());

	sigc::slot<void, const Operation&> slot = sigc::mem_fun(*this, &EntityExporterBase::operationGetResult);
	sendAndAwaitResponse(get, slot);

	mStats.entitiesQueried++;
	EventProgress.emit();
}

void EntityExporterBase::operationGetResult(const Operation & op)
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
			S_LOG_WARNING("Got unexpected response on a GET request with operation of type " << op->getParent());
			S_LOG_WARNING("Error message: " << errorMessage);
			mStats.entitiesError++;
			EventProgress.emit();
		}
	}
}

void EntityExporterBase::operationGetRuleResult(const Operation & op)
{
	if (!mCancelled) {
		if (op->getArgs().empty()) {
			S_LOG_WARNING("Got response to GET for rule with no args.");
			mStats.rulesError++;
			cancel();
			return;
		}

		Root ent = smart_dynamic_cast<Root>(op->getArgs().front());
		if (!ent.isValid()) {
			S_LOG_WARNING("Malformed rule arg when dumping.");
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
							S_LOG_WARNING("Child was not a string.");
						}
					}
				}
			}
		}

		MapType ruleMap;
		ent->addToMessage(ruleMap);

		//Check if we're actually exporting rules; we might also be getting rules if we're set to ignore
		//transients, since we then need to get the types in order to know what entities are transient.
		if (mExportRules) {
		    mRules.emplace_back(ruleMap);
		}

        std::string ruleId = ruleMap.find("id")->second.asString();

		Element attributesElem;
        bool foundTransientProperty = false;
		if (ent->copyAttr("attributes", attributesElem) == 0 && attributesElem.isMap()) {
            mTypeAttributes[ruleId] = attributesElem.Map();
		    auto transientI = attributesElem.Map().find("transient");
		    if (transientI != attributesElem.Map().end() && transientI->second.isMap()) {
		        auto defaultI = transientI->second.Map().find("default");
		        if (defaultI != transientI->second.Map().end()) {
		            foundTransientProperty = true;
		            if (defaultI->second.isNum() && defaultI->second.asNum() != 0) {
		                mTransientTypes.insert(ruleId);
		            }
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

