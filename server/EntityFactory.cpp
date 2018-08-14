// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#include "EntityFactory_impl.h"

#include "rulesets/Plant.h"
#include "rulesets/Stackable.h"
#include "rulesets/World.h"

#include "common/debug.h"
#include "common/ScriptKit.h"
#include "common/TypeNode.h"
#include "common/random.h"

#include <Atlas/Objects/Entity.h>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

static const bool debug_flag = false;

EntityFactoryBase::EntityFactoryBase()
: m_parent(nullptr)
{

}
EntityFactoryBase::~EntityFactoryBase()
{
}


template <>
Ref<LocatedEntity> EntityFactory<World>::newEntity(const std::string & id,
                                                long intId,
                                                const Atlas::Objects::Entity::RootEntity & attributes,
                                                LocatedEntity* location)
{
    return nullptr;
}

void EntityFactoryBase::initializeEntity(LocatedEntity& thing,
        const Atlas::Objects::Entity::RootEntity & attributes, LocatedEntity* location)
{
    thing.setType(m_type);
    thing.m_location.m_loc = location;

    //Only apply attributes if the supplied attributes is valid.
    //The main use of this is when doing restoration from stored entities and we don't want to apply the default attributes directly when
    //the entity first is created.
    if (attributes.isValid()) {
        thing.m_location.readFromEntity(attributes);

        if (!thing.m_location.pos().isValid()) {
            // If no position coords were provided, put it somewhere near origin
            thing.m_location.m_pos = Point3D(uniform(-8,8), uniform(-8,8), 0);
        }
        if (thing.m_location.velocity().isValid()) {
            if (attributes.isValid() && attributes->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
                log(ERROR, compose("EntityFactory::initializeEntity(%1, %2): "
                                   "Entity has velocity set from the attributes "
                                   "given by the creator", thing.getId(), m_type->name()));
            } else {
                log(ERROR, compose("EntityFactory::initializeEntity(%1, %2): Entity has "
                                   "velocity set from an unknown source",
                                   thing.getId(), m_type->name()));
            }
            thing.m_location.m_velocity.setValid(false);
        }

        MapType attrs = attributes->asMessage();
        // Apply the attribute values
        thing.merge(attrs);
        // Then set up the default class properties
        for (auto& propIter : m_type->defaults()) {
            PropertyBase * prop = propIter.second;
            // If a property is in the class it won't have been installed
            // as setAttr() checks
            prop->install(&thing, propIter.first);
            // The property will have been applied if it has an overridden
            // value, so we only apply it the value is still default.
            if (attrs.find(propIter.first) == attrs.end()) {
                prop->apply(&thing);
                thing.propertyApplied(propIter.first, *prop);
            }
        }
    }
}

void EntityFactoryBase::addProperties()
{
    assert(m_type != nullptr);
    m_type->addProperties(m_attributes);
}

void EntityFactoryBase::updateProperties(std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    assert(m_type != nullptr);
    changes.emplace(m_type, m_type->updateProperties(m_attributes));

    for (auto& child_factory : m_children) {
        child_factory->m_attributes = m_attributes;
        MapType::const_iterator J = child_factory->m_classAttributes.begin();
        MapType::const_iterator Jend = child_factory->m_classAttributes.end();
        for (; J != Jend; ++J) {
            child_factory->m_attributes[J->first] = J->second;
        }
        child_factory->updateProperties(changes);
    }
}

template class EntityFactory<Thing>;
template class EntityFactory<Plant>;
template class EntityFactory<Stackable>;
template class EntityFactory<World>;
