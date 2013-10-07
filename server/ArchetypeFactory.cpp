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
        std::map<std::string, RootEntity>& entities)
{
    std::string concreteType = attributes->getParents().front();

    LocatedEntity* entity = EntityBuilder::instance()->newChildEntity(id, intId,
            concreteType, attributes, *location);

    if (entity == nullptr) {
        log(ERROR,
                String::compose("Could not create entity of type %1.",
                        concreteType));
        return nullptr;
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
                entityI->second, entity, entities);
        if (childEntity == nullptr) {
            log(ERROR, "Could not create child entity.");
            return nullptr;
        }
        entity->makeContainer();
        entity->m_contains->insert(childEntity);
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

    for (RootEntity entity : m_entities) {
        if (!entity.isValid()) {
            log(ERROR, "Entity definition is not in Entity format.");
            return nullptr;
        }
        entities.insert(std::make_pair(entity->getId(), entity));
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
    LocatedEntity* entity = createEntity(id, intId, attrEntity, location,
            entities);

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

}

void ArchetypeFactory::updateProperties()
{

}
