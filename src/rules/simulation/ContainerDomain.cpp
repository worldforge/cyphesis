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
        Domain(entity),
        mContainerAccessProperty(*entity.requirePropertyClassFixed<ContainerAccessProperty>())
{
    entity.makeContainer();
}

void ContainerDomain::addEntity(LocatedEntity& entity)
{
    //Check if entity should be stacked.
    if (StackableDomain::stackIfPossible(m_entity, entity)) {
        return;
    }

    entity.m_location.resetTransformAndMovement();

    entity.removeFlags(entity_clean);

    //Reset any mode_data properties when moving to this domain.
    if (auto prop = entity.modPropertyClassFixed<ModeDataProperty>()) {
        prop->clearData();
    }

    auto& entries = mContainerAccessProperty.getEntries();
    for (auto& entry: entries) {
//        if (entry.second.observer->hasFlags(entity_admin) || entity.hasFlags(entity_contained_visible)) {
            entry.second.observedEntities.push_back(&entity);
 //       }
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

    if (observedEntity.hasFlags(entity_contained_visible)) {
        return true;
    }

    //Entities can only be seen by outside observers if the outside entity can reach this.
    return mContainerAccessProperty.hasEntity(observingEntity);


    // return observingEntity.canReach(m_entity.m_location);
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
    auto& entries = mContainerAccessProperty.getEntries();
    for (auto& entry: entries) {
        if (entry.second.observer->hasFlags(entity_admin) || observedEntity.hasFlags(entity_contained_visible)) {
            list.push_back(entry.second.observer.get());
        } else {
            if (std::find(entry.second.observedEntities.begin(), entry.second.observedEntities.end(), &observedEntity) != entry.second.observedEntities.end()) {
                list.push_back(entry.second.observer.get());
            }
        }
    }
    if (m_entity.hasFlags(entity_perceptive)) {
        list.push_back(&m_entity);
    }
    return list;
}

bool ContainerDomain::isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const
{
    return mContainerAccessProperty.hasEntity(reachingEntity);
}

std::vector<Domain::CollisionEntry> ContainerDomain::queryCollision(const WFMath::Ball<3>& sphere) const
{
    std::vector<Domain::CollisionEntry> entries;
    if (m_entity.m_contains) {
        entries.reserve(m_entity.m_contains->size());
        for (auto& child: *m_entity.m_contains) {
            entries.emplace_back(Domain::CollisionEntry{child.get(), WFMath::Point<3>::ZERO(), 0.0f});
        }
    }
    return entries;
}

boost::optional<std::function<void()>> ContainerDomain::observeCloseness(LocatedEntity& reacher, LocatedEntity& target, double reach, std::function<void()> callback)
{
    if (m_entity.m_contains) {
        auto I = std::find_if(m_entity.m_contains->begin(), m_entity.m_contains->end(), [&target](const Ref<LocatedEntity>& child) { return child.get() == &target; });
        if (I != m_entity.m_contains->end()) {
            //TODO: add listeners for when the entity leaves the container
            return boost::optional<std::function<void()>>([this]() {

            });

        }
    }
    return boost::none;
}
