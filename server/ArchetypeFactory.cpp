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


#include "ArchetypeFactory.h"
#include "EntityBuilder.h"
#include "WorldRouter.h"

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
#include "common/id.h"

#include <Atlas/Objects/Entity.h>

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

static const bool debug_flag = false;

ArchetypeFactory::ArchetypeFactory()
{
}

ArchetypeFactory::ArchetypeFactory(ArchetypeFactory & o)
{
}

ArchetypeFactory::~ArchetypeFactory()
{
}


LocatedEntity * ArchetypeFactory::newEntity(const std::string & id,
                                                long intId,
                                                const Atlas::Objects::Entity::RootEntity & attributes,
                                                LocatedEntity* location)
{
    std::string concreteType = m_classAttributes.find("type")->second.String();

    LocatedEntity* entity = EntityBuilder::instance()->newEntity(id, intId,
            concreteType, attributes, WorldRouter::instance());

    auto containsI = m_classAttributes.find("contains");
    if (containsI != m_classAttributes.end()) {
        if (!containsI->second.isList()) {
            log(WARNING, "'contains' attribute is not a list");
        } else {
            auto& contains = containsI->second.asList();
            for (auto& childElem : contains) {
                if (!childElem.isMap()) {
                    log(WARNING, "child definition is not map");
                } else {
                    auto& childMap = childElem.asMap();

                    auto childTypeI = childMap.find("type");
                    if (childTypeI == childMap.end() || !childTypeI->second.isString()) {
                        log(WARNING, "No type defined for child.");
                        continue;
                    }
                    const std::string& childType = childTypeI->second.asString();

                    std::string childId;
                    long childIntId = newId(childId);

                    RootEntity childAttrEntity;
                    auto childAttrI = childMap.find("attributes");
                    if (childAttrI != childMap.end() && childAttrI->second.isMap()) {
                        childAttrEntity->setAttr(childAttrI->first, childAttrI->second);
                    }

                    LocatedEntity* childEntity = EntityBuilder::instance()->newChildEntity(childId, childIntId,
                            childType, childAttrEntity, *entity);

                    entity->makeContainer();
                    entity->m_contains->insert(childEntity);
                }
            }
            if (entity->m_contains != nullptr && !entity->m_contains->empty()) {
                entity->onUpdated();
            }
        }
    }

    return entity;
}

EntityKit * ArchetypeFactory::duplicateFactory()
{
    EntityKit * f = new ArchetypeFactory(*this);
    // Copy the defaults to the parent
    f->m_attributes = this->m_attributes;
    f->m_parent = this;
    return f;
}
