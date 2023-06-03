//
// Copyright (C) 2009 Alistair Riddoch
// Copyright (C) 2013 Erik Ogenvik
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

#include "EntityImporterBase.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <sstream>
#include <iostream>


using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Set;
using Atlas::Message::Element;

//The following are adapters to make the Ember logging syntax work with the Cyphesis log.
#include "common/log.h"
#include "common/debug.h"

static const bool debug_flag = true;
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


StackEntry::StackEntry(Atlas::Objects::Entity::RootEntity o, const std::vector<std::string>::const_iterator& c) :
        obj(std::move(o)),
        currentChildIterator(c)
{
}

StackEntry::StackEntry(Atlas::Objects::Entity::RootEntity o) :
        obj(std::move(o))
{
    currentChildIterator = obj->getContains().end();
}

bool EntityImporterBase::getEntity(const std::string& id, OpVector& res)
{
    auto I = mPersistedEntities.find(id);
    if (I == mPersistedEntities.end()) {
        S_LOG_VERBOSE("Could not find entity with id " << id << "; this one was probably transient.")
        //This will often happen if the child entity was transient, and therefore wasn't exported (but is still references from the parent entity).
        return false;
    }
    const RootEntity& obj = smart_dynamic_cast<RootEntity>(I->second.obj);
    if (!obj.isValid()) {
        S_LOG_FAILURE("Corrupt dump - non entity found " << id << ".")
        return false;
    }

    m_state = ENTITY_WALKING;
    mTreeStack.emplace_back(obj);

    Anonymous get_arg;
    get_arg->setId(id);
    get_arg->setObjtype("obj");

    Get get;
    get->setArgs1(get_arg);
    get->setFrom(mAccountId);
    get->setSerialno(newSerialNumber());
    res.push_back(get);
    S_LOG_VERBOSE("EntityImporterBase: Getting entity with id " << id)
    return true;
}

bool EntityImporterBase::getOrCreateEntity(const std::string& id, OpVector& res)
{
    if (mAlwaysCreateNewEntities && id != "0") {
        auto createI = mPersistedEntities.find(id);
        if (createI == mPersistedEntities.end()) {
            S_LOG_FAILURE("Could not find entity " << id << ".")
            return false;
        } else {
            const RootEntity& obj = smart_dynamic_cast<RootEntity>(createI->second.obj);
            m_state = ENTITY_WALKING;
            mTreeStack.emplace_back(obj);
            createEntity(obj, res);
            return true;
        }
    } else {
        return getEntity(id, res);
    }
}

void EntityImporterBase::walkEntities(OpVector& res)
{
    if (mTreeStack.empty()) {
        sendResolvedEntityReferences();
    } else {
        StackEntry& current = mTreeStack.back();
        //Check if there are any children. If not, we should pop the stack and
        if (current.obj->getContains().empty()) {
            // Pop: Go back to WALKING parent
            assert(!mTreeStack.empty());
            mTreeStack.pop_back();
            while (!mTreeStack.empty()) {
                StackEntry& se = mTreeStack.back();
                //Try to get the next child entity (unless we've reached the end of the list of children).
                //Since some entities are references but not persisted we need to loop until we find one that we know of.
                for (; ++se.currentChildIterator, se.currentChildIterator != se.obj->getContains().end();) {
                    if (getOrCreateEntity(*se.currentChildIterator, res)) {
                        return;
                    }
                }

                //If we've reached the end of the contained entities we should pop the current stack entry.
                mTreeStack.pop_back();
            }
        } else {
            //Iterate until we find an entity that's persisted.
            //Note that there might be a reference to an entity in CONTAINS which can't be found
            //in the list of entities; this happens with transient entities.
            for (auto I = current.obj->getContains().begin(); I != current.obj->getContains().end(); ++I) {
                current.currentChildIterator = I;
                if (getOrCreateEntity(*current.currentChildIterator, res)) {
                    return;
                }
            }
        }
        if (mTreeStack.empty()) {
            sendResolvedEntityReferences();
        }
    }
}

