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

#include "ContainerDomain.h"
#include "rules/LocatedEntity.h"

#include "common/TypeNode.h"
#include "AmountProperty.h"
#include "StackableDomain.h"
#include "ModeDataProperty.h"

#include <Atlas/Objects/Anonymous.h>

#include <unordered_set>
#include <common/operations/Update.h>

ContainerDomain::ContainerDomain(LocatedEntity& entity) :
    Domain(entity)
{
    entity.makeContainer();
}

void ContainerDomain::addEntity(LocatedEntity& entity)
{
    //Check if entity should be stacked.
    //TODO: combine this code, which is lifted from InventoryDomain
    if (entity.hasFlags(entity_stacked) && m_entity.m_contains) {
        for (const auto& child : *m_entity.m_contains) {
            if (child != &entity && child->getType() == entity.getType() && child->hasFlags(entity_stacked)) {
                if (StackableDomain::checkEntitiesStackable(*child, entity)) {
                    //Entity can be stacked.
                    auto newEntityStackProp = entity.requirePropertyClassFixed<AmountProperty>(1);

                    auto stackProp = child->requirePropertyClassFixed<AmountProperty>(1);
                    stackProp->data() += newEntityStackProp->data();
                    stackProp->removeFlags(persistence_clean);
                    child->applyProperty(AmountProperty::property_name, stackProp);

                    newEntityStackProp->data() = 0;
                    entity.applyProperty(AmountProperty::property_name, newEntityStackProp);

                    Atlas::Objects::Operation::Update update;
                    update->setTo(child->getId());
                    child->sendWorld(update);
                    return;
                }
            }
        }
    }

    entity.m_location.resetTransformAndMovement();

    entity.removeFlags(entity_clean);

    //Reset any mode_data properties when moving to this domain.
    if (auto prop = entity.modPropertyClassFixed<ModeDataProperty>()) {
        prop->clearData();
    }
}

void ContainerDomain::removeEntity(LocatedEntity& entity)
{
    //Nothing special to do for this domain.
}

bool ContainerDomain::isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const
{
    //If the observing entity is the same as the one the domain belongs to it can see everything.
    if (&observingEntity == &m_entity) {
        return true;
    }

    if (observingEntity.flags().hasFlags(entity_admin)) {
        return true;
    }

    //Entities can only be seen by outside observers if the outside entity can reach this.
    return observingEntity.canReach(m_entity.m_location);
}

void ContainerDomain::getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const
{
    if (observingEntity.canReach(m_entity.m_location)) {
        if (m_entity.m_contains) {
            for (auto& entity : *m_entity.m_contains) {
                if (isEntityVisibleFor(observingEntity, *entity)) {
                    entityList.push_back(entity.get());
                }
            }
        }
    }
}

std::list<LocatedEntity*> ContainerDomain::getObservingEntitiesFor(const LocatedEntity& observedEntity) const
{
    std::list<LocatedEntity*> list;
    list.push_back(&m_entity);
    return list;
}

bool ContainerDomain::isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const
{
    //If the container can be reached, its content can be reached.
    return reachingEntity.canReach(m_entity.m_location);
}
