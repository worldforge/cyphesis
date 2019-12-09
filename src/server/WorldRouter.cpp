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

#include "ArithmeticBuilder.h"
#include "EntityBuilder.h"
#include "SpawnEntity.h"

#include "rules/simulation/World.h"
#include "rules/Domain.h"
#include "rules/simulation/Task.h"
#include "rules/simulation/ArithmeticScript.h"

#include "common/id.h"
#include "common/debug.h"
#include "common/TypeNode.h"
#include "common/Inheritance.h"
#include "common/Monitors.h"
#include "common/Variable.h"
#include "common/operations/Tick.h"
#include "server/Connection.h"
#include "ServerRouting.h"
#include "Account.h"

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
WorldRouter::WorldRouter(Ref<LocatedEntity> baseEntity, EntityBuilder& entityBuilder) :
        BaseWorld(),
        m_operationsDispatcher([&](const Operation& op, Ref<LocatedEntity> from) { this->operation(op, std::move(from)); }, [&]() -> double { return getTime(); }),
        m_entityCount(1),
        m_baseEntity(std::move(baseEntity)),
        m_entityBuilder(entityBuilder)
{
    m_eobjects[m_baseEntity->getIntId()] = m_baseEntity;
    Monitors::instance().watch("entities", new Variable<int>(m_entityCount));

    /**
     * When types are updated we will send an "change" op to all connected clients.
     */
    Inheritance::instance().typesUpdated.connect([&](const std::map<const TypeNode*, TypeNode::PropertiesUpdate> typeNodes) {
        //Send Change ops to all clients
        if (!typeNodes.empty()) {
            Atlas::Objects::Operation::Change change;
            std::vector<Atlas::Objects::Root> args;
            for (auto& entry: typeNodes) {
                auto typeNode = entry.first;
                Atlas::Objects::Entity::Anonymous o;
                o->setObjtype(typeNode->description(Visibility::PRIVATE)->getObjtype());
                o->setId(typeNode->name());
                args.emplace_back(o);
            }
            change->setArgs(args);

            messageToClients(change);

            //Go through all world entities and check if they need to be updated
            for (auto& entry : m_eobjects) {
                auto entity = entry.second;
                auto I = typeNodes.find(entity->getType());
                if (I != typeNodes.end()) {
                    auto typeNode = I->first;
//                    for (auto& removedPropName : I->second.removedProps) {
//                        if (entity->getProperties().find(removedPropName) == entity->getProperties().end()) {
//                            auto prop = typeNode->defaults().find(removedPropName)->second;
//                            prop->remove(entity, removedPropName);
//                        }
//                    }
                    for (auto& changedPropName : I->second.changedProps) {
                        if (entity->getProperties().find(changedPropName) == entity->getProperties().end()) {
                            auto& prop = typeNode->defaults().find(changedPropName)->second;
                            prop->apply(entity.get());
                            entity->propertyApplied(changedPropName, *prop);
                        }
                    }
                    for (auto& newPropName : I->second.changedProps) {
                        if (entity->getProperties().find(newPropName) == entity->getProperties().end()) {
                            auto& prop = typeNode->defaults().find(newPropName)->second;
                            prop->apply(entity.get());
                            entity->propertyApplied(newPropName, *prop);
                        }
                    }
                }
            }
        }
    });
}

