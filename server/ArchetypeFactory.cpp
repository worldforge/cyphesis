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

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/objectFactory.h>

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
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
        long intId, EntityCreation& entityCreation, LocatedEntity* location,
        std::map<std::string, EntityCreation>& entities)
{
    auto& attributes = entityCreation.definition;
    std::string concreteType = attributes->getParents().front();

    MapType attrMap;
    attributes->addToMessage(attrMap);
    RootEntity cleansedAttributes;
    for (auto& attr : attrMap) {
        if (isEntityRefAttribute(attr.second)) {
            entityCreation.unresolvedAttributes.insert(attr);
        } else {
            cleansedAttributes->setAttr(attr.first, attr.second);
        }
    }

    //If no position is set, make sure it's zeroed
    if (cleansedAttributes->isDefaultPos()) {
        ::addToEntity(Point3D::ZERO(), cleansedAttributes->modifyPos());
    }

    LocatedEntity* entity = EntityBuilder::instance()->newChildEntity(id, intId,
            concreteType, cleansedAttributes, *location);

    if (entity == nullptr) {
        log(ERROR,
                String::compose("Could not create entity of type %1.",
                        concreteType));
        return nullptr;
    }

    entityCreation.createdEntity = entity;

    for (auto& childId : attributes->getContains()) {
        auto entityI = entities.find(childId);
        if (entityI == entities.end()) {
            log(ERROR,
                    String::compose(
                            "Referenced child entity with id %1 does not exist.",
                            childId));
            return nullptr;
        }
        std::string childEntityId;
        long childIntId = newId(childEntityId);
        LocatedEntity* childEntity = createEntity(childEntityId, childIntId,
                entityI->second, entity, entities);
        if (childEntity == nullptr) {
            log(ERROR,
                    String::compose("Could not create child entity with id %1.",
                            childId));
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

bool ArchetypeFactory::parseEntities(const ListType& entitiesElement,
        std::map<std::string, EntityCreation>& entities)
{
    std::map<std::string, MapType> entityMap;
    for (auto& entityElem : entitiesElement) {
        if (entityElem.isMap()) {
            std::string id;
            auto I = entityElem.asMap().find("id");
            if (I != entityElem.asMap().end() && I->second.isString()) {
                id = I->second.asString();
            }
            entityMap.insert(std::make_pair(id, entityElem.asMap()));
        }
    }
    return parseEntities(entityMap, entities);
}

bool ArchetypeFactory::parseEntities(
        const std::map<std::string, MapType>& entitiesElement,
        std::map<std::string, EntityCreation>& entities)
{
    for (auto& entityI : entitiesElement) {

        auto entity = smart_dynamic_cast<RootEntity>(
                Factories::instance()->createObject(entityI.second));
        if (!entity.isValid()) {
            log(ERROR, "Entity definition is not in Entity format.");
            return false;
        }

        auto result = entities.insert(
                std::make_pair(entity->getId(), EntityCreation { entity,
                        nullptr, Atlas::Message::MapType() }));
        if (!result.second) {
            //it already existed; we should update with the attributes
            for (auto& I : entityI.second) {
                result.first->second.definition->setAttr(I.first, I.second);
            }
        }
    }
    return true;
}

LocatedEntity * ArchetypeFactory::newEntity(const std::string & id, long intId,
        const RootEntity & attributes, LocatedEntity* location)
{
    //parse entities into RootEntity instances first
    std::map<std::string, EntityCreation> entities;
    std::vector<Atlas::Message::Element> extraThoughts;

    bool parseResult = parseEntities(m_entities, entities);
    if (!parseResult) {
        return nullptr;
    }
    MapType attrs;

    //If the object type of the attributes is "archetype" we should merge it's
    //"entities" and "thoughts" attributes.
    if (attributes->getObjtype() == "archetype") {
        if (attributes->hasAttr("entities")) {
            auto& entitiesElem = attributes->getAttr("entities");
            if (!entitiesElem.isList()) {
                log(WARNING, "'entities' attribute is not a list.");
            } else {
                const ListType& entitiesList = entitiesElem.asList();
                parseEntities(entitiesList, entities);
            }
        }

        if (attributes->hasAttr("thoughts")) {
            auto& thoughtsElem = attributes->getAttr("thoughts");
            if (!thoughtsElem.isList()) {
                log(WARNING, "'thoughts' attribute is not a list.");
            } else {
                extraThoughts = thoughtsElem.asList();
            }
        }

    } else {
        //If no, we should consider the attributes to only apply to the first entity
        attrs = attributes->asMessage();
    }

    if (entities.empty()) {
        return nullptr;
    }
    auto& entityCreation = entities.begin()->second;
    RootEntity& attrEntity = entityCreation.definition;
    for (auto& attrI : attrs) {
        //copy all attributes except "parents", since that will point to the name of the archetype
        if (attrI.first != "parents") {
            attrEntity->setAttr(attrI.first, attrI.second);
        }
    }

    if (!attributes->isDefaultPos()) {
        attrEntity->modifyPos() = attributes->getPos();
    }
    if (attributes->hasAttr("orientation")) {
        attrEntity->setAttr("orientation", attributes->getAttr("orientation"));
    }
    LocatedEntity* entity = createEntity(id, intId, entityCreation, location,
            entities);

    if (entity != nullptr) {
        processResolvedAttributes(entities);
        if (!m_thoughts.empty() || !extraThoughts.empty()) {
            //We must send a sight op to the entity informing it of itself before we send any thoughts.
            //Else the mind won't have any information about itself and the thoughts will
            //cause errors.
            sendInitialSight(*entity);
            sendThoughts(*entity, m_thoughts);
            sendThoughts(*entity, extraThoughts);
        }
    }
    return entity;
}

void ArchetypeFactory::processResolvedAttributes(
        std::map<std::string, EntityCreation>& entities)
{
    for (auto& entityI : entities) {
        if (entityI.second.createdEntity != nullptr
                && !entityI.second.unresolvedAttributes.empty()) {
            for (auto& attrI : entityI.second.unresolvedAttributes) {
                Atlas::Message::Element& attr = attrI.second;
                resolveEntityReference(entities, attr);
                entityI.second.createdEntity->setAttr(attrI.first, attr);
            }
        }
    }
}

bool ArchetypeFactory::isEntityRefAttribute(
        const Atlas::Message::Element& attr) const
{
    if (attr.isString()) {
        //This is a bit naive, but will work for now: we'll just
        //check if the first character is '@'
        const std::string& attr_string = attr.asString();
        if (!attr_string.empty() && attr_string[0] == '@') {
            return true;
        }
    }
    if (attr.isMap()) {
        //If it's a map we need to process all child elements too
        for (auto& I : attr.asMap()) {
            if (isEntityRefAttribute(I.second)) {
                return true;
            }
        }
    } else if (attr.isList()) {
        //If it's a list we need to process all child elements too
        for (auto& I : attr.asList()) {
            if (isEntityRefAttribute(I)) {
                return true;
            }
        }
    }
    return false;
}

void ArchetypeFactory::resolveEntityReference(
        std::map<std::string, EntityCreation>& entities,
        Atlas::Message::Element& attr)
{
    //Only handle strings
    if (attr.isString()) {
        const std::string& attr_string = attr.asString();
        //This is a bit naive, but will work for now: we'll just
        //check if the first character is '@'
        if (!attr_string.empty() && attr_string[0] == '@') {
            std::string id = attr_string.substr(1, attr_string.length() - 1);
            auto resolvedI = entities.find(id);
            if (resolvedI != entities.end()) {
                if (resolvedI->second.createdEntity != nullptr) {
                    attr = Atlas::Message::Element(
                            resolvedI->second.createdEntity);
                } else {
                    log(WARNING,
                            String::compose(
                                    "Attribute '%1' refers to an entity which wasn't created.",
                                    id));
                }
            } else {
                log(WARNING,
                        String::compose("Could not find entity with id '%1'.",
                                id));
            }
        }
    } else if (attr.isMap()) {
        //If it's a map we need to process all child elements too
        for (auto& I : attr.asMap()) {
            resolveEntityReference(entities, I.second);
        }
    } else if (attr.isList()) {
        //If it's a list we need to process all child elements too
        for (auto& I : attr.asList()) {
            resolveEntityReference(entities, I);
        }
    }
}

void ArchetypeFactory::sendInitialSight(LocatedEntity& entity)
{
    Atlas::Objects::Operation::Sight sight;
    sight->setTo(entity.getId());
    Atlas::Objects::Entity::Anonymous args;
    entity.addToEntity(args);
    sight->setArgs1(args);
    entity.sendWorld(sight);
}

void ArchetypeFactory::sendThoughts(LocatedEntity& entity,
        std::vector<Atlas::Message::Element>& thoughts)
{
    //Send any thoughts.
    //Note that we currently only allow for thoughts for the top entity,
    //even though the format they are stored in would allow for thoughts for
    //many entities (by using the id).

    if (!thoughts.empty()) {
        Atlas::Objects::Operation::Think thoughtOp;
        thoughtOp->setArgsAsList(thoughts);
        //Make the thought come from the entity itself
        thoughtOp->setTo(entity.getId());
        thoughtOp->setFrom(entity.getId());
        entity.sendWorld(thoughtOp);
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
    ListType entities;
    for (auto I : m_entities) {
        entities.push_back(I.second);
    }
    attributes.insert(std::make_pair("entities", entities));
    attributes.insert(std::make_pair("thoughts", m_thoughts));
    m_type->addProperties(attributes);

}

void ArchetypeFactory::updateProperties()
{
    assert(m_type != 0);
    MapType attributes;
    ListType entities;
    for (auto I : m_entities) {
        entities.push_back(I.second);
    }
    attributes.insert(std::make_pair("entities", entities));
    attributes.insert(std::make_pair("thoughts", m_thoughts));
    m_type->updateProperties(attributes);

    for (auto& child_factory : m_children) {
        child_factory->m_thoughts = m_thoughts;
        child_factory->m_thoughts.insert(child_factory->m_thoughts.end(),
                child_factory->m_classThoughts.begin(),
                child_factory->m_classThoughts.end());

        child_factory->m_entities = m_entities;

        child_factory->updateProperties();
    }

}
