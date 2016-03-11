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
#include "config.h"
#endif

#include "InventoryDomain.h"
#include "OutfitProperty.h"
#include "EntityProperty.h"
#include "LocatedEntity.h"

#include "common/const.h"

#include "common/debug.h"
#include "common/const.h"
#include "common/Unseen.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <iostream>
#include <unordered_set>

static const bool debug_flag = true;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Unseen;

InventoryDomain::InventoryDomain(LocatedEntity& entity) :
                Domain(entity)
{
    entity.makeContainer();
}

InventoryDomain::~InventoryDomain()
{
}

float InventoryDomain::constrainHeight(LocatedEntity& entity, LocatedEntity * parent, const Point3D & pos, const std::string & mode)
{
    //Nothing can move
    return 0.0f;
}

double InventoryDomain::tick(double t)
{
    return 0;
}

void InventoryDomain::addEntity(LocatedEntity& entity)
{
    //Nothing special to do for this domain.
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

    //Entities can only be seen by outside observers if they are outfitted or wielded.
    const OutfitProperty* outfitProperty = m_entity.getPropertyClass<OutfitProperty>("outfit");
    if (outfitProperty) {
        for (auto& entry : outfitProperty->data()) {
            auto outfittedEntity = entry.second.get();
            if (outfittedEntity && outfittedEntity == &observedEntity) {
                return true;
            }
        }
    }
    //If the entity isn't outfitted, perhaps it's wielded?
    const EntityProperty* rightHandWieldProperty = m_entity.getPropertyClass<EntityProperty>("right_hand_wield");
    if (rightHandWieldProperty) {
        auto entity = rightHandWieldProperty->data().get();
        if (entity && entity == &observedEntity) {
            return true;
        }
    }

    return false;
}

void InventoryDomain::processVisibilityForMovedEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res)
{
    if (m_entity.m_contains) {

        std::vector<LocatedEntity*> visibleEntities;

        std::unordered_set<int> outfitted;
        LocatedEntity* rightHandWieldedEntity = nullptr;
        const OutfitProperty* outfitProperty = m_entity.getPropertyClass<OutfitProperty>("outfit");

        if (outfitProperty) {
            for (auto& entry : outfitProperty->data()) {
                outfitted.insert(entry.second->getIntId());
            }
        }
        const EntityProperty* rightHandWieldProperty = m_entity.getPropertyClass<EntityProperty>("right_hand_wield");
        if (rightHandWieldProperty) {
            rightHandWieldedEntity = rightHandWieldProperty->data().get();
        }

        for (auto childEntity : *m_entity.m_contains) {
            if (childEntity == rightHandWieldedEntity) {
                visibleEntities.push_back(childEntity);
            } else if (outfitted.find(childEntity->getIntId()) != outfitted.end()) {
                visibleEntities.push_back(childEntity);
            }
        }

        std::set<std::string> newVisibleEntities;
        for (auto visibleEntity : visibleEntities) {
            if (m_lastVisibleEntities.find(visibleEntity->getId()) == m_lastVisibleEntities.end()) {
                Anonymous ent;
                ent->setId(visibleEntity->getId());
                ent->setStamp(visibleEntity->getSeq());

                Appearance d;
                d->setArgs1(ent);
                res.push_back(d);
            } else {
                m_lastVisibleEntities.erase(visibleEntity->getId());
            }
            newVisibleEntities.insert(visibleEntity->getId());
        }
        for (auto entityId : m_lastVisibleEntities) {
            Anonymous ent;
            ent->setId(entityId);

            Disappearance d;
            d->setArgs1(ent);
            res.push_back(d);

        }

        m_lastVisibleEntities = std::move(newVisibleEntities);
    }
}

void InventoryDomain::processDisappearanceOfEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res)
{
}

float InventoryDomain::checkCollision(LocatedEntity& entity, CollisionData& collisionData)
{
    //Nothing can move
    return consts::move_tick;
}
