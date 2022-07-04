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

#include "EntityFactory.h"
#include "ArchetypeFactory.h"

#include "rules/simulation/Entity.h"

#include "common/debug.h"
#include "common/Inheritance.h"
#include "common/Monitors.h"
#include "common/Variable.h"

#include <Atlas/Objects/RootOperation.h>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

static const bool debug_flag = false;

EntityBuilder::EntityBuilder()
{
    installBaseFactory("archetype", "root_entity", std::make_unique<ArchetypeFactory>(*this));
}

EntityBuilder::~EntityBuilder() = default;

/// \brief Build and populate a new entity object.
///
/// A factory is found for the type of entity, and invoked to create the object
/// instance.
/// The attribute values are then set on the instance, taking into account
/// the defaults for the class, and those inherited from parent classes, and
/// the values specified for this instance.
/// @param id The string identifier of the new entity.
/// @param intId The integer identifier of the new entity.
/// @param type The string specifying the type of entity.
/// @param attributes A mapping of attribute values to set on the entity.
Ref<Entity> EntityBuilder::newEntity(RouterId id, const std::string& type, const RootEntity& attributes) const
{
    try {
        return newChildEntity(id, type, attributes);
    } catch (const std::exception& ex) {
        log(ERROR, String::compose("Error when creating entity of type %1."
                                   " Message: %2", type, ex.what()));
        return nullptr;
    }
}

Ref<Entity> EntityBuilder::newChildEntity(RouterId id,
                                                 const std::string& type,
                                                 const Atlas::Objects::Entity::RootEntity& attributes) const
{
    debug_print("EntityFactor::newEntity()")
    auto I = m_entityFactories.find(type);
    if (I == m_entityFactories.end()) {
        return nullptr;
    }

    auto& factory = I->second;
    debug_print("[" << type << "]")
    if (attributes) {
        attributes->removeAttr("parent");
    }
    return factory->newEntity(id, attributes);

}

/// \brief Clear out all the factory objects owned by the entity builder.
void EntityBuilder::flushFactories()
{
    m_entityFactories.clear();
}

void EntityBuilder::installBaseFactory(const std::string& class_name, const std::string& parent, std::unique_ptr<EntityKit> factory)
{
    installFactory(class_name, atlasClass(class_name, parent), std::move(factory));
}

int EntityBuilder::installFactory(const std::string& class_name, const Root& class_desc, std::unique_ptr<EntityKit> factory)
{
    Inheritance& i = Inheritance::instance();
    factory->m_type = i.addChild(class_desc);

    if (factory->m_type == nullptr) {
        return -1;
    }

    Monitors::instance().watch(compose("created_count{type=\"%1\"}", class_name), std::make_unique<Variable<int>>(factory->m_createdCount));

    m_entityFactories.emplace(class_name, std::move(factory));

    return 0;
}

EntityKit* EntityBuilder::getClassFactory(const std::string& class_name) const
{
    auto I = m_entityFactories.find(class_name);
    if (I == m_entityFactories.end()) {
        return nullptr;
    }
    return I->second.get();
}

