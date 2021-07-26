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
#include "rules/simulation/BaseWorld.h"
#include "AmountProperty.h"
#include "ModeDataProperty.h"

#include <Atlas/Objects/Anonymous.h>

#include <unordered_set>
#include <Atlas/Objects/Operation.h>
#include "common/Property_impl.h"

static const bool debug_flag = true;

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::smart_dynamic_cast;

std::vector<std::string> StackableDomain::sIgnoredProps = {"pos", "orientation", "mode_data", "amount", "id", "stamp", "mode"};


StackableDomain::StackableDomain(LocatedEntity& entity) :
        Domain(entity)
{
    entity.addFlags(entity_stacked);
}

void StackableDomain::addEntity(LocatedEntity& entity)
{
    //TODO: make these kind of reset actions on the domain being moved out of instead. Like PhysicalDomain.
    //entity.m_location.resetTransformAndMovement();
    entity.removeFlags(entity_clean);

    if (m_entity.getType() == entity.getType() && m_entity.hasFlags(entity_stacked)) {
        if (checkEntitiesStackable(m_entity, entity)) {
            //Entity can be stacked.
            auto& newEntityStackProp = entity.requirePropertyClassFixed<AmountProperty>(1);

            auto& stackProp = m_entity.requirePropertyClassFixed<AmountProperty>(1);
            stackProp.data() += newEntityStackProp.data();
            stackProp.removeFlags(prop_flag_persistence_clean);
            m_entity.applyProperty(stackProp);

            newEntityStackProp.data() = 0;
            entity.applyProperty(newEntityStackProp);

            m_entity.enqueueUpdateOp();
            return;
        }
    }

    //Reset any mode_data properties when moving to this domain.
    if (auto prop = entity.getPropertyClassFixed<ModeDataProperty>()) {
        //Check that we've moved from another entity.
        if (prop->getMode() != ModeProperty::Mode::Unknown) {
            entity.setAttrValue(ModeDataProperty::property_name, Atlas::Message::Element());
            entity.enqueueUpdateOp();
        }
    }
}

void StackableDomain::installDelegates(LocatedEntity& entity, const std::string& propertyName)
{
    entity.installDelegate(Atlas::Objects::Operation::DELETE_NO, propertyName);
    entity.installDelegate(Atlas::Objects::Operation::MOVE_NO, propertyName);
}


HandlerResult StackableDomain::operation(LocatedEntity& entity, const Operation& op, OpVector& res)
{
    switch (op->getClassNo()) {
        case Atlas::Objects::Operation::DELETE_NO:
            return DeleteOperation(entity, op, res);
        case Atlas::Objects::Operation::MOVE_NO:
            return MoveOperation(entity, op, res);
        default:
            return OPERATION_IGNORED;
    }
}

HandlerResult StackableDomain::DeleteOperation(LocatedEntity& owner, const Operation& op, OpVector& res)
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

    if (amount < 0) {
        return OPERATION_BLOCKED;
    }


    auto& amountProperty = owner.requirePropertyClassFixed<AmountProperty>(1);

    if (amountProperty.data() - amount > 0) {
        amountProperty.data() -= amount;
        owner.applyProperty(amountProperty);

        owner.enqueueUpdateOp(res);
        return OPERATION_BLOCKED;
    }
    return OPERATION_IGNORED;
}

