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

#include "rulesets/World.h"
#include "rulesets/Domain.h"
#include "rulesets/TransformsProperty.h"

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/const.h"
#include "common/globals.h"
#include "common/random.h"
#include "common/system.h"
#include "common/TypeNode.h"
#include "common/serialno.h"
#include "common/compose.hpp"
#include "common/Inheritance.h"
#include "common/Monitors.h"
#include "common/SystemTime.h"
#include "common/Variable.h"
#include "common/Tick.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sstream>
#include <algorithm>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

/**
 * \brief Acts as a RAII scoped guard for an entity.
 */
struct EntityScopedRef {
    LocatedEntity& entity;
    explicit EntityScopedRef(LocatedEntity& e);
    ~EntityScopedRef();
};

inline EntityScopedRef::EntityScopedRef(LocatedEntity & e) : entity(e)
{
    entity.incRef();
}

inline EntityScopedRef::~EntityScopedRef()
{
    entity.decRef();
}



/// \brief Constructor for the world object.
///
/// The Entity representing the world is implicitly constructed.
/// Currently the world entity is included in the perceptives list,
/// but I am not clear why. Need to look into why.
WorldRouter::WorldRouter(const SystemTime & time) :
      BaseWorld(*new World(consts::rootWorldId, consts::rootWorldIntId)),
      m_operationsDispatcher([&](const Operation & op, LocatedEntity & from){this->operation(op, from);}, [&]()->double {return getTime();}),
      m_entityCount(1)
          
{
    m_initTime = time.seconds();
    m_gameWorld.incRef();
    EntityBuilder::init();
    m_gameWorld.setType(Inheritance::instance().getType("world"));
    m_eobjects[m_gameWorld.getIntId()] = &m_gameWorld;
    m_perceptives.insert(&m_gameWorld);
    //WorldTime tmp_date("612-1-1 08:57:00");
    Monitors::instance()->watch("entities", new Variable<int>(m_entityCount));
}

/// \brief Destructor for the world object.
///
/// Destruction of the world object implicity deletes all IG objects in
/// the server, clears the operation queue
WorldRouter::~WorldRouter()
{
    { 
        debug(std::cout << "Flushing world with " << m_eobjects.size()
                        << " entities" << std::endl << std::flush;);
    }

    //Make sure to clear the queues first so that there's nothing referencing entities
    //in them.
    m_operationsDispatcher.clearQueues();
    m_suspendedQueue = OpQueue();

    EntityDict::const_iterator Jend = m_eobjects.end();
    for (EntityDict::const_iterator J = m_eobjects.begin(); J != Jend; ++J) {
        J->second->decRef();
    }
    for (auto entry : m_spawns) {
        delete entry.second.first;
    }
    m_spawns.clear();
    // This should be deleted here rather than in the base class because
    // we created it, and BaseWorld should not even know what it is.
    m_gameWorld.decRef();
}

bool WorldRouter::isQueueDirty() const
{
    return m_operationsDispatcher.isQueueDirty();
}

void WorldRouter::markQueueAsClean()
{
    m_operationsDispatcher.markQueueAsClean();
}



/// \brief Add a new entity to the world.
///
/// Adds a new entity to the lists maintained by the WorldRouter.
/// Verify that the entity has a valid location, setting to
/// the default spawn area if necessary. Handle inserting the
/// entity into the loc/contains tree maintained by the Entity
/// class. Send a Setup op to the entity.
LocatedEntity * WorldRouter::addEntity(LocatedEntity * ent)
{
    debug(std::cout << "WorldRouter::addEntity(" << ent->getIntId() << ")" << std::endl
                    << std::flush;);
    assert(ent->getIntId() != 0);
    m_eobjects[ent->getIntId()] = ent;
    ++m_entityCount;
    assert(ent->m_location.isValid());

    TransformsProperty* transProp = ent->requirePropertyClassFixed<TransformsProperty>();
    if (!ent->m_location.isValid()) {
        log(ERROR, String::compose("Entity %1 of type %2 added to world with invalid location!", ent->getId(), ent->getType()->name()));
        debug(std::cout << "set loc " << &getDefaultLocation()  << std::endl
                        << std::flush;);
        ent->m_location.m_loc = &getDefaultLocation();
        transProp->getTranslate() = Vector3D(uniform(-8,8), uniform(-8,8), 0);
        transProp->apply(ent);
        debug(std::cout << "loc set with loc " << ent->m_location.m_loc->getId()
                        << std::endl << std::flush;);
    }
    ent->m_location.update(getTime());
    // FIXME
    std::string mode;
    Element mode_attr;
    if (ent->getAttrType("mode", mode_attr, Element::TYPE_STRING) == 0) {
        mode = mode_attr.String();
    }
    if (ent->m_location.m_loc) {
        Domain* movementDomain = ent->m_location.m_loc->getMovementDomain();
        if (movementDomain) {
            float height = movementDomain->
                  constrainHeight(*ent, ent->m_location.m_loc,
                                  ent->m_location.pos(),
                                  mode);
            transProp->getTranslate().z() = height;
            transProp->apply(ent);
            movementDomain->addEntity(*ent);
        }
    }
    ent->m_location.m_loc->makeContainer();
    bool cont_change = ent->m_location.m_loc->m_contains->empty();
    bool child_inserted = ent->m_location.m_loc->m_contains->insert(ent).second;
    //check that the child wasn't already present
    if (child_inserted) {
        ent->m_location.m_loc->incRef();
    }
    // FIXME Should we call this every time a new child is inserted (now it's just called if the container is empty first
    if (cont_change) {
        // FIXME Mark the entity as dirty?
        ent->onUpdated();
    }
    debug(std::cout << "Entity loc " << ent->m_location << std::endl
                    << std::flush;);

    if (ent->m_contains != nullptr) {
        for (auto& child : *ent->m_contains) {
            addEntity(child);
        }
    }

    Anonymous arg;
    Appearance app;
    arg->setId(ent->getId());
    arg->setStamp(ent->getSeq());
    app->setArgs1(arg);
    message(app, *ent);

    inserted.emit(ent);

    return ent;
}

