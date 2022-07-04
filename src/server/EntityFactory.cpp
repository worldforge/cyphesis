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

#include "rules/simulation/World.h"

#include "common/ScriptKit.h"
#include "common/TypeNode.h"
#include "EntityFactory.h"


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

EntityFactoryBase::~EntityFactoryBase() = default;


template<>
Ref<Entity> EntityFactory<World>::newEntity(RouterId id,
                                                   const Atlas::Objects::Entity::RootEntity& attributes)
{
    return nullptr;
}

void EntityFactoryBase::initializeEntity(Entity& thing,
                                         const Atlas::Objects::Entity::RootEntity& attributes)
{
    thing.setType(m_type);

    //Only apply attributes if the supplied attributes is valid.
    //The main use of this is when doing restoration from stored entities and we don't want to apply the default attributes directly when
    //the entity first is created.
    if (attributes.isValid()) {

        auto attrs = attributes->asMessage();
        //First make sure that all properties are installed, since Entity::setAttr won't install props if they exist in the type.
        for (auto& propIter : m_type->defaults()) {
            auto& prop = propIter.second;
            prop->install(thing, propIter.first);
        }

        // Apply the attribute values
        thing.merge(attrs);
        // Then set up the default class properties
        for (auto& propIter : m_type->defaults()) {
            // The property will have been applied if it has an overridden
            // value, so we only apply if the value is still default.
            if (thing.getProperties().find(propIter.first) == thing.getProperties().end()) {
                auto& prop = propIter.second;
                prop->apply(thing);
                thing.propertyApplied(propIter.first, *prop);
            }
        }
    }

}

void EntityFactoryBase::addProperties(const PropertyManager& propertyManager)
{
    assert(m_type != nullptr);
    m_type->addProperties(m_attributes, propertyManager);
}

void EntityFactoryBase::updateProperties(std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes, const PropertyManager& propertyManager)
{
    assert(m_type != nullptr);
    changes.emplace(m_type, m_type->updateProperties(m_attributes, propertyManager));

    for (auto& child_factory : m_children) {
        child_factory->m_attributes = m_attributes;
        for (const auto& entry : child_factory->m_classAttributes) {
            auto existingI = child_factory->m_attributes.find(entry.first);
            if (existingI != child_factory->m_attributes.end()) {
                entry.second.combine(existingI->second);
            } else {
                Atlas::Message::Element value;
                entry.second.combine(value);
                child_factory->m_attributes.emplace(entry.first, std::move(value));
            }
        }
        child_factory->updateProperties(changes, propertyManager);
    }
}

template
class EntityFactory<Thing>;

template
class EntityFactory<World>;

void ClassAttribute::combine(Atlas::Message::Element& existing) const
{
    if (!defaultValue.isNone()) {
        existing = defaultValue;
    }
    if (!add_fraction.isNone()) {
        AddFractionModifier modifier(add_fraction);
        modifier.process(existing, existing);
    }
    if (!subtract.isNone()) {
        SubtractModifier modifier(subtract);
        modifier.process(existing, existing); //Won't touch baseValue, so ok if we send "existing".
    }
    if (!prepend.isNone()) {
        PrependModifier modifier(prepend);
        modifier.process(existing, existing); //Won't touch baseValue, so ok if we send "existing".
    }
    if (!append.isNone()) {
        AppendModifier modifier(append);
        modifier.process(existing, existing); //Won't touch baseValue, so ok if we send "existing".
    }
}
