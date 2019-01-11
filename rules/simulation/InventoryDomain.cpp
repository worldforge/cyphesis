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

#ifdef HAVE_CONFIG_H
#endif

#include "InventoryDomain.h"
#include "EntityProperty.h"
#include "rules/LocatedEntity.h"

#include "common/TypeNode.h"
#include "common/debug.h"
#include "common/operations/Unseen.h"
#include "PlantedOnProperty.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <iostream>
#include <unordered_set>
#include <rules/simulation/BaseWorld.h>
#include <common/operations/Update.h>

static const bool debug_flag = true;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Unseen;

InventoryDomain::InventoryDomain(LocatedEntity& entity) :
        Domain(entity)
{
    entity.makeContainer();
}

void InventoryDomain::tick(double t, OpVector& res)
{
}

void InventoryDomain::addEntity(LocatedEntity& entity)
{
    entity.m_location.m_pos = WFMath::Point<3>::ZERO();
    entity.m_location.m_orientation = WFMath::Quaternion::IDENTITY();
//    entity.m_location.update(BaseWorld::instance().getTime());
    entity.removeFlags(entity_clean);

    //Reset any planted_on properties when moving to this domain.
    if (auto prop = entity.getPropertyClassFixed<PlantedOnProperty>()) {
        if (prop->data()) {
            entity.setAttr(PlantedOnProperty::property_name, Atlas::Message::Element());
            Atlas::Objects::Operation::Update update;
            update->setTo(entity.getId());
            entity.sendWorld(update);
        }
    }
}

void InventoryDomain::removeEntity(LocatedEntity& entity)
{
    //Nothing special to do for this domain.
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

    //Entities can only be seen by outside observers if they are attached.
    auto plantedOnProp = observedEntity.getPropertyClassFixed<PlantedOnProperty>();
    return plantedOnProp && plantedOnProp->data().entity.get() == &m_entity;

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

std::list<LocatedEntity*> InventoryDomain::getObservingEntitiesFor(const LocatedEntity& observedEntity) const
{
    std::list<LocatedEntity*> list;
    list.push_back(&m_entity);
    return list;
}

bool InventoryDomain::isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const
{
    //Only the entity to which the inventory belongs is allowed to reach things in the inventory.
    return &reachingEntity == &m_entity;
}