/// \brief Create a new entity and add to the world.
///
/// Construct a new entity using the entity description provided,
/// and pass it to addEntity().
/// @return a pointer to the new entity.
LocatedEntity * WorldRouter::addNewEntity(const std::string & typestr,
                                          const RootEntity & attrs)
{
    debug(std::cout << "WorldRouter::addNewEntity(\"" << typestr << "\", attrs)"
                    << std::endl << std::flush;);
    std::string id;
    long intId = newId(id);

    if (intId < 0) {
        log(ERROR, "Unable to get ID for new Entity");
        return 0;
    }

    LocatedEntity * ent = EntityBuilder::instance()->newEntity(id, intId, typestr, attrs, *this);
    if (ent == 0) {
        log(ERROR, String::compose("Attempt to create an entity of type \"%1\" "
                                   "but type is unknown or forbidden",
                                   typestr));
        return 0;
    }
    return addEntity(ent);
}

int WorldRouter::createSpawnPoint(const MapType & data, LocatedEntity * ent)
{
    MapType::const_iterator I = data.find("name");
    if (I == data.end() || !I->second.isString()) {
        log(ERROR, "No name on spawn point");
        return -1;
    }
    SpawnEntity * new_spawn = new SpawnEntity(ent);
    if (new_spawn->setup(data) != 0) {
        delete new_spawn;
        log(ERROR, "Error setting up spawn point");
        return -1;
    }

    const std::string & name = I->second.String();
    SpawnDict::iterator J = m_spawns.find(name);
    if (J != m_spawns.end()) {
        Spawn * old = J->second.first;
        J->second.first = new_spawn;
        J->second.second = ent->getId();
        delete old;
    } else {
        m_spawns.insert(std::make_pair(name, std::make_pair(new_spawn, ent->getId())));
    }
    return 0;
}

int WorldRouter::removeSpawnPoint(LocatedEntity * ent)
{
    for (auto I = m_spawns.begin(); I != m_spawns.end(); ++I) {
        if (I->second.second == ent->getId()) {
            delete I->second.first;
            m_spawns.erase(I);
            return 0;
        }
    }
    return 1;
}


int WorldRouter::getSpawnList(Atlas::Message::ListType & data)
{
    for (auto entry : m_spawns) {
        MapType spawn;
        spawn.insert(std::make_pair("name", entry.first));
        entry.second.first->addToMessage(spawn);
        data.push_back(spawn);
    }
    return 0;
}

LocatedEntity * WorldRouter::spawnNewEntity(const std::string & name,
                                            const std::string & type,
                                            const RootEntity & desc)
{
    SpawnDict::const_iterator I = m_spawns.find(name);
    if (I == m_spawns.end()) {
        log(ERROR, String::compose("Spawn not found %1", name));
        return 0;
    }
    Spawn * s = I->second.first;
    int ret = s->spawnEntity(type, desc);
    if (ret != 0) {
        log(ERROR, String::compose("Spawn not permitting %1", type));
        return 0;
    }
    LocatedEntity * e = addNewEntity(type, desc);
    if (e == 0) {
        log(ERROR, String::compose("Entity creation failed %1", type));
        return e;
    }

    return e;
}

int WorldRouter::moveToSpawn(const std::string & name, Location& location)
{
    auto I = m_spawns.find(name);
    if (I == m_spawns.end()) {
        log(ERROR, String::compose("Spawn not found %1", name));
        return -10;
    }
    return I->second.first->placeInSpawn(location);
}