void EntityImporterBase::sendResolvedEntityReferences()
{
    m_state = ENTITY_REF_RESOLVING;
    if (!mEntitiesWithReferenceAttributes.empty()) {
        S_LOG_INFO("Resolving entity references for " << mEntitiesWithReferenceAttributes.size() << " entities.")
        for (auto entryI : mEntitiesWithReferenceAttributes) {
            const auto& persistedEntityId = entryI.first;
            const auto& referenceEntries = entryI.second;

            auto createdEntityI = mEntityIdMap.find(persistedEntityId);
            if (createdEntityI == mEntityIdMap.end()) {
                S_LOG_WARNING("Could not find final server side entity id for persisted entity " << persistedEntityId << " when doing entity ref resolving.")
                continue;
            }
            const auto& createdEntityId = createdEntityI->second;

            //This should not fail at this phase, so we're not doing any checks.
            auto& persistedEntity = mPersistedEntities.find(persistedEntityId)->second;

            RootEntity entity;

            for (const auto& referenceEntry : referenceEntries) {
                Element element = persistedEntity.obj->getAttr(referenceEntry.propertyName);
                resolveEntityReferences(element);
                entity->setAttr(referenceEntry.propertyName, element);
            }

            Set set;
            set->setFrom(mAvatarId);
            set->setSerialno(newSerialNumber());
            set->setTo(createdEntityId);
            set->setArgs1(entity);

            mSetOpsInTransit++;
            sigc::slot<void(const Operation&)> slot = sigc::mem_fun(*this, &EntityImporterBase::operationSetResult);
            sendAndAwaitResponse(set, slot);
        }
    } else {
        complete();
    }
}

