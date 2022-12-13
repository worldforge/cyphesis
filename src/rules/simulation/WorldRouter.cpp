// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2006 Alistair Riddoch
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


#include "WorldRouter.h"

#include "rules/simulation/World.h"
#include "rules/Domain.h"

#include "common/id.h"
#include "common/debug.h"
#include "common/TypeNode.h"
#include "common/Monitors.h"
#include "common/Variable.h"
#include "common/operations/Tick.h"

#include "Remotery.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <algorithm>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

/// \brief Constructor for the world object.
WorldRouter::WorldRouter(Ref<LocatedEntity> baseEntity,
                         EntityCreator& entityCreator,
                         TimeProviderFnType timeProviderFn) :
        BaseWorld(timeProviderFn),
        m_operationsDispatcher([&](const Operation& op, Ref<LocatedEntity> from) { this->operation(op, std::move(from)); }, timeProviderFn),
        m_entityCount(1),
        m_baseEntity(std::move(baseEntity)),
        m_entityCreator(entityCreator)
{
    m_eobjects[m_baseEntity->getIntId()] = m_baseEntity;
    Monitors::instance().watch("entities", std::make_unique<Variable<int>>(m_entityCount));


}

/// \brief Destructor for the world object.
///
/// Destruction of the world object implicitly deletes all IG objects in
/// the server, clears the operation queue
WorldRouter::~WorldRouter()
{
    shutdown();
    m_operationsDispatcher.clearQueues();
    m_suspendedQueue = std::queue<OpQueEntry<LocatedEntity>>();
}

void WorldRouter::shutdown()
{
    //Make sure to clear the queues first so that there's nothing referencing entities
    //in them.
    m_operationsDispatcher.clearQueues();
    m_suspendedQueue = std::queue<OpQueEntry<LocatedEntity>>();
    m_baseEntity = nullptr;
    BaseWorld::shutdown();
}

/// \brief Add a new entity to the world.
///
/// Adds a new entity to the lists maintained by the WorldRouter.
/// Verify that the entity has a valid location, setting to
/// the default spawn area if necessary. Handle inserting the
/// entity into the loc/contains tree maintained by the Entity
/// class. Send a Setup op to the entity.
void WorldRouter::addEntity(const Ref<LocatedEntity>& ent, const Ref<LocatedEntity>& parent)
{
    debug_print("WorldRouter::addEntity(" << ent->describeEntity() << ")")
    assert(ent->getIntId() != 0);
    assert(m_eobjects.find(ent->getIntId()) == m_eobjects.end());
    m_eobjects[ent->getIntId()] = ent;
    ++m_entityCount;

    ent->changeContainer(parent);

    //debug_print("Entity loc " << ent->m_location)

    if (ent->m_contains != nullptr) {
        //Iterate through copy, to handle entities being deleted while iterating.
        auto contains = *ent->m_contains;
        for (auto& child : contains) {
            addEntity(child, ent);
        }
    }

    //Broadcast an Appearance and rely on the system sending it to any observers.
    Anonymous arg;
    Appearance app;
    arg->setId(ent->getId());
    arg->setStamp(ent->getSeq());
    app->setArgs1(arg);
    message(app, *ent);

    inserted.emit(*ent);
}

/// \brief Create a new entity and add to the world.
///
/// Construct a new entity using the entity description provided,
/// and pass it to addEntity().
/// @return a pointer to the new entity.
Ref<LocatedEntity> WorldRouter::addNewEntity(const std::string& typestr,
                                             const RootEntity& attrs)
{
    debug_print("WorldRouter::addNewEntity(\"" << typestr << "\", attrs)")
    auto id = newId();

    if (!id.isValid()) {
        log(ERROR, "Unable to get ID for new Entity");
        return nullptr;
    }

    auto ent = m_entityCreator.newEntity(id, typestr, attrs);
    if (!ent) {
        log(ERROR, String::compose("Attempt to create an entity of type \"%1\" "
                                   "but type is unknown or forbidden",
                                   typestr));
        return nullptr;
    }

    Ref<LocatedEntity> loc;
    // Get location from entity, if it is present
    // The default attributes cannot contain info on location
    if (attrs.isValid() && attrs->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        loc = getEntity(attrs->getLoc());
    }
    if (!loc) {
        log(ERROR, String::compose("Attempt to create an entity %1 without a valid parent.", ent->describeEntity()));
        return nullptr;
    }


    addEntity(ent, loc);
    return ent;
}

/// \brief Remove an entity from the world.
///
/// Remove an entity from the various lists in which it is stored.
/// The entity is removed from the LOC/CONTAINS tree, and the
/// reference held by the world is decremented. There may still be
/// a reference held by an operation in the queue from the removed
/// entity.
void WorldRouter::delEntity(LocatedEntity* ent)
{
    if (ent == m_baseEntity.get()) {
        log(WARNING, "Attempt to delete game world");
        return;
    }
    assert(ent->getIntId() != 0);
    ent->destroy();
    ent->updated.emit();
    m_eobjects.erase(ent->getIntId());
    --m_entityCount;
}