/// \brief Destructor for the world object.
///
/// Destruction of the world object implicitly deletes all IG objects in
/// the server, clears the operation queue
WorldRouter::~WorldRouter()
{
    m_operationsDispatcher.clearQueues();
    m_suspendedQueue = std::queue<OpQueEntry<LocatedEntity>>();

    m_spawns.clear();

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
void WorldRouter::addEntity(const Ref<LocatedEntity>& ent)
{
    debug_print("WorldRouter::addEntity(" << ent->describeEntity() << ")")
    assert(ent->getIntId() != 0);
    m_eobjects[ent->getIntId()] = ent;
    ++m_entityCount;

    if (!ent->m_location.isValid()) {
        log(ERROR, String::compose("Entity %1 added to world with invalid location!", ent->describeEntity()));
        debug_print("set loc " << &getDefaultLocation())
        ent->m_location.m_parent = &getDefaultLocation();
//        ent->m_location.m_pos = Point3D(uniform(-8,8), uniform(-8,8), 0);
        ent->m_location.m_pos = Point3D::ZERO();
        debug_print("loc set with loc " << ent->m_location.m_parent->getId())
    }
    ent->m_location.update(getTime());

    ent->m_location.m_parent->addChild(*ent);

    debug_print("Entity loc " << ent->m_location)

    if (ent->m_contains != nullptr) {
        //Iterate through copy, to handle entities being deleted while iterating.
        auto contains = *ent->m_contains;
        for (auto& child : contains) {
            addEntity(child);
        }
    }

    Anonymous arg;
    Appearance app;
    arg->setId(ent->getId());
    arg->setStamp(ent->getSeq());
    app->setArgs1(arg);
    message(app, *ent);

    inserted.emit(ent.get());
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
    std::string id;
    long intId = newId(id);

    if (intId < 0) {
        log(ERROR, "Unable to get ID for new Entity");
        return nullptr;
    }

    auto ent = m_entityBuilder.newEntity(id, intId, typestr, attrs, *this);
    if (!ent) {
        log(ERROR, String::compose("Attempt to create an entity of type \"%1\" "
                                   "but type is unknown or forbidden",
                                   typestr));
        return nullptr;
    }
    addEntity(ent);
    return ent;
}

int WorldRouter::createSpawnPoint(const MapType& data, LocatedEntity* ent)
{
    auto I = data.find("name");
    if (I == data.end() || !I->second.isString()) {
        log(ERROR, "No name on spawn point");
        return -1;
    }
    auto new_spawn = std::make_unique<SpawnEntity>(ent);
    if (new_spawn->setup(data) != 0) {
        log(ERROR, "Error setting up spawn point");
        return -1;
    }

    const std::string& name = I->second.String();
    auto J = m_spawns.find(name);
    if (J != m_spawns.end()) {
        J->second.first = std::move(new_spawn);
        J->second.second = ent->getId();
    } else {
        m_spawns.insert(std::make_pair(name, std::make_pair(std::move(new_spawn), ent->getId())));
    }
    return 0;
}

int WorldRouter::removeSpawnPoint(LocatedEntity* ent)
{
    for (auto I = m_spawns.begin(); I != m_spawns.end(); ++I) {
        if (I->second.second == ent->getId()) {
            m_spawns.erase(I);
            return 0;
        }
    }
    return 1;
}


int WorldRouter::getSpawnList(Atlas::Message::ListType& data)
{
    for (const auto& entry : m_spawns) {
        MapType spawn;
        spawn.insert(std::make_pair("name", entry.first));
        entry.second.first->addToMessage(spawn);
        data.push_back(spawn);
    }
    return 0;
}

Ref<LocatedEntity> WorldRouter::spawnNewEntity(const std::string& name,
                                               const std::string& type,
                                               const RootEntity& desc)
{
    auto I = m_spawns.find(name);
    if (I == m_spawns.end()) {
        log(ERROR, String::compose("Spawn not found %1", name));
        return nullptr;
    }
    auto& s = I->second.first;
    int ret = s->spawnEntity(type, desc);
    if (ret != 0) {
        log(ERROR, String::compose("Spawn not permitting %1", type));
        return nullptr;
    }
    auto e = addNewEntity(type, desc);
    if (!e) {
        log(ERROR, String::compose("Entity creation failed %1", type));
        return e;
    }

    return e;
}

int WorldRouter::moveToSpawn(const std::string& name, Location& location)
{
    auto I = m_spawns.find(name);
    if (I == m_spawns.end()) {
        log(ERROR, String::compose("Spawn not found %1", name));
        return -10;
    }
    return I->second.first->placeInSpawn(location);
}


std::unique_ptr<ArithmeticScript> WorldRouter::newArithmetic(const std::string& name,
                                                             LocatedEntity* owner)
{
    return ArithmeticBuilder::instance().newArithmetic(name, owner);
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


/// \brief Pass an operation to the World.
///
/// Pass an operation to addOperationToQueue()
/// so it gets added to the queue for dispatch.
/// If the op is a broadcast op, it will be split up into separate ops
/// for each observer.
void WorldRouter::message(const Operation& op, LocatedEntity& fromEntity)
{
    if (op->isDefaultTo()) {
        if (shouldBroadcastPerception(op)) {
            OpVector res;
            fromEntity.broadcast(op, res, Visibility::PUBLIC);
            for (auto& broadcastedOp : res) {
                m_operationsDispatcher.addOperationToQueue(broadcastedOp, Ref<LocatedEntity>(&fromEntity));
            }
        } else {
            //Don't broadcast ops which shouldn't be broadcasted.
            log(WARNING, String::compose("Trying to broadcast '%1' op from %2, which we don't allow.",
                                         op->getParent(),
                                         fromEntity.describeEntity()));

        }
    } else {
        m_operationsDispatcher.addOperationToQueue(op, Ref<LocatedEntity>(&fromEntity));
    }
    debug_print("WorldRouter::message {"
                        << op->getParent() << ":"
                        << op->getFrom() << ":" << op->getTo() << "}")
}

void WorldRouter::messageToClients(const Operation& op)
{
    auto& accounts = ServerRouting::instance().getAccounts();
    OpVector res;
    for (auto& entry : accounts) {
        entry.second->operation(op, res);
    }

    debug_print("WorldRouter::messageToClients {"
                        << op->getParent() << ":"
                        << op->getFrom() << ":" << op->getTo() << "}")
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
            m_suspendedQueue.push(OpQueEntry<LocatedEntity>(op, std::move(ent)));
            return;
        }
    }
    OpVector res;
    debug(std::cout << "WorldRouter::deliverTo begin {"
                    << op->getParent() << ":"
                    << op->getFrom() << ":" << op->getTo() << "}" << std::endl
                    << std::flush;)
    ent->operation(op, res);
    debug(std::cout << "WorldRouter::deliverTo done {"
                    << op->getParent() << ":"
                    << op->getFrom() << ":" << op->getTo() << "}" << std::endl
                    << std::flush;)
    for (auto& resOp : res) {
        if (op->getFrom() == resOp->getTo()) {
            if (!op->isDefaultSerialno() && resOp->isDefaultRefno()) {
                resOp->setRefno(op->getSerialno());
            }
        }
        message(resOp, *ent);
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
    debug(std::cout << "WorldRouter::operation {"
                    << op->getParent() << ":"
                    << op->getFrom() << ":" << op->getTo() << "}"
                    << std::endl << std::flush;)
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

/// Main world loop function.
/// This function is called whenever the communications code is idle.
/// It updates the in-game time, and dispatches operations that are
/// now due for dispatch. The number of operations dispatched is limited
/// to 10 to ensure that client communications are always handled in a timely
/// manner. If the maximum number of operations are dispatched, the return 
/// value indicates that this is the case, and the communications code
/// will call this function again as soon as possible rather than sleeping.
/// This ensures that the maximum possible number of operations are dispatched
/// without becoming unresponsive to client communications traffic.
bool WorldRouter::idle()
{
    return m_operationsDispatcher.idle(10);
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

LocatedEntity& WorldRouter::getDefaultLocation() const
{
    if (m_defaultLocation) {
        return *m_defaultLocation;
    }
    return *m_baseEntity;
}

OperationsDispatcher<LocatedEntity>& WorldRouter::getOperationsHandler()
{
    return m_operationsDispatcher;
}
