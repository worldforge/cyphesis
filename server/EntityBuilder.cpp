// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
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


#include "EntityBuilder.h"

#include "CorePropertyManager.h"
#include "EntityFactory.h"

#include "rulesets/LocatedEntity.h"

#include "common/BaseWorld.h"
#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/Inheritance.h"
#include "common/random.h"
#include "common/compose.hpp"
#include "common/Monitors.h"
#include "common/ScriptKit.h"
#include "common/TaskKit.h"
#include "common/TypeNode.h"
#include "common/Variable.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/RootOperation.h>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

class Character;
class Creator;
class Entity;
class Plant;
class Stackable;
class Thing;
class World;

extern template class EntityFactory<Character>;
extern template class EntityFactory<Creator>;
extern template class EntityFactory<Thing>;
extern template class EntityFactory<Plant>;
extern template class EntityFactory<Stackable>;
extern template class EntityFactory<World>;

static const bool debug_flag = false;

EntityBuilder * EntityBuilder::m_instance = NULL;

EntityBuilder::EntityBuilder()
{
    installBaseFactory("world", "game_entity", new EntityFactory<World>());
    EntityFactory<Thing> * tft = new EntityFactory<Thing>();
    installBaseFactory("thing", "game_entity", tft);
    installBaseFactory("character", "thing", new EntityFactory<Character>());
    installBaseFactory("creator", "character", new EntityFactory<Creator>());
    installBaseFactory("plant", "thing", new EntityFactory<Plant>());
    installBaseFactory("stackable", "thing", new EntityFactory<Stackable>());

    // The property manager instance installs itself at construction time.
    new CorePropertyManager();
}

EntityBuilder::~EntityBuilder()
{
    FactoryDict::const_iterator I = m_entityFactories.begin();
    FactoryDict::const_iterator Iend = m_entityFactories.end();
    for (; I != Iend; ++I) {
        delete I->second;
    }

    TaskFactoryDict::const_iterator J = m_taskFactories.begin();
    TaskFactoryDict::const_iterator Jend = m_taskFactories.end();
    for (; J != Jend; ++J) {
        delete J->second;
    }

    delete PropertyManager::instance();
}

/// \brief Build and populate a new entity object.
///
/// A factory is found for the type of entity, and invoked to create the object
/// instance. If the type has a script factory, this is invoked to create the
/// associates script object which is attached to the entity instance.
/// The attribute values are then set on the instance, taking into account
/// the defaults for the class, and those inherited from parent classes, and
/// the values specified for this instance. The essential location data for
/// this instance is then set up. The final block of code relating to
/// persistence is legacy, and should never be invoked.
/// @param id The string identifier of the new entity.
/// @param intId The integer identifier of the new entity.
/// @param type The string specifying the type of entity.
/// @param attributes A mapping of attribute values to set on the entity.
LocatedEntity * EntityBuilder::newEntity(const std::string & id, long intId,
                                         const std::string & type,
                                         const RootEntity & attributes,
                                         const BaseWorld & world) const
{
    debug(std::cout << "EntityFactor::newEntity()" << std::endl << std::flush;);
    LocatedEntity * thing = 0;
    FactoryDict::const_iterator I = m_entityFactories.find(type);
    if (I == m_entityFactories.end()) {
        return 0;
    }
    EntityKit * factory = I->second;
    thing = factory->newEntity(id, intId);
    if (thing == 0) {
        return 0;
    }
    debug( std::cout << "[" << type << "]"
                     << std::endl << std::flush;);
    thing->setType(factory->m_type);
    // Sort out python object
    if (factory->m_scriptFactory != 0) {
        debug(std::cout << "Class " << type << " has a python class"
                        << std::endl << std::flush;);
        factory->m_scriptFactory->addScript(thing);
    }
    //

    // Get location from entity, if it is present
    // The default attributes cannot contain info on location
    if (attributes->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        const std::string & loc_id = attributes->getLoc();
        thing->m_location.m_loc = world.getEntity(loc_id);
    }
    if (thing->m_location.m_loc == 0) {
        // If no info was provided, put the entity in the game world
        thing->m_location.m_loc = &world.m_gameWorld;
    }
    thing->m_location.readFromEntity(attributes);
    if (!thing->m_location.pos().isValid()) {
        // If no position coords were provided, put it somewhere near origin
        thing->m_location.m_pos = Point3D(uniform(-8,8), uniform(-8,8), 0);
    }
    if (thing->m_location.velocity().isValid()) {
        if (attributes->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            log(ERROR, compose("EntityBuilder::newEntity(%1, %2): "
                               "Entity has velocity set from the attributes "
                               "given by the creator", id, type));
        } else {
            log(ERROR, compose("EntityBuilder::newEntity(%1, %2): Entity has "
                               "velocity set from an unknown source",
                               id, type));
        }
        thing->m_location.m_velocity.setValid(false);
    }

    MapType attrs = attributes->asMessage();
    // Apply the attribute values
    thing->merge(attrs);
    // Then set up the default class properties
    PropertyDict::const_iterator J = factory->m_type->defaults().begin();
    PropertyDict::const_iterator Jend = factory->m_type->defaults().end();
    for (; J != Jend; ++J) {
        PropertyBase * prop = J->second;
        // If a property is in the class it won't have been installed
        // as setAttr() checks
        prop->install(thing, J->first);
        // The property will have been applied if it has an overriden
        // value, so we only apply it the value is still default.
        if (attrs.find(J->first) == attrs.end()) {
            prop->apply(thing);
        }
    }
    return thing;
}

