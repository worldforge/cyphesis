/*
 Copyright (C) 2014 Erik Ogenvik

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

#include "InventoryDomain.h"
#include "rules/LocatedEntity.h"

#include "common/TypeNode.h"
#include "AmountProperty.h"
#include "StackableDomain.h"
#include "ModeDataProperty.h"

#include <Atlas/Objects/Anonymous.h>

#include <unordered_set>
#include <common/operations/Update.h>

static const bool debug_flag = true;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Set;

InventoryDomain::InventoryDomain(LocatedEntity& entity) :
        Domain(entity)
{
    entity.makeContainer();
}

InventoryDomain::~InventoryDomain()
{
    //Clean up observers in a safe manner (as they will alter the m_closenessObservations map on deletion).
    std::vector<std::string> observingIds;
    observingIds.reserve(m_closenessObservations.size());
    for (auto& entry : m_closenessObservations) {
        observingIds.emplace_back(entry.first);
    }
    for (auto& entityId : observingIds) {
        removeClosenessObservation(entityId);
    }
}


void InventoryDomain::addEntity(LocatedEntity& entity)
{
    //Check if entity should be stacked.
    if (StackableDomain::stackIfPossible(m_entity, entity)) {
        return;
    }

    //TODO: make these kind of reset actions on the domain being moved out of instead. Like PhysicalDomain.
    //entity.m_location.resetTransformAndMovement();
    entity.removeFlags(entity_clean);


    //Reset any mode_data properties when moving to this domain.
    if (auto prop = entity.getPropertyClassFixed<ModeDataProperty>()) {
        if (prop->getMode() == ModeProperty::Mode::Planted) {
            auto& plantedOnData = prop->getPlantedOnData();
            //Check that we've moved from another entity.
            if (plantedOnData.entityId && (plantedOnData.entityId != m_entity.getIntId())) {
                entity.setAttrValue(ModeDataProperty::property_name, Atlas::Message::Element());
                entity.enqueueUpdateOp();
            }
        }
    }
}

void InventoryDomain::removeClosenessObservation(const std::string& entityId)
{
    //Since closeness observation callbacks can trigger alterations to the collections we need to extra precautions to iterate in a safe manner, which allows outside modifications.
    auto I = m_closenessObservations.find(entityId);
    while (I != m_closenessObservations.end()) {
        auto& observations = I->second;
        if (observations.empty()) {
            m_closenessObservations.erase(I);
            break;
        } else {
            auto J = observations.begin();
            auto obs = std::move(J->second);
            observations.erase(J);
            obs->callback();
        }
        I = m_closenessObservations.find(entityId);
    }

}


void InventoryDomain::removeEntity(LocatedEntity& entity)
{
    removeClosenessObservation(entity.getId());
}

bool InventoryDomain::isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const
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

    //Entities can only be seen by outside observers if they are attached.
    auto modeDataProp = observedEntity.getPropertyClassFixed<ModeDataProperty>();
    return modeDataProp && modeDataProp->getMode() == ModeProperty::Mode::Planted
           && modeDataProp->getPlantedOnData().entityId
           && *modeDataProp->getPlantedOnData().entityId == m_entity.getIntId();

}

void InventoryDomain::getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const
{
    if (m_entity.m_contains) {
        for (auto& entity : *m_entity.m_contains) {
            if (isEntityVisibleFor(observingEntity, *entity)) {
                entityList.push_back(entity.get());
            }
        }
    }
}

std::vector<LocatedEntity*> InventoryDomain::getObservingEntitiesFor(const LocatedEntity& observedEntity) const
{
    std::vector<LocatedEntity*> list;
    list.push_back(&m_entity);
    return list;
}

bool InventoryDomain::isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const
{
    //Only the entity to which the inventory belongs is allowed to reach things in the inventory.
    return &reachingEntity == &m_entity;
}

std::vector<Domain::CollisionEntry> InventoryDomain::queryCollision(const WFMath::Ball<3>& sphere) const
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

boost::optional<std::function<void()>> InventoryDomain::observeCloseness(LocatedEntity& reacher, LocatedEntity& target, double reach, std::function<void()> callback)
{
    if (&reacher == &m_entity) {
        auto obs = new ClosenessObserverEntry{target, callback};
        m_closenessObservations[target.getId()].emplace(obs, std::unique_ptr<ClosenessObserverEntry>(obs));
        return boost::optional<std::function<void()>>([this, &target, obs]() {
            auto I = m_closenessObservations.find(target.getId());
            if (I != m_closenessObservations.end()) {
                auto J = I->second.find(obs);
                if (J != I->second.end()) {
                    I->second.erase(J);
                    if (I->second.empty()) {
                        m_closenessObservations.erase(I);
                    }
                }
            }
        });
    }
    return boost::none;
}