/// \brief Create a new task
///
/// Construct a new task linked to the LocatedEntity provided.
/// @param name the name of the task type to be instantiated
/// @param owner the character who will own the task
/// @return a pointer to the new task
Task * WorldRouter::newTask(const std::string & name, LocatedEntity & owner)
{
    Task * task = EntityBuilder::instance()->newTask(name, owner);
    if (task == 0) {
        log(ERROR, String::compose("Attempt to create a task of type \"%1\" "
                                   "but type is unknown or forbidden", name));
    }
    return task;
}

/// \brief Activate a new task
///
/// Construct a task linked to the LocatedEntity provided, activated by the
/// tool and operation class given.
/// @param tool the type of tool activating the task
/// @param op the type of operation acitivating the task
/// @param target the entity that the task uses as its target
/// @param owner the character who will own the task
/// @return a pointer to the new task
Task * WorldRouter::activateTask(const std::string & tool,
                                 const std::string & op,
                                 LocatedEntity * target,
                                 LocatedEntity & owner)
{
    return EntityBuilder::instance()->activateTask(tool, op, target, owner);
}

ArithmeticScript * WorldRouter::newArithmetic(const std::string & name,
                                              LocatedEntity * owner)
{
    return ArithmeticBuilder::instance()->newArithmetic(name, owner);
}


/// \brief Remove an entity from the world.
///
/// Remove an entity from the various lists in which it is stored.
/// The entity is removed from the LOC/CONTAINS tree, and the
/// reference held by the world is decremented. There may still be
/// a reference held by an operation in the queue from the removed
/// entity.
void WorldRouter::delEntity(LocatedEntity * ent)
{
    if (ent == &m_gameWorld) {
        log(WARNING, "Attempt to delete game world");
        return;
    }
    assert(ent->getIntId() != 0);
    m_perceptives.erase(ent);
    m_eobjects.erase(ent->getIntId());
    --m_entityCount;
    ent->destroy();
    ent->updated.emit();
    ent->decRef();
}

void WorldRouter::resumeWorld()
{
    //Take all suspended operations and add them to be executed.
    while (!m_suspendedQueue.empty()) {
        auto& ope = m_suspendedQueue.front();
        m_operationsDispatcher.addOperationToQueue(ope.op, *ope.from);
        m_suspendedQueue.pop();
    }
}


/// \brief Pass an operation to the World.
///
/// Pass an operation to addOperationToQueue()
/// so it gets added to the queue for dispatch.
void WorldRouter::message(const Operation & op, LocatedEntity & ent)
{
    m_operationsDispatcher.addOperationToQueue(op, ent);
    debug(std::cout << "WorldRouter::message {"
                    << op->getParents().front() << ":"
                    << op->getFrom() << ":" << op->getTo() << "}" << std::endl
                    << std::flush;);
}

/// \brief Determine the broadcast list to be used to broadcast an operation.
///
/// Check the type of operation, and work out which list of entities
/// it should be broadcast to. This will be perceptives in case 
/// a perception operation, or all entities in any other case.
/// This should probably go, as there is essentially no sane reason
/// for broadcasting a random op to all entities.
/// @return a reference to the list of entities to be used for braodcast.
bool WorldRouter::broadcastPerception(const Operation & op) const
{
    int op_class = op->getClassNo();
    if (op_class == Atlas::Objects::Operation::SIGHT_NO ||
        op_class == Atlas::Objects::Operation::SOUND_NO ||
        op_class == Atlas::Objects::Operation::APPEARANCE_NO ||
        op_class == Atlas::Objects::Operation::DISAPPEARANCE_NO) {
        return true;
    }
    log(WARNING, String::compose("Broadcasting %1 op from %2",
                                 op->getParents().front(),
                                 op->getFrom()));
    return false;
}

/// \brief Deliver an operation to its target.
///
/// Pass the operation to the target entity. The resulting operations
/// have their ref numbers set, and are added to the queue for
/// dispatch.
void WorldRouter::deliverTo(const Operation & op, LocatedEntity & ent)
{
    //Make sure the entity isn't dereferenced while in this loop.
    //This is mainly in place to handle relayed deletion ops.
    EntityScopedRef referenceGuard(ent);
    //If the world is suspended and the op is a tick, we should store it
    //(to be resent when the world is resumed) and not process it now.
    if (m_isSuspended) {
        if (op->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
            m_suspendedQueue.push(OpQueEntry(op, ent));
            return;
        }
    }
    OpVector res;
    debug(std::cout << "WorldRouter::deliverTo begin {"
                        << op->getParents().front() << ":"
                        << op->getFrom() << ":" << op->getTo() << "}" << std::endl
                        << std::flush;);
    ent.operation(op, res);
    debug(std::cout << "WorldRouter::deliverTo done {"
                        << op->getParents().front() << ":"
                        << op->getFrom() << ":" << op->getTo() << "}" << std::endl
                        << std::flush;);
    for(auto& resOp : res) {
        if (op->getFrom() == resOp->getTo()) {
            if (!op->isDefaultSerialno() && resOp->isDefaultRefno()) {
                resOp->setRefno(op->getSerialno());
            }
        }
        message(resOp, ent);
    }
}