void WorldRouter::resumeWorld()
{
    //Take all suspended operations and add them to be executed.
    while (!m_suspendedQueue.empty()) {
        auto& ope = m_suspendedQueue.front();
        m_operationsDispatcher.addOperationToQueue(std::move(ope.op), std::move(ope.from));
        m_suspendedQueue.pop();
    }
}

void WorldRouter::resolveDispatchTimeForOp(Atlas::Objects::Operation::RootOperationData& op)
{
    if (!op.isDefaultFutureSeconds()) {
        double t = getTimeAsSeconds() + (op.getFutureSeconds() * consts::time_multiplier);
        op.setSeconds(t);
        op.removeAttrFlag(Atlas::Objects::Operation::FUTURE_SECONDS_FLAG);
    } else if (op.isDefaultSeconds()) {
        op.setSeconds(getTimeAsSeconds());
    }
}


/// \brief Pass an operation to the World.
///
/// Pass an operation to addOperationToQueue()
/// so it gets added to the queue for dispatch.
/// If the op is a broadcast op, it will be split up into separate ops
/// for each observer.
void WorldRouter::message(Operation op, LocatedEntity& fromEntity)
{
    debug_print("WorldRouter::message {"
                        << op->getParent() << ":"
                        << op->getFrom() << ":" << op->getTo() << "}")

    if (op->isDefaultTo()) {
        if (shouldBroadcastPerception(op)) {
            OpVector res;
            resolveDispatchTimeForOp(*op);
            fromEntity.broadcast(op, res, Visibility::PUBLIC);

            for (auto& broadcastedOp : res) {
                m_operationsDispatcher.addOperationToQueue(std::move(broadcastedOp), Ref<LocatedEntity>(&fromEntity));
            }
        } else {
            //Don't broadcast ops which shouldn't be broadcasted.
            log(WARNING, String::compose("Trying to broadcast '%1' op from %2, which we don't allow. Did you forget to set 'to' on the op?",
                                         op->getParent(),
                                         fromEntity.describeEntity()));

        }
    } else {
        resolveDispatchTimeForOp(*op);
        m_operationsDispatcher.addOperationToQueue(std::move(op), Ref<LocatedEntity>(&fromEntity));
    }

}

bool WorldRouter::shouldBroadcastPerception(const Operation& op) const
{
    int op_class = op->getClassNo();
    return op_class == Atlas::Objects::Operation::SIGHT_NO ||
           op_class == Atlas::Objects::Operation::SOUND_NO ||
           op_class == Atlas::Objects::Operation::APPEARANCE_NO ||
           op_class == Atlas::Objects::Operation::DISAPPEARANCE_NO;
}

/// \brief Deliver an operation to its target.
///
/// Pass the operation to the target entity. The resulting operations
/// have their ref numbers set, and are added to the queue for
/// dispatch.
void WorldRouter::deliverTo(const Operation& op, Ref<LocatedEntity> ent)
{
    //If the world is suspended and the op is a tick, we should store it
    //(to be resent when the world is resumed) and not process it now.
    if (m_isSuspended) {
        if (op->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
            m_suspendedQueue.push(OpQueEntry<LocatedEntity>(op, std::move(ent), m_suspendedQueue.size()));
            return;
        }
    }
    //Set the time of when this op is dispatched. That way, other components in the system can
    //always use the seconds set on the op to know the current time.
    op->setSeconds(std::chrono::duration_cast<std::chrono::duration<double>>(getTime()).count());

    OpVector res;
    debug_print("WorldRouter::deliverTo begin {"
                    << op->getParent() << ":"
                    << op->getFrom() << ":" << op->getTo() << "}")
    ent->operation(op, res);
    debug_print("WorldRouter::deliverTo done {"
                    << op->getParent() << ":"
                    << op->getFrom() << ":" << op->getTo() << "}")
    for (auto& resOp : res) {
        if (op->getFrom() == resOp->getTo()) {
            if (!op->isDefaultSerialno() && resOp->isDefaultRefno()) {
                resOp->setRefno(op->getSerialno());
            }
        }
        //If nothing is specified, resulting ops are sent from the active entity.
        if (resOp->isDefaultFrom() || resOp->getFrom() == ent->getId()) {
            message(resOp, *ent);
        } else {
            auto fromEntity = getEntity(resOp->getFrom());
            if (fromEntity) {
                message(resOp, *fromEntity);
            } else {
                log(WARNING, String::compose("Resulting operation %1, from sending op of type %2 to entity '%3' was marked as being from entity with id '%4' which doesn't exist.",
                                             resOp->getParent(),
                                             op->getParent(),
                                             ent->describeEntity(),
                                             resOp->getFrom()));
            }
        }
    }
}