void EntityImporterBase::resolveEntityReferences(Atlas::Message::Element& element)
{
    if (element.isMap()) {
        auto entityRefI = element.asMap().find("$eid");
        if (entityRefI != element.asMap().end() && entityRefI->second.isString()) {
            auto I = mEntityIdMap.find(entityRefI->second.asString());
            if (I != mEntityIdMap.end()) {
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


void EntityImporterBase::complete()
{
    S_LOG_INFO("Restore done.")
    S_LOG_INFO("Restored " << mStats.entitiesProcessedCount << ", created: "
                           << mStats.entitiesCreateCount << ", updated: "
                           << mStats.entitiesUpdateCount << ", create errors: "
                           << mStats.entitiesCreateErrorCount << " .")
    EventCompleted.emit();
}

void EntityImporterBase::createEntity(const RootEntity& obj, OpVector& res)
{
    ++mStats.entitiesProcessedCount;
    ++mStats.entitiesCreateCount;
    EventProgress.emit();

    m_state = ENTITY_CREATING;

    assert(mTreeStack.size() > 1);
    auto I = mTreeStack.rbegin();
    ++I;
    assert(I != mTreeStack.rend());
    const std::string& loc = I->restored_id;

    RootEntity create_arg = obj.copy();

    create_arg->removeAttrFlag(Atlas::Objects::Entity::CONTAINS_FLAG);
    create_arg->removeAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG);
    create_arg->removeAttrFlag(Atlas::Objects::ID_FLAG);
    create_arg->setLoc(loc);

    //Remove any attribute which references another entity from the Create op.
    //This is because the attribute will at this time with certainty refer to the wrong or a non-existing entity.
    //The attribute will later on be set through a Set op in sendResolvedEntityReferences().
    auto referenceMapEntryI = mEntitiesWithReferenceAttributes.find(obj->getId());
    if (referenceMapEntryI != mEntitiesWithReferenceAttributes.end()) {
        std::set<std::string> resolvedAttributes;
        for (const auto& referenceEntry : referenceMapEntryI->second) {
            size_t resolvedEntitiesCount = 0;
            //Check if all the referenced entities perhaps already have been created.
            for (const auto& entityId : referenceEntry.referencedEntities) {
                auto resolvedI = mEntityIdMap.find(entityId);
                if (resolvedI != mEntityIdMap.end()) {
                    resolvedEntitiesCount++;
                }
            }

            //If all entities were resolved, we should resolve the property now.
            if (resolvedEntitiesCount == referenceEntry.referencedEntities.size()) {
                Element element = create_arg->getAttr(referenceEntry.propertyName);
                resolveEntityReferences(element);
                create_arg->setAttr(referenceEntry.propertyName, element);
                resolvedAttributes.insert(referenceEntry.propertyName);
            } else {
                create_arg->removeAttr(referenceEntry.propertyName);
            }
        }
        //Remove those attributes that were resolved
        if (resolvedAttributes.size() == referenceMapEntryI->second.size()) {
            //All attributes were resolved, remove the entry completely.
            mEntitiesWithReferenceAttributes.erase(referenceMapEntryI);
        } else {
            //Only remove those entries that were destroyed.
            std::vector<ReferencedEntry> copy;
            for (auto& referenceEntry : referenceMapEntryI->second) {
                if (resolvedAttributes.find(referenceEntry.propertyName) == resolvedAttributes.end()) {
                    copy.push_back(std::move(referenceEntry));
                }
            }
            referenceMapEntryI->second = std::move(copy);
        }

    }

    Create create;
    create->setArgs1(create_arg);
    create->setFrom(mAvatarId);
    create->setSerialno(newSerialNumber());

    mCreateEntityMapping.emplace(create->getSerialno(), obj->getId());

    res.push_back(create);
}


void EntityImporterBase::errorArrived(const Operation& op, OpVector& res)
{
    std::string errorMessage;
    if (!op->getArgs().empty()) {
        auto arg = op->getArgs().front();
        if (arg->hasAttr("message")) {
            const Atlas::Message::Element messageElem = arg->getAttr("message");
            if (messageElem.isString()) {
                errorMessage = messageElem.asString();
            }
        }
    }

    switch (m_state) {
        case ENTITY_WALKING: {
            //An error here just means that the entity we asked for didn't exist on the server, and we need
            //to create it. This is an expected result.
            assert(!mTreeStack.empty());
            StackEntry& current = mTreeStack.back();
            const RootEntity& obj = current.obj;

            assert(obj.isValid());

            createEntity(obj, res);
        }
            break;
        case ENTITY_CREATING: {
            //An error here means that something went wrong when trying to create an entity. This is wrong.
            //It probably means that there's something wrong with the data we're sending. Either the
            //persisted data is corrupt, or there have been changes on the server (for example entity types
            //renamed or removed).
            std::string entityType = "unknown";

            auto I = mCreateEntityMapping.find(op->getRefno());
            if (I != mCreateEntityMapping.end()) {
                auto J = mPersistedEntities.find(I->second);
                if (J != mPersistedEntities.end()) {
                    auto& entity = J->second;
                    entityType = entity.obj->getParent();
                }
            }
            S_LOG_FAILURE("Could not create entity of type '" << entityType << "', continuing with next. Server message: " << errorMessage)
            mStats.entitiesCreateErrorCount++;
            EventProgress.emit();
            walkEntities(res);
        }
            break;
        default: S_LOG_FAILURE("Unexpected state in state machine: " << m_state << ". Server message: " << errorMessage)
            break;
    }
}

void EntityImporterBase::infoArrived(const Operation& op, OpVector& res)
{
    if (op->isDefaultRefno()) {
        return;
    }
    if (op->isDefaultArgs() || op->getArgs().empty()) {
        S_LOG_FAILURE("Info with no arg.")
        return;
    }
    const Root& arg = op->getArgs().front();

    if (m_state == ENTITY_CREATING) {
        if (!op.isValid()) {
            return;
        }
        mNewIds.insert(arg->getId());
        StackEntry& current = mTreeStack.back();
        current.restored_id = arg->getId();
        S_LOG_VERBOSE("Created: " << arg->getParent() << "(" << arg->getId() << ")")

        auto I = mCreateEntityMapping.find(op->getRefno());
        if (I != mCreateEntityMapping.end()) {
            mEntityIdMap.emplace(I->second, arg->getId());
            mCreateEntityMapping.erase(op->getRefno());
        } else {
            S_LOG_WARNING("Got info about create for an entity which we didn't seem to have sent.")
        }

        walkEntities(res);
    } else if (m_state == ENTITY_WALKING) {
        const RootEntity& ent = smart_dynamic_cast<RootEntity>(arg);
        if (!ent.isValid()) {
            S_LOG_FAILURE("Info response is not entity.")
            return;
        }
        if (arg->isDefaultId()) {
            S_LOG_FAILURE("Corrupted info response: no id.")
        }
        const std::string& id = arg->getId();

        StackEntry& current = mTreeStack.back();
        const RootEntity& obj = current.obj;

        assert(id == obj->getId());

        if (mNewIds.find(id) != mNewIds.end() || (mTreeStack.size() != 1 && ent->isDefaultLoc()) || ent->getParent() != obj->getParent()) {
            createEntity(obj, res);
        } else {

            Root update = obj.copy();

            current.restored_id = id;

            S_LOG_VERBOSE("Updating: " << obj->getId() << " ," << obj->getParent())

            update->removeAttrFlag(Atlas::Objects::Entity::CONTAINS_FLAG);
            update->removeAttrFlag(Atlas::Objects::STAMP_FLAG);

            Set set;
            set->setArgs1(update);
            set->setFrom(mAvatarId);
            set->setTo(id);
            set->setSerialno(newSerialNumber());

            res.push_back(set);

            ++mStats.entitiesProcessedCount;
            ++mStats.entitiesUpdateCount;
            EventProgress.emit();

            m_state = ENTITY_UPDATING;
        }
    }
}

void EntityImporterBase::sightArrived(const Operation& op, OpVector& res)
{
    if (op->isDefaultArgs() || op->getArgs().empty()) {
        S_LOG_FAILURE("No arg")
        return;
    }
    const Root& arg = op->getArgs().front();
    switch (m_state) {
        case ENTITY_WALKSTART:
            if (op->isDefaultRefno()) {
                break;
            }
            if (arg->isDefaultId()) {
                S_LOG_WARNING("Corrupted top level entity: no id")
                cancel();
                return;
            } else {
                getEntity(arg->getId(), res);
            }


            // Expecting sight of world root
            break;
        case ENTITY_UPDATING: {
            const Operation& sub_op = smart_dynamic_cast<Operation>(arg);
            if (!sub_op.isValid()) {
                break;
            }
            if (sub_op->getClassNo() != Atlas::Objects::Operation::SET_NO || sub_op->getArgs().empty() || sub_op->isDefaultSerialno()) {
                S_LOG_FAILURE("This is not our entity update response.")
                break;
            }
            walkEntities(res);
        }
            break;
        case ENTITY_CREATING:
        case ENTITY_WALKING:
        case ENTITY_REF_RESOLVING:
            //Just ignore sights when creating; these are sights of the actual creation ops.
            break;
        default: S_LOG_WARNING("Unexpected state in state machine: " << m_state)
            break;
    }
}

EntityImporterBase::EntityImporterBase(std::string accountId, std::string avatarId) :
        mAccountId(std::move(accountId)),
        mAvatarId(std::move(avatarId)),
        mStats({}),
        m_state(INIT),
        mSetOpsInTransit(0),
        mResumeWorld(false),
        mSuspendWorld(false),
        mAlwaysCreateNewEntities(false)
{
}

void EntityImporterBase::start(const std::string& filename)
{
    S_LOG_VERBOSE("Starting import from " << filename)
    Atlas::Objects::Factories factories;

    auto rootObj = loadFromFile(filename);

    if (!rootObj.isValid()) {
        EventCompleted.emit();
        return;
    }
    Atlas::Message::Element metaElem;
    Atlas::Message::Element entitiesElem;
    Atlas::Message::Element rulesElem;
    rootObj->copyAttr("meta", metaElem);
    rootObj->copyAttr("entities", entitiesElem);

    if (!entitiesElem.isNone() && !entitiesElem.isList()) {
        S_LOG_WARNING("Entities element is not list.")
        EventCompleted.emit();
        return;
    }

    if (!entitiesElem.isNone()) {
        auto& topEntitiesList = entitiesElem.asList();

        auto topChildIds = extractChildEntities(factories, topEntitiesList);
    }

    if (mResumeWorld && mSuspendWorld) {
        S_LOG_WARNING("Trying to both suspend and resume the world. The end result will be a suspended world.")
    }

    //If we should resume the world, check if the world has a "suspended" property,
    //and disable it if so.
    if (mResumeWorld) {
        auto I = mPersistedEntities.find("0");
        if (I != mPersistedEntities.end()) {
            if (I->second.obj->hasAttr("suspended")) {
                I->second.obj->setAttr("suspended", 0);
                S_LOG_INFO("Resuming suspended world.")
            }
        }
    }
    if (mSuspendWorld) {
        auto I = mPersistedEntities.find("0");
        if (I != mPersistedEntities.end()) {
            I->second.obj->setAttr("suspended", 1);
            S_LOG_INFO("Suspending world.")
        }
    }


    S_LOG_INFO("Starting loading of world. Number of entities: " << mPersistedEntities.size())
    mStats.entitiesCount = static_cast<unsigned int>(mPersistedEntities.size());

    EventProgress.emit();

    startEntityWalking();

}

std::vector<std::string> EntityImporterBase::extractChildEntities(Atlas::Objects::Factories& factories, Atlas::Message::ListType contains)
{
    std::vector<std::string> ids;

    for (auto& entityMessage : contains) {
        if (entityMessage.isMap()) {
            auto& entityMap = entityMessage.asMap();
            auto containsI = entityMap.find("~contains");
            Atlas::Message::ListType containsList;
            if (containsI != entityMap.end()) {
                containsList = std::move(containsI->second.asList());
                entityMap.erase(containsI);
            }
            auto object = smart_dynamic_cast<RootEntity>(factories.createObject(entityMap));
            if (object.isValid()) {
                if (!object->isDefaultId()) {
                    ids.emplace_back(object->getId());
                    registerEntityReferences(object->getId(), entityMap);

                    auto childIds = extractChildEntities(factories, containsList);
                    if (!childIds.empty()) {
                        auto& containsProp = object->modifyContains();
                        for (auto& childId : childIds) {
                            containsProp.emplace_back(childId);
                        }
                    }
                    mPersistedEntities.emplace(object->getId(), EntityEntry{object, childIds});
                }
            }
        }

    }

    return ids;
}


void EntityImporterBase::registerEntityReferences(const std::string& id, const Atlas::Message::MapType& element)
{
    for (const auto& I : element) {
        const auto& name = I.first;
        if (name == "id" || name == "parent" || name == "contains" || name == "~contains") {
            continue;
        }
        auto res = extractEntityReferences(I.second);
        if (!res.empty()) {
            mEntitiesWithReferenceAttributes[id].push_back({name, std::move(res)});
        }
    }
}

std::set<std::string> EntityImporterBase::extractEntityReferences(const Element& element) const
{
    std::set<std::string> ids;
    if (element.isMap()) {
        auto entityRefI = element.asMap().find("$eid");
        if (entityRefI != element.asMap().end() && entityRefI->second.isString()) {
            ids.insert(entityRefI->second.String());
        }
        //If it's a map we need to process all child elements too
        for (auto& I : element.asMap()) {
            auto res = extractEntityReferences(I.second);
            ids.insert(res.begin(), res.end());
        }
    } else if (element.isList()) {
        //If it's a list we need to process all child elements too
        for (auto& I : element.asList()) {
            auto res = extractEntityReferences(I);
            ids.insert(res.begin(), res.end());
        }
    }
    return ids;
}


void EntityImporterBase::startEntityWalking()
{
    m_state = ENTITY_WALKSTART;
    Look l;

    l->setFrom(mAvatarId);
    l->setSerialno(newSerialNumber());

    sendOperation(l);

}

void EntityImporterBase::sendOperation(const Operation& op)
{
    if (!op->isDefaultSerialno()) {
        sigc::slot<void(const Operation&)> slot = sigc::mem_fun(*this, &EntityImporterBase::operation);
        sendAndAwaitResponse(op, slot);
    } else {
        send(op);
    }
}

void EntityImporterBase::cancel()
{
    m_state = CANCEL;
}

const EntityImporterBase::Stats& EntityImporterBase::getStats() const
{
    return mStats;
}

void EntityImporterBase::setResume(bool enabled)
{
    mResumeWorld = enabled;
}

void EntityImporterBase::setSuspend(bool enabled)
{
    mSuspendWorld = enabled;
}

void EntityImporterBase::setAlwaysCreateNewEntities(bool alwaysCreateNew)
{
    mAlwaysCreateNewEntities = alwaysCreateNew;
}

void EntityImporterBase::operationSetResult(const Operation& op)
{
    mSetOpsInTransit--;
    if (m_state == ENTITY_REF_RESOLVING && mSetOpsInTransit == 0) {
        complete();
    }
}

void EntityImporterBase::operation(const Operation& op)
{

    if (m_state == CANCEL) {
        m_state = CANCELLED;
        return;
    }
    if (m_state == CANCELLED) {
        return;
    }
    OpVector res;
    if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
        infoArrived(op, res);
    } else if (op->getClassNo() == Atlas::Objects::Operation::ERROR_NO) {
        errorArrived(op, res);
    } else if (op->getClassNo() == Atlas::Objects::Operation::SIGHT_NO) {
        sightArrived(op, res);
    }

    for (auto& resOp : res) {
        sendOperation(resOp);
    }

}
