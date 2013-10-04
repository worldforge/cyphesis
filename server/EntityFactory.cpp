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

#include "rulesets/Character.h"
#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Stackable.h"
#include "rulesets/World.h"

#include "rulesets/LocatedEntity.h"
#include "common/debug.h"
#include "common/log.h"
#include "common/ScriptKit.h"
#include "common/TypeNode.h"
#include "common/random.h"

#include <Atlas/Objects/Entity.h>

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

static const bool debug_flag = false;

template <>
LocatedEntity * EntityFactory<World>::newEntity(const std::string & id,
                                                long intId,
                                                const Atlas::Objects::Entity::RootEntity & attributes,
                                                LocatedEntity* location)
{
    return 0;
}

template <class T>
void EntityFactory<T>::initializeEntity(EntityKit& kit, LocatedEntity& thing,
        const Atlas::Objects::Entity::RootEntity & attributes, LocatedEntity* location)
{
    thing.setType(kit.m_type);
    // Sort out python object
    if (kit.m_scriptFactory != 0) {
        debug(std::cout << "Class " << kit.m_type->name() << " has a python class"
                        << std::endl << std::flush;);
        kit.m_scriptFactory->addScript(&thing);
    }
    thing.m_location.m_loc = location;

    thing.m_location.readFromEntity(attributes);
    if (!thing.m_location.pos().isValid()) {
        // If no position coords were provided, put it somewhere near origin
        thing.m_location.m_pos = Point3D(uniform(-8,8), uniform(-8,8), 0);
    }
    if (thing.m_location.velocity().isValid()) {
        if (attributes->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            log(ERROR, compose("EntityBuilder::newEntity(%1, %2): "
                               "Entity has velocity set from the attributes "
                               "given by the creator", thing.getId(), kit.m_type->name()));
        } else {
            log(ERROR, compose("EntityBuilder::newEntity(%1, %2): Entity has "
                               "velocity set from an unknown source",
                               thing.getId(), kit.m_type->name()));
        }
        thing.m_location.m_velocity.setValid(false);
    }

    MapType attrs = attributes->asMessage();
    // Apply the attribute values
    thing.merge(attrs);
    // Then set up the default class properties
    for (auto propIter : kit.m_type->defaults()) {
        PropertyBase * prop = propIter.second;
        // If a property is in the class it won't have been installed
        // as setAttr() checks
        prop->install(&thing, propIter.first);
        // The property will have been applied if it has an overriden
        // value, so we only apply it the value is still default.
        if (attrs.find(propIter.first) == attrs.end()) {
            prop->apply(&thing);
        }
    }
}


template class EntityFactory<Thing>;
template class EntityFactory<Character>;
template class EntityFactory<Creator>;
template class EntityFactory<Plant>;
template class EntityFactory<Stackable>;
template class EntityFactory<World>;