/// \brief Main in-game operation dispatch function.
///
/// Operations are passed here when they are due for dispatch.
/// Determine the target of the operation and deliver it directly.
/// @param op operation to be dispatched to the world.
/// @param from entity the operation to be dispatched was send from. Note
/// that it is possible that this entity has been destroyed.
void WorldRouter::operation(const Operation& op, Ref<LocatedEntity> from)
{
    m_operationsCount++;
    try {
        rmt_ScopedCPUSample(WorldRouter_operation, 0)

        debug_print("WorldRouter::operation {"
                            << op->getParent() << ":"
                            << op->getFrom() << ":" << op->getTo() << "}")
        assert(op->getFrom() == from->getId());
        assert(!op->getParent().empty());

        Dispatching.emit(op);

        if (!op->isDefaultTo()) {
            const std::string& to = op->getTo();
            if (to.empty()) {
                log(ERROR, String::compose("Op with 'to' set to an empty string. From %1. Op: %2", from->describeEntity(), debug_tostring(op)));
                return;
            }
            Ref<LocatedEntity> to_entity;

            if (to == from->getId()) {
                if (from->isDestroyed()) {
                    // Entity no longer exists, don't send anything
                    return;
                }
                to_entity = std::move(from);
            } else {
                to_entity = getEntity(to);

                if (to_entity == nullptr || to_entity->isDestroyed()) {
                    // Entity has been removed, send an Unseen op back to the observer
                    // But check that the observer hasn't been destroyed (to avoid infinite loops)
                    if (!from->isDestroyed()) {
                        Atlas::Objects::Operation::Unseen unseen;
                        Atlas::Objects::Entity::Anonymous ent;
                        ent->setId(to);
                        ent->setAttr("destroyed", 1); //Add attribute clarifying that this entity is destroyed.
                        unseen->setArgs1(ent);
                        unseen->setTo(from->getId());
                        if (!op->isDefaultSerialno()) {
                            unseen->setRefno(op->getSerialno());
                        }
                        message(unseen, *from);
                    }
                    return;
                }
            }

            deliverTo(op, std::move(to_entity));

        } else {
            //This will send an op to all entities in the system. Perhaps we should add some more checks for when we want to allow for this?
            for (auto& entry : m_eobjects) {
                op->setTo(entry.second->getId());
                deliverTo(op, entry.second);
            }
        }
    }
    catch (const std::exception& ex) {
        log(ERROR, String::compose("Exception caught in OperationsDispatcher::dispatchOperation() "
                                   "thrown while processing operation "
                                   "sent to \"%1\" from \"%2\": %3",
                                   op->getTo(), op->getFrom(), ex.what()));
    }
    catch (...) {
        log(ERROR, String::compose("Unspecified exception caught in OperationsDispatcher::dispatchOperation() "
                                   "thrown while processing operation "
                                   "sent to \"%1\" from \"%2\"",
                                   op->getTo(), op->getFrom()));
    }
}

/// Find an entity of the given name. This is provided to allow administrators
/// to perform certain admin tasks. It finds and returns the first instance
/// with the name provided in the game world.
/// @param name string specifying name of the instance required.
/// @return a pointer to an entity with the type required, or zero if an
/// instance with this name was not found.
Ref<LocatedEntity> WorldRouter::findByName(const std::string& name)
{
    Element name_attr;
    for (auto& entry : m_eobjects) {
        if (entry.second->getAttr("name", name_attr) == 0) {
            if (name_attr == name) {
                return entry.second;
            }
        }
    }
    return nullptr;
}

/// Find an entity of the given type. This is provided to allow administrators
/// to perform certain admin tasks. It finds and returns the first instance
/// of the type provided in the game world.
/// @param type string specifying the class name of the instance required.
/// @return a pointer to an entity of the type required, or zero if no
/// instance was found.
Ref<LocatedEntity> WorldRouter::findByType(const std::string& type)
{
    for (auto& entry : m_eobjects) {
        if (entry.second->getType()->name() == type) {
            return entry.second;
        }
    }
    return nullptr;
}

OperationsDispatcher<LocatedEntity>& WorldRouter::getOperationsHandler()
{
    return m_operationsDispatcher;
}

const std::set<std::string>& WorldRouter::getSpawnEntities() const
{
    return m_spawnEntities;
}

void WorldRouter::registerSpawner(const std::string& id)
{
    m_spawnEntities.insert(id);
}

void WorldRouter::unregisterSpawner(const std::string& id)
{
    m_spawnEntities.erase(id);
}