/// \brief Main in-game operation dispatch function.
///
/// Operations are passed here when they are due for dispatch.
/// Determine the target of the operation and deliver it directly,
/// or broadcast if broadcast is required. This function implements
/// sight ranges for perception operations.
/// @param op operation to be dispatched to the world. This is non-const
/// so that broadcast ops can have their TO set correctly for each target.
/// @param from entity the operation to be dispatched was send from. Note
/// that it is possible that this entity has been destroyed, but it
/// should still have a valid location, so can be used for range
/// calculations.
void WorldRouter::operation(const Operation & op, LocatedEntity & from)
{
    debug(std::cout << "WorldRouter::operation {"
                    << op->getParents().front() << ":"
                    << op->getFrom() << ":" << op->getTo() << "}"
                    << std::endl << std::flush;);
    assert(op->getFrom() == from.getId());
    assert(!op->getParents().empty());

    Dispatching.emit(op);

    if (!op->isDefaultTo()) {
        const std::string & to = op->getTo();
        assert(!to.empty());
        LocatedEntity * to_entity = 0;

        if (to == from.getId()) {
            if (from.isDestroyed()) {
                // Entity no longer exists
                return;
            }
            to_entity = &from;
        } else {
            to_entity = getEntity(to);

            if (to_entity == 0) {
                debug(std::cerr << "WARNING: Op to=\"" << to << "\""
                                << " does not exist"
                                << std::endl << std::flush;);
                return;
            }
        }

        assert(to_entity != 0);

        deliverTo(op, *to_entity);

    } else if (broadcastPerception(op)) {
        if (from.m_location.m_loc) {
            // Where broadcasts go depends on type of op
            for (auto& entity : m_perceptives) {
                if (entity->m_location.m_loc) {
                    Domain* entityDomain = entity->m_location.m_loc->getMovementDomain();
                    if (entityDomain && entityDomain->isEntityVisibleFor(*entity, from)) {
                        op->setTo(entity->getId());
                        deliverTo(op, *entity);
                    }

                }
            }
        }
    } else {
        EntityDict::const_iterator I = m_eobjects.begin();
        EntityDict::const_iterator Iend = m_eobjects.end();
        for (; I != Iend; ++I) {
            op->setTo(I->second->getId());
            deliverTo(op, *I->second);
        }
    }
}

/// Add entity provided to the list of perceptive entities.
/// Look up the entity with the id provided, and add a pointer
/// to the entity to the set of perceptive entities. This method is
/// called when key events occur that indicate that the entity in
/// question can receive broadcast perception operations.
void WorldRouter::addPerceptive(LocatedEntity * perceptive)
{
    debug(std::cout << "WorldRouter::addPerceptive" << std::endl << std::flush;);
    m_perceptives.insert(perceptive);
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
    return m_operationsDispatcher.idle();
}


double WorldRouter::secondsUntilNextOp() const {
    return m_operationsDispatcher.secondsUntilNextOp();
}

/// Find an entity of the given name. This is provided to allow administrators
/// to perform certain admin tasks. It finds and returns the first instance
/// with the name provided in the game world.
/// @param name string specifying name of the instance required.
/// @return a pointer to an entity with the type required, or zero if an
/// instance with this name was not found.
LocatedEntity * WorldRouter::findByName(const std::string & name)
{
    Element name_attr;
    EntityDict::const_iterator Iend = m_eobjects.end();
    for (EntityDict::const_iterator I = m_eobjects.begin(); I != Iend; ++I) {
        if (I->second->getAttr("name", name_attr) == 0) {
            if (name_attr == name) {
                return I->second;
            }
        }
    }
    return NULL;
}

/// Find an entity of the given type. This is provided to allow administrators
/// to perform certain admin tasks. It finds and returns the first instance
/// of the type provided in the game world.
/// @param type string specifying the class name of the instance required.
/// @return a pointer to an entity of the type required, or zero if no
/// instance was found.
LocatedEntity * WorldRouter::findByType(const std::string & type)
{
    EntityDict::const_iterator Iend = m_eobjects.end();
    for(EntityDict::const_iterator I = m_eobjects.begin(); I != Iend; ++I) {
        if (I->second->getType()->name() == type) {
            return I->second;
        }
    }
    return NULL;
}