Task * EntityBuilder::buildTask(TaskKit * factory, LocatedEntity & owner) const
{
    Task * task = factory->newTask(owner);

    if (task != 0 && factory->m_scriptFactory != 0) {
        if (factory->m_scriptFactory->addScript(task) != 0) {
            log(ERROR, "Assigning script to task failed");
        }
    }
    return task;
}

/// \brief Build and populate a new task object.
///
/// @param name The name of the task type.
/// @param owner The character entity that owns the task.
Task * EntityBuilder::newTask(const std::string & name, LocatedEntity & owner) const
{
    TaskFactoryDict::const_iterator I = m_taskFactories.find(name);
    if (I == m_taskFactories.end()) {
        return 0;
    }
    return buildTask(I->second, owner);
}

void EntityBuilder::installTaskFactory(const std::string & class_name,
                                       TaskKit * factory)
{
    m_taskFactories.insert(std::make_pair(class_name, factory));
}

TaskKit * EntityBuilder::getTaskFactory(const std::string & class_name)
{
    TaskFactoryDict::const_iterator I = m_taskFactories.find(class_name);
    if (I == m_taskFactories.end()) {
        return 0;
    }
    return I->second;
}

void EntityBuilder::addTaskActivation(const std::string & tool,
                                      const std::string & op,
                                      TaskKit * factory)
{
    m_taskActivations[tool].insert(std::make_pair(op, factory));
}

/// \brief Build a new task object activated by the described event.
///
/// An event is described in terms of the tool type used to cause it,
/// the type of operation being performed using the tool and the type of
/// the target object the tool is being used on. If a match is found for
/// this event, a task object is instanced to track the progress of the
/// result of the event.
/// @param tool The type of tool activating the event.
/// @param op The type of operation being performed with the tool.
/// @param target The type of entity the operation is being performed on.
/// @param owner The character entity activating the task.
Task * EntityBuilder::activateTask(const std::string & tool,
                                   const std::string & op,
                                   LocatedEntity * target,
                                   LocatedEntity & owner) const
{
    TaskFactoryActivationDict::const_iterator I = m_taskActivations.find(tool);
    if (I == m_taskActivations.end()) {
        return 0;
    }
    const TaskFactoryMultimap & dict = I->second;
    TaskFactoryMultimap::const_iterator J = dict.lower_bound(op);
    if (J == dict.end()) {
        return 0;
    }
    TaskFactoryMultimap::const_iterator Jend = dict.upper_bound(op);
    for (; J != Jend; ++J) {
        if (J->second->checkTarget(target) == -1) {
            continue;
        }
        return buildTask(J->second, owner);
    }
    return 0;
}

/// \brief Clear out all the factory objects owned by the entity builder.
void EntityBuilder::flushFactories()
{
    FactoryDict::const_iterator Iend = m_entityFactories.end();
    for (FactoryDict::const_iterator I = m_entityFactories.begin(); I != Iend; ++I) {
        delete I->second;
    }
    m_entityFactories.clear();
    TaskFactoryDict::const_iterator K = m_taskFactories.begin();
    TaskFactoryDict::const_iterator Kend = m_taskFactories.end();
    for (; K != Kend; ++K) {
        delete K->second;
    }
    m_taskFactories.clear();
}

bool EntityBuilder::isTask(const std::string & class_name)
{
    if (class_name == "task") {
        return true;
    }
    return (m_taskFactories.find(class_name) != m_taskFactories.end());
}

bool EntityBuilder::hasTask(const std::string & class_name)
{
    return (m_taskFactories.find(class_name) != m_taskFactories.end());
}

void EntityBuilder::installBaseFactory(const std::string & class_name,
                                       const std::string & parent,
                                       EntityKit * factory)
{
    installFactory(class_name, atlasClass(class_name, parent), factory);
}

int EntityBuilder::installFactory(const std::string & class_name,
                                  const Root & class_desc,
                                  EntityKit * factory)
{
    Inheritance & i = Inheritance::instance();
    factory->m_type = i.addChild(class_desc);

    if (factory->m_type == 0) {
        return -1;
    }

    m_entityFactories.insert(std::make_pair(class_name, factory));

    Monitors::instance()->watch(compose("created_count{type=%1}", class_name),
                                new Variable<int>(factory->m_createdCount));
    return 0;
}

EntityKit * EntityBuilder::getClassFactory(const std::string & class_name)
{
    FactoryDict::const_iterator I = m_entityFactories.find(class_name);
    if (I == m_entityFactories.end()) {
        return 0;
    }
    return I->second;
}
