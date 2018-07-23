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
#include "ArchetypeFactory.h"

#include "rulesets/LocatedEntity.h"

#include "common/BaseWorld.h"
#include "common/debug.h"
#include "common/Inheritance.h"
#include "common/Monitors.h"
#include "common/ScriptKit.h"
#include "common/TaskKit.h"
#include "common/TypeNode.h"
#include "common/Variable.h"
#include "TaskFactory.h"

#include <Atlas/Objects/RootOperation.h>
#include <rulesets/Python_API.h>
#include <rulesets/ScriptsProperty.h>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

static const bool debug_flag = false;

EntityBuilder* EntityBuilder::m_instance = nullptr;

EntityBuilder::EntityBuilder()
: m_propertyManager(new CorePropertyManager())
{
    installBaseFactory("archetype", "root_entity", new ArchetypeFactory());
    python_reload_scripts.connect([&]() {

        ScriptsProperty::reloadAllScriptFactories();

        //TODO: Implement scripts on archetypes and reload them here
//        std::map<const TypeNode*, EntityFactoryBase*> collector;
//        for (auto& entry : m_entityFactories) {
//            auto entityFactory = dynamic_cast<EntityFactoryBase*>(entry.second);
//            if (entityFactory && entityFactory->m_scriptFactory) {
//                log(NOTICE, compose("Reloading scripts for %1", entityFactory->m_type->name()));
//                entityFactory->m_scriptFactory->refreshClass();
//                collector.emplace(entityFactory->m_type, entityFactory);
//            }
//        }

        auto& entities = BaseWorld::instance().getEntities();

        for (auto& entry : entities) {
            auto scriptsProp = entry.second->getPropertyClassFixed<ScriptsProperty>();
            if (scriptsProp) {
                scriptsProp->applyScripts(entry.second);
            }
        }

        for (auto& entry : m_taskFactories) {
            if (entry.second->m_scriptFactory) {
                entry.second->m_scriptFactory->refreshClass();
                //Note that we don't reload the tasks since they often are short lived. Should we perhaps?
            }
        }


    });
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
}

/// \brief Build and populate a new entity object.
///
/// A factory is found for the type of entity, and invoked to create the object
/// instance. If the type has a script factory, this is invoked to create the
/// associates script object which is attached to the entity instance.
/// The attribute values are then set on the instance, taking into account
/// the defaults for the class, and those inherited from parent classes, and
/// the values specified for this instance. The essential location data for
/// this instance is then set up.
/// @param id The string identifier of the new entity.
/// @param intId The integer identifier of the new entity.
/// @param type The string specifying the type of entity.
/// @param attributes A mapping of attribute values to set on the entity.
Ref<LocatedEntity> EntityBuilder::newEntity(const std::string& id, long intId, const std::string& type, const RootEntity& attributes, const BaseWorld& world) const
{
    LocatedEntity* loc = nullptr;
    // Get location from entity, if it is present
    // The default attributes cannot contain info on location
    if (attributes.isValid() && attributes->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        const std::string& loc_id = attributes->getLoc();
        loc = world.getEntity(loc_id);
    }
    if (loc == nullptr) {
        // If no info was provided, put the entity in the default location world
        loc = &world.getDefaultLocation();
    }

    try {
        return newChildEntity(id, intId, type, attributes, *loc);
    } catch (const std::exception& ex) {
        log(ERROR, String::compose("Error when creating entity of type %1."
                                       " Message: %2", type, ex.what()));
        return nullptr;
    }
}

Ref<LocatedEntity> EntityBuilder::newChildEntity(const std::string& id, long intId, const std::string& type, const Atlas::Objects::Entity::RootEntity& attributes,
                                             LocatedEntity& parentEntity) const
{
    debug(std::cout << "EntityFactor::newEntity()" << std::endl << std::flush;);
    auto I = m_entityFactories.find(type);
    if (I == m_entityFactories.end()) {
        return nullptr;
    }

    EntityKit* factory = I->second;
    debug(std::cout << "[" << type << "]" << std::endl << std::flush;);
    return factory->newEntity(id, intId, attributes, &parentEntity);

}

Ref<Task> EntityBuilder::buildTask(TaskKit* factory, LocatedEntity& owner) const
{
    auto task = factory->newTask(owner);

    if (task && factory->m_scriptFactory != nullptr) {
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
Ref<Task> EntityBuilder::newTask(const std::string& name, LocatedEntity& owner) const
{
    auto I = m_taskFactories.find(name);
    if (I == m_taskFactories.end()) {
        return nullptr;
    }
    return buildTask(I->second, owner);
}

void EntityBuilder::installTaskFactory(const std::string& class_name, TaskKit* factory)
{
    m_taskFactories.insert(std::make_pair(class_name, factory));
}

TaskKit* EntityBuilder::getTaskFactory(const std::string& class_name)
{
    TaskFactoryDict::const_iterator I = m_taskFactories.find(class_name);
    if (I == m_taskFactories.end()) {
        return nullptr;
    }
    return I->second;
}

void EntityBuilder::addTaskActivation(const std::string& tool, const std::string& op, TaskKit* factory)
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
Ref<Task> EntityBuilder::activateTask(const std::string& tool, const std::string& op, LocatedEntity* target, LocatedEntity& owner) const
{
    auto I = m_taskActivations.find(tool);
    if (I == m_taskActivations.end()) {
        return nullptr;
    }
    const TaskFactoryMultimap& dict = I->second;
    auto J = dict.lower_bound(op);
    if (J == dict.end()) {
        return nullptr;
    }
    auto Jend = dict.upper_bound(op);
    for (; J != Jend; ++J) {
        if (J->second->checkTarget(target) == -1) {
            continue;
        }
        return buildTask(J->second, owner);
    }
    return nullptr;
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

bool EntityBuilder::isTask(const std::string& class_name)
{
    if (class_name == "task") {
        return true;
    }
    return (m_taskFactories.find(class_name) != m_taskFactories.end());
}

bool EntityBuilder::hasTask(const std::string& class_name)
{
    return (m_taskFactories.find(class_name) != m_taskFactories.end());
}

void EntityBuilder::installBaseFactory(const std::string& class_name, const std::string& parent, EntityKit* factory)
{
    installFactory(class_name, atlasClass(class_name, parent), factory);
}

int EntityBuilder::installFactory(const std::string& class_name, const Root& class_desc, EntityKit* factory)
{
    Inheritance& i = Inheritance::instance();
    factory->m_type = i.addChild(class_desc);

    if (factory->m_type == nullptr) {
        return -1;
    }

    m_entityFactories.insert(std::make_pair(class_name, factory));

    Monitors::instance()->watch(compose("created_count{type=\"%1\"}", class_name), new Variable<int>(factory->m_createdCount));
    return 0;
}

EntityKit* EntityBuilder::getClassFactory(const std::string& class_name)
{
    FactoryDict::const_iterator I = m_entityFactories.find(class_name);
    if (I == m_entityFactories.end()) {
        return nullptr;
    }
    return I->second;
}

