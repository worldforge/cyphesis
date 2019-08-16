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

#include "rules/simulation/Stackable.h"
#include "rules/simulation/World.h"

#include "common/debug.h"
#include "common/ScriptKit.h"
#include "common/TypeNode.h"
#include "common/random.h"
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
Ref<LocatedEntity> EntityFactory<World>::newEntity(const std::string& id,
                                                   long intId,
                                                   const Atlas::Objects::Entity::RootEntity& attributes,
                                                   LocatedEntity* location)
{
    return nullptr;
}

void EntityFactoryBase::initializeEntity(LocatedEntity& thing,
                                         const Atlas::Objects::Entity::RootEntity& attributes, LocatedEntity* location)
{
    thing.setType(m_type);
    thing.m_location.m_parent = location;

    //Only apply attributes if the supplied attributes is valid.
    //The main use of this is when doing restoration from stored entities and we don't want to apply the default attributes directly when
    //the entity first is created.
    if (attributes.isValid()) {
        thing.m_location.readFromEntity(attributes);

        if (!thing.m_location.pos().isValid()) {
            thing.m_location.m_pos = WFMath::Point<3>::ZERO();
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

        auto attrs = attributes->asMessage();
        //First make sure that all properties are installed, since Entity::setAttr won't install props if they exist in the type.
        for (auto& propIter : m_type->defaults()) {
            PropertyBase* prop = propIter.second;
            prop->install(&thing, propIter.first);
        }

        // Apply the attribute values
        thing.merge(attrs);
        // Then set up the default class properties
        for (auto& propIter : m_type->defaults()) {
            // The property will have been applied if it has an overridden
            // value, so we only apply if the value is still default.
            if (attrs.find(propIter.first) == attrs.end()) {
                auto prop = propIter.second;
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
        child_factory->updateProperties(changes);
    }
}

template
class EntityFactory<Thing>;

template
class EntityFactory<Stackable>;

template
class EntityFactory<World>;

void ClassAttribute::combine(Atlas::Message::Element& existing) const
{
    if (!defaultValue.isNone()) {
        existing = defaultValue;
    }
    if (!subtract.isNone()) {
        switch (subtract.getType()) {
            case Atlas::Message::Element::TYPE_INT:
                if (existing.isNone() || existing.getType() != subtract.getType()) {
                    existing = 0 - subtract.Int();
                } else {
                    existing = existing.Int() - subtract.Int();
                }
                break;
            case Atlas::Message::Element::TYPE_FLOAT:
                if (existing.isNone() || existing.getType() != subtract.getType()) {
                    existing = 0 - subtract.Float();
                } else {
                    existing = existing.Float() - subtract.Float();
                }
                break;
            case Atlas::Message::Element::TYPE_STRING:
                //There's no one obvious way of subtracting one string from another, so we'll just skip this
                break;
            case Atlas::Message::Element::TYPE_MAP:
                if (existing.isNone() || existing.getType() != subtract.getType()) {
                    existing = Atlas::Message::MapType();
                } else {
                    //Only act on the map keys
                    for (const auto& entry: subtract.Map()) {
                        existing.Map().erase(entry.first);
                    }
                }
                break;
            case Atlas::Message::Element::TYPE_LIST: {
                if (existing.isNone() || existing.getType() != subtract.getType()) {
                    existing = Atlas::Message::ListType();
                } else {
                    for (auto& entry: subtract.List()) {
                        Atlas::Message::ListType::iterator I;
                        //Delete all instances from the list.
                        while (true) {
                            I = std::find(std::begin(existing.List()), std::end(existing.List()), entry);
                            if (I != existing.List().end()) {
                                existing.List().erase(I);
                            } else {
                                break;
                            }
                        }
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    if (!prepend.isNone()) {
        if (existing.isNone() || existing.getType() != prepend.getType()) {
            existing = prepend;
        } else {
            switch (prepend.getType()) {
                case Atlas::Message::Element::TYPE_INT:
                    existing = prepend.Int() + existing.Int();
                    break;
                case Atlas::Message::Element::TYPE_FLOAT:
                    existing = prepend.Float() + existing.Float();
                    break;
                case Atlas::Message::Element::TYPE_STRING:
                    existing = prepend.String() + existing.String();
                    break;
                case Atlas::Message::Element::TYPE_MAP:
                    //Overwrite existing entries; with C++17 we can use "insert_or_assign".
                    for (const auto& entry: prepend.Map()) {
                        existing.Map()[entry.first] = entry.second;
                    }
                    break;
                case Atlas::Message::Element::TYPE_LIST: {
                    auto listCopy = std::move(existing.List());
                    existing.List().clear();
                    existing.List().reserve(listCopy.size() + prepend.List().size());
                    for (auto& entry: prepend.List()) {
                        existing.List().push_back(entry);
                    }
                    for (auto&& entry: listCopy) {
                        existing.List().push_back(std::move(entry));
                    }
                }
                    break;
                default:
                    break;
            }
        }
    }
    if (!append.isNone()) {
        if (existing.isNone() || existing.getType() != append.getType()) {
            existing = append;
        } else {
            switch (append.getType()) {
                case Atlas::Message::Element::TYPE_INT:
                    existing = existing.Int() + append.Int();
                    break;
                case Atlas::Message::Element::TYPE_FLOAT:
                    existing = existing.Float() + append.Float();
                    break;
                case Atlas::Message::Element::TYPE_STRING:
                    existing = existing.String() + append.String();
                    break;
                case Atlas::Message::Element::TYPE_MAP:
                    //Overwrite existing entries; with C++17 we can use "insert_or_assign".
                    for (const auto& entry: append.Map()) {
                        existing.Map()[entry.first] = entry.second;
                    }
                    break;
                case Atlas::Message::Element::TYPE_LIST:
                    existing.List().reserve(existing.List().size() + append.List().size());
                    for (auto& entry: append.List()) {
                        existing.List().push_back(entry);
                    }
                    break;
                default:
                    break;
            }

        }
    }
}
