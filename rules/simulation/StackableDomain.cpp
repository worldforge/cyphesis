/*
 Copyright (C) 2019 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "StackableDomain.h"

#include "rules/LocatedEntity.h"

#include "common/TypeNode.h"
#include "PlantedOnProperty.h"
#include "AmountProperty.h"

#include <Atlas/Objects/Anonymous.h>

#include <unordered_set>
#include <common/operations/Update.h>
#include <Atlas/Objects/Operation.h>

static const bool debug_flag = true;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Unseen;

std::vector<std::string> StackableDomain::sIgnoredProps = {"pos", "orientation", "planted_on", "stack", "id", "stamp"};


StackableDomain::StackableDomain(LocatedEntity& entity) :
    Domain(entity)
{
    entity.addFlags(entity_stacked);
}

void StackableDomain::addEntity(LocatedEntity& entity)
{
    if (m_entity.getType() == entity.getType() && m_entity.hasFlags(entity_stacked)) {
        if (checkEntitiesStackable(m_entity, entity)) {
            //Entity can be stacked.
            auto newEntityStackProp = entity.requirePropertyClassFixed<AmountProperty>(1);

            auto stackProp = m_entity.requirePropertyClassFixed<AmountProperty>(1);
            stackProp->data() += newEntityStackProp->data();
            stackProp->removeFlags(per_clean);
            m_entity.applyProperty(AmountProperty::property_name, stackProp);

            newEntityStackProp->data() = 0;
            entity.applyProperty(AmountProperty::property_name, newEntityStackProp);

            Atlas::Objects::Operation::Update update;
            update->setTo(m_entity.getId());
            m_entity.sendWorld(update);
            return;
        }
    }

    entity.m_location.m_pos = WFMath::Point<3>::ZERO();
    entity.m_location.m_orientation = WFMath::Quaternion::IDENTITY();
//    entity.m_location.update(BaseWorld::instance().getTime());
    entity.removeFlags(entity_clean);

    //Reset any planted_on properties when moving to this domain.
    if (auto prop = entity.getPropertyClassFixed<PlantedOnProperty>()) {
        //Check that we've moved from another entity.
        if (prop->data() && (!prop->data().entity || prop->data().entity->getId() != m_entity.getId())) {
            entity.setAttr(PlantedOnProperty::property_name, Atlas::Message::Element());
            Atlas::Objects::Operation::Update update;
            update->setTo(entity.getId());
            entity.sendWorld(update);
        }
    }
}

void StackableDomain::installDelegates(LocatedEntity* entity, const std::string& propertyName)
{
    entity->installDelegate(Atlas::Objects::Operation::DELETE_NO, propertyName);
}


HandlerResult StackableDomain::operation(LocatedEntity* entity, const Operation& op, OpVector& res)
{
    switch (op->getClassNo()) {
        case Atlas::Objects::Operation::DELETE_NO:
            return DeleteOperation(entity, op, res);
        default:
            return OPERATION_IGNORED;
    }
}

HandlerResult StackableDomain::DeleteOperation(LocatedEntity* owner, const Operation& op, OpVector& res)
{
    //By default remove one, unless another value is specified in "amount" in the first arg of the op.
    int amount = 1;

    if (!op->getArgs().empty()) {
        auto& firstArg = op->getArgs().front();
        Atlas::Message::Element elem;
        if (firstArg->copyAttr("amount", elem) == 0 && elem.isInt()) {
            amount = elem.Int();
        }
    }


    auto amountProperty = owner->requirePropertyClassFixed<AmountProperty>(1);

    if (amountProperty->data() - amount > 0) {
        amountProperty->data() -= amount;
        owner->applyProperty(AmountProperty::property_name, amountProperty);

        Atlas::Objects::Operation::Update update;
        update->setTo(owner->getId());

        res.emplace_back(std::move(update));
        return OPERATION_HANDLED;
    }
    return OPERATION_IGNORED;
}


void StackableDomain::removeEntity(LocatedEntity& entity)
{
    //Nothing special to do for this domain.
}

bool StackableDomain::isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const
{
    //If it can see the stackable entity, it can see any children
    return true;
}

void StackableDomain::getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const
{
    //If it can see the stackable entity, it can see any children
    if (m_entity.m_contains) {
        for (auto& entity : *m_entity.m_contains) {
            entityList.push_back(entity.get());
        }
    }
}

std::list<LocatedEntity*> StackableDomain::getObservingEntitiesFor(const LocatedEntity& observedEntity) const
{
    std::list<LocatedEntity*> list;
//    list.push_back(&m_entity);
    return list;
}

bool StackableDomain::isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const
{
    //If it can reach the stackable entity, it can reach any children
    return true;
}

bool StackableDomain::checkEntitiesStackable(const LocatedEntity& first, const LocatedEntity& second)
{
    PropertyDict firstProps = first.getProperties();
    PropertyDict secondProps = first.getProperties();
    for (auto& propName : sIgnoredProps) {
        firstProps.erase(propName);
        secondProps.erase(propName);
    }

    if (firstProps.size() != secondProps.size()) {
        return false;
    }


    for (const auto& firstEntry : firstProps) {
        auto secondPropI = secondProps.find(firstEntry.first);
        if (secondPropI == secondProps.end()) {
            return false;
        }
        if (*secondPropI->second != *firstEntry.second) {
            return false;
        }
    }
    return true;

}
