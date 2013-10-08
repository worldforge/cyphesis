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
#include "common/Think.h"

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/objectFactory.h>

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Factories;

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

LocatedEntity * ArchetypeFactory::createEntity(const std::string & id,
        long intId, const RootEntity & attributes, LocatedEntity* location,
        std::map<std::string, RootEntity>& entities,
        std::map<std::string, LocatedEntity*>& resolvedEntities)
{
    std::string concreteType = attributes->getParents().front();

    MapType attrMap;
    attributes->addToMessage(attrMap);
    RootEntity cleansedAttributes;
    for (auto attr : attrMap) {
        if (attr.second.isString()) {
            const std::string& attr_string = attr.second.asString();
            if (!attr_string.empty() && attr_string[0] == '@') {
                continue;
            }
        }
        cleansedAttributes->setAttr(attr.first, attr.second);
    }

    //If no position is set, make sure it's zeroed
    if (cleansedAttributes->isDefaultPos()) {
        std::vector<double> pos;
        pos.push_back(0.0f);
        pos.push_back(0.0f);
        pos.push_back(0.0f);
        cleansedAttributes->setPos(pos);
    }

    LocatedEntity* entity = EntityBuilder::instance()->newChildEntity(id, intId,
            concreteType, cleansedAttributes, *location);

    if (entity == nullptr) {
        log(ERROR,
                String::compose("Could not create entity of type %1.",
                        concreteType));
        return nullptr;
    }

    if (!attributes->isDefaultId()) {
        resolvedEntities.insert(std::make_pair(attributes->getId(), entity));
    }

    for (auto& childId : attributes->getContains()) {
        auto entityI = entities.find(childId);
        if (entityI == entities.end()) {
            log(ERROR, "References child entity does not exist.");
            return nullptr;
        }
        std::string childId;
        long childIntId = newId(childId);
        LocatedEntity* childEntity = createEntity(childId, childIntId,
                entityI->second, entity, entities, resolvedEntities);
        if (childEntity == nullptr) {
            log(ERROR, "Could not create child entity.");
            return nullptr;
        }
        entity->makeContainer();
        entity->m_contains->insert(childEntity);
        entity->incRef();
    }

    if (entity->m_contains != nullptr && !entity->m_contains->empty()) {
        entity->onUpdated();
    }

    return entity;
}

LocatedEntity * ArchetypeFactory::newEntity(const std::string & id, long intId,
        const RootEntity & attributes, LocatedEntity* location)
{
    //parse entities into RootEntity instances first
    std::map<std::string, RootEntity> entities;

    for (auto& entityElem : m_entities) {
        if (entityElem.isMap()) {
            auto entity = smart_dynamic_cast<RootEntity>(
                    Factories::instance()->createObject(entityElem.asMap()));
            if (!entity.isValid()) {
                log(ERROR, "Entity definition is not in Entity format.");
                return nullptr;
            }
            entities.insert(std::make_pair(entity->getId(), entity));
        }
    }

    if (entities.empty()) {
        return nullptr;
    }

    RootEntity attrEntity = entities.begin()->second;
    MapType attrs = attributes->asMessage();
    for (auto attrI : attrs) {
        //copy all attributes except "parents", since that will point to the name of the archetype
        if (attrI.first != "parents") {
            attrEntity->setAttr(attrI.first, attrI.second);
        }
    }
    std::map<std::string, LocatedEntity*> resolvedEntities;
    LocatedEntity* entity = createEntity(id, intId, attrEntity, location,
            entities, resolvedEntities);

    MapType entityAttributes;
    attrEntity->addToMessage(entityAttributes);
    for (auto attr : entityAttributes) {
        if (attr.second.isString()) {
            const std::string& attr_string = attr.second.asString();
            if (!attr_string.empty() && attr_string[0] == '@') {
                std::string id = attr_string.substr(1,
                        attr_string.length() - 1);
                auto resolvedI = resolvedEntities.find(id);
                if (resolvedI != resolvedEntities.end()) {
                    entity->setAttr(attr.first, resolvedI->second);
                }
            }
        }
    }

    if (entity != nullptr) {
        sendThoughts(*entity);
    }
    return entity;
}

void ArchetypeFactory::sendThoughts(LocatedEntity& entity)
{
    //Send any thoughts.
    //Note that we currently only allow for thoughts for the top entity,
    //even though the format they are stored in would allow for thoughts for
    //many entities (by using the id).

    if (!m_thoughts.empty()) {
        Atlas::Objects::Operation::Think thoughtOp;
        thoughtOp->setArgsAsList(m_thoughts);
        //Make the thought come from the entity itself
        thoughtOp->setTo(entity.getId());
        thoughtOp->setFrom(entity.getId());
        WorldRouter::instance().message(thoughtOp, entity);
    }

}

ArchetypeFactory * ArchetypeFactory::duplicateFactory()
{
    ArchetypeFactory * f = new ArchetypeFactory(*this);
    // Copy the defaults to the parent
    f->m_entities = this->m_entities;
    f->m_thoughts = this->m_thoughts;
    f->m_parent = this;
    return f;
}

void ArchetypeFactory::addProperties()
{
    assert(m_type != 0);
    MapType attributes;
    attributes.insert(std::make_pair("entities", m_entities));
    attributes.insert(std::make_pair("thoughts", m_thoughts));
    m_type->addProperties(attributes);

}

void ArchetypeFactory::updateProperties()
{
    assert(m_type != 0);
    MapType attributes;
    attributes.insert(std::make_pair("entities", m_entities));
    attributes.insert(std::make_pair("thoughts", m_thoughts));
    m_type->addProperties(attributes);
}
