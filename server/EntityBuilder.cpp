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
#include "rulesets/Task.h"

#include "rulesets/BaseWorld.h"
#include "common/debug.h"
#include "common/Inheritance.h"
#include "common/Monitors.h"
#include "common/ScriptKit.h"
#include "common/TaskKit.h"
#include "common/TypeNode.h"
#include "common/Variable.h"

#include <Atlas/Objects/RootOperation.h>
#include <rulesets/Python_API.h>
#include <rulesets/ScriptsProperty.h>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

static const bool debug_flag = false;

template<>
EntityBuilder* Singleton<EntityBuilder>::ms_Singleton = nullptr;

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
            auto scriptsProp = entry.second->getPropertyClass<ScriptsProperty>("__scripts");
            if (scriptsProp) {
                scriptsProp->applyScripts(entry.second);
            }
            auto scriptsInstanceProp = entry.second->getPropertyClass<ScriptsProperty>("__scripts_instance");
            if (scriptsInstanceProp) {
                scriptsInstanceProp->applyScripts(entry.second);
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

/// \brief Clear out all the factory objects owned by the entity builder.
void EntityBuilder::flushFactories()
{
    FactoryDict::const_iterator Iend = m_entityFactories.end();
    for (FactoryDict::const_iterator I = m_entityFactories.begin(); I != Iend; ++I) {
        delete I->second;
    }
    m_entityFactories.clear();
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

    Monitors::instance().watch(compose("created_count{type=\"%1\"}", class_name), new Variable<int>(factory->m_createdCount));
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