HandlerResult StackableDomain::MoveOperation(LocatedEntity& owner, const Operation& op, OpVector& res)
{
    // Check the validity of the operation.
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        return OPERATION_IGNORED;
    }
    RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
    if (!ent.isValid() || ent->isDefaultId()) {
        return OPERATION_IGNORED;
    }
    if (owner.getId() != ent->getId()) {
        return OPERATION_IGNORED;
    }

    auto& amountProperty = owner.requirePropertyClassFixed<AmountProperty>(1);

    //By default move one, unless another value is specified in "amount" in the first arg of the op.
    int amount = 1;

    Atlas::Message::Element amountElem;
    if (ent->copyAttr("amount", amountElem) == 0 && amountElem.isInt()) {
        amount = amountElem.Int();
    }

    amount = std::max(0, std::min(amountProperty.data(), amount));

    if (amount == 0) {
        log(WARNING, String::compose("Got move op in StackableDomain with 0 or less amount, for %1", owner.describeEntity()));
        return HandlerResult::OPERATION_BLOCKED;
    }

    //If we're moving all items just ignore it here and let the standard move code apply.
    if (amount == amountProperty.data()) {
        return HandlerResult::OPERATION_IGNORED;
    }

    //First create a new entity, a copy of this one.
    Atlas::Objects::Entity::Anonymous new_ent;
    for (const auto& entry : owner.getProperties()) {
        Atlas::Message::Element elem;
        entry.second.property->get(elem);
        try {
            new_ent->setAttr(entry.first, std::move(elem));
        } catch (const Atlas::Message::WrongTypeException& ex) {
            log(WARNING, String::compose("Error when setting attribute '%1' when copying entity '%2' with Stackable domain. Error message: %3", entry.first, owner.describeEntity(), ex.what()));
        }
    }
    //Make sure to adjust the amount
    new_ent->setAttr(AmountProperty::property_name, amount);
    new_ent->setLoc("0"); //Place in top world void domain as interim, since it will be moved immediately.
    auto obj = BaseWorld::instance().addNewEntity(owner.getType()->name(), new_ent);

    if (!obj) {
        owner.error(op, "Create op failed.", res, op->getFrom());
        return HandlerResult::OPERATION_BLOCKED;
    }

    //We now need to decrease the amount of the original entity with as much.
    amountProperty.data() -= amount;
    owner.applyProperty(amountProperty);

    owner.enqueueUpdateOp(res);

    //and finally we'll issue a new Move operation moving the new entity.
    // Since the move op has the same amount as the entity, this handler will ignore it.

    auto newMoveOp = op.copy();
    newMoveOp->setTo(obj->getId());
    newMoveOp->getArgs().front()->setId(obj->getId());
    newMoveOp->getArgs().front()->setAttr("amount", amount);
    //Send the move op directly.
    OpVector new_res;
    obj->operation(newMoveOp, new_res);
    for (auto& resOp: new_res) {
        obj->sendWorld(resOp);
    }


//    res.emplace_back(std::move(newMoveOp));

    return OPERATION_BLOCKED;

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

std::vector<LocatedEntity*> StackableDomain::getObservingEntitiesFor(const LocatedEntity& observedEntity) const
{
    return {};
}

bool StackableDomain::isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const
{
    //If it can reach the stackable entity, it can reach any children
    return true;
}

bool StackableDomain::checkEntitiesStackable(const LocatedEntity& first, const LocatedEntity& second)
{
    std::map<std::string, PropertyBase*> firstProps;
    std::map<std::string, PropertyBase*> secondProps;

    for (auto& entry: first.getProperties()) {
        firstProps.emplace(entry.first, entry.second.property.get());
    }
    for (auto& entry: second.getProperties()) {
        secondProps.emplace(entry.first, entry.second.property.get());
    }

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

bool StackableDomain::stackIfPossible(const LocatedEntity& domainEntity, LocatedEntity& entity)
{
    if (entity.hasFlags(entity_stacked) && domainEntity.m_contains) {
        for (const auto& child : *domainEntity.m_contains) {
            if (child != &entity && child->getType() == entity.getType() && child->hasFlags(entity_stacked)) {
                if (StackableDomain::checkEntitiesStackable(*child, entity)) {
                    //Entity can be stacked.
                    auto& newEntityStackProp = entity.requirePropertyClassFixed<AmountProperty>(1);

                    auto& stackProp = child->requirePropertyClassFixed<AmountProperty>(1);
                    stackProp.data() += newEntityStackProp.data();
                    stackProp.removeFlags(prop_flag_persistence_clean);
                    child->applyProperty(stackProp);

                    newEntityStackProp.data() = 0;
                    entity.applyProperty(newEntityStackProp);

                    child->enqueueUpdateOp();
                    return true;
                }
            }
        }
    }
    return false;
}

boost::optional<std::function<void()>> StackableDomain::observeCloseness(LocatedEntity& reacher, LocatedEntity& target, double reach, std::function<void()> callback)
{
    return boost::none;
}
