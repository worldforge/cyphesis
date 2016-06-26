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

#include "PhysicalDomain.h"

#include "TerrainProperty.h"
#include "LocatedEntity.h"
#include "OutfitProperty.h"
#include "EntityProperty.h"

#include "physics/Collision.h"

#include "common/debug.h"
#include "common/const.h"
#include "common/Unseen.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>


#include <iostream>
#include <unordered_set>

#include <cassert>

static const bool debug_flag = false;

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

PhysicalDomain::PhysicalDomain(LocatedEntity& entity)
: Domain(entity)
{
}

PhysicalDomain::~PhysicalDomain()
{
}

float PhysicalDomain::constrainHeight(LocatedEntity * parent,
                              const Point3D & pos,
                              const std::string & mode)
{
    assert(parent != 0);
    if (mode == "fixed") {
        return pos.z();
    }
    const TerrainProperty * tp = parent->getPropertyClass<TerrainProperty>("terrain");
    if (tp != 0) {
        if (mode == "floating") {
            return 0.f;
        }
        float h = pos.z();
        Vector3D normal;
        tp->getHeightAndNormal(pos.x(), pos.y(), h, normal);
        // FIXME Use a virtual movement_domain function to get the constraints
        debug(std::cout << "Fix height " << pos.z() << " to " << h
                        << std::endl << std::flush;);
        return h;
    } else if (parent->m_location.m_loc != 0) {
        static const Quaternion identity(Quaternion().identity());
        const Point3D & ppos = parent->m_location.pos();
        debug(std::cout << "parent " << parent->getId() << " of type "
                        << parent->getType() << " pos " << ppos.z()
                        << " my pos " << pos.z()
                        << std::endl << std::flush;);
        float h;
        const Quaternion & parent_orientation = parent->m_location.orientation().isValid() ? parent->m_location.orientation() : identity;
        h =  constrainHeight(parent->m_location.m_loc,
                             pos.toParentCoords(parent->m_location.pos(),
                                                parent_orientation),
                             mode) - ppos.z();
        debug(std::cout << "Correcting height from " << pos.z() << " to " << h
                        << std::endl << std::flush;);
        return h;
    }
    return pos.z();
}

void PhysicalDomain::tick(double t)
{

}

bool PhysicalDomain::isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const
{
    if (&observedEntity == &m_entity) {
        return true;
    }

    //We need to check the distance to the entity being looked at, and make sure that both the looking entity and
    //the entity being looked at belong to the same domain
    const Location* ancestor;
    //We'll optimize for the case when a child entity is looking at the domain entity, by sending the looked at entity first, since this is the most common case.
    //The squareDistanceWithAncestor() method will first try to find the first entity being sent by walking upwards from the second entity being sent (if
    //it fails it will try other approaches).
    float distance = squareDistanceWithAncestor(observedEntity.m_location, observingEntity.m_location, &ancestor);
    if (ancestor == nullptr) {
        //No common ancestor found
        return false;
    } else {
        //Make sure that the ancestor is the domain entity, or a child entity.
        while (ancestor != &m_entity.m_location) {
            if (ancestor->m_loc == nullptr) {
                //We've reached the top of the parents chain without hitting our domain entity; the ancestor isn't a child of the domain entity.
                return false;
            }
            ancestor = &ancestor->m_loc->m_location;
        }
    }
    //If we get here we know that the ancestor is a child of the domain entity.
    //Now we need to determine if the looking entity can see the looked at entity. The default way of doing this is by comparing the size of the looked at entity with the distance,
    //but this check can be overridden if the looked at entity is either wielded or outfitted by a parent entity.
    if ((observedEntity.m_location.squareBoxSize() / distance) > consts::square_sight_factor) {
        return true;
    }

    return false;
}

void PhysicalDomain::calculateVisibility(std::vector<Root>& appear, std::vector<Root>& disappear, Anonymous& this_ent, const LocatedEntity& parent,
        const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res) const {

    float fromSquSize = moved_entity.m_location.squareBoxSize();

    //We need to get the position of the moved entity in relation to the parent.
    const Point3D new_pos = relativePos(parent.m_location, moved_entity.m_location);
    const Point3D old_pos = relativePos(parent.m_location, old_loc);

    //For now we'll only consider movement within the same loc. This should change as we extend the domain code.
    assert(parent.m_contains != nullptr);
    for (const LocatedEntity* other: *parent.m_contains) {
        if (other == &moved_entity) {
            continue;
        }

        assert(other != nullptr);
        float old_dist = squareDistance(other->m_location.pos(), old_pos),
              new_dist = squareDistance(other->m_location.pos(), new_pos),
              squ_size = other->m_location.squareBoxSize();

        // Build appear and disappear lists, and send disappear operations
        // to perceptive entities saying that we are disappearing
        if (other->isPerceptive()) {
            bool was_in_range = ((fromSquSize / old_dist) > consts::square_sight_factor),
                 is_in_range = ((fromSquSize / new_dist) > consts::square_sight_factor);
            if (was_in_range != is_in_range) {
                if (was_in_range) {
                    // Send operation to the entity in question so it
                    // knows it is losing sight of us.
                    Disappearance d;
                    d->setArgs1(this_ent);
                    d->setTo(other->getId());
                    res.push_back(d);
                }
                //Note that we don't send any Appear ops for those entities that we now move within sight range of.
                //This is because these will receive a Move op anyway as part of the broadcast, which informs them
                //that an entity has moved within sight range anyway.
            }
        }

        bool could_see = ((squ_size / old_dist) > consts::square_sight_factor),
             can_see = ((squ_size / new_dist) > consts::square_sight_factor);
        if (could_see ^ can_see) {
            Anonymous that_ent;
            that_ent->setId(other->getId());
            that_ent->setStamp(other->getSeq());
            if (could_see) {
                // We are losing sight of that object
                disappear.push_back(that_ent);
                debug(std::cout << moved_entity.getId() << ": losing sight of "
                                << other->getId() << std::endl;);
            } else /*if (can_see)*/ {
                // We are gaining sight of that object
                appear.push_back(that_ent);
                debug(std::cout << moved_entity.getId() << ": gaining sight of "
                                << other->getId() << std::endl;);
            }
//        } else {
//            //We've seen this entity before, and we're still seeing it. Check if there are any children that's now changing visibility.
//            if (other->m_contains && !other->m_contains->empty()) {
//                calculateVisibility(appear, disappear, this_ent, *other, moved_entity, old_loc, res);
//            }
        }
    }
}


void PhysicalDomain::processVisibilityForMovedEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res) {
    debug_print("testing range for " << moved_entity.describeEntity());
    std::vector<Root> appear, disappear;

    Anonymous this_ent;
    this_ent->setId(moved_entity.getId());
    this_ent->setStamp(moved_entity.getSeq());

    calculateVisibility(appear, disappear, this_ent, m_entity, moved_entity, old_loc, res);

    if (!appear.empty()) {
        // Send an operation to ourselves with a list of entities
        // we are gaining sight of
        Appearance a;
        a->setArgs(appear);
        a->setTo(moved_entity.getId());
        res.push_back(a);
    }
    if (!disappear.empty()) {
        // Send an operation to ourselves with a list of entities
        // we are losing sight of
        Disappearance d;
        d->setArgs(disappear);
        d->setTo(moved_entity.getId());
        res.push_back(d);
    }
}

void PhysicalDomain::processDisappearanceOfEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res) {

    float fromSquSize = old_loc.squareBoxSize();
    Anonymous this_ent;
    this_ent->setId(moved_entity.getId());
    this_ent->setStamp(moved_entity.getSeq());

    //We need to get the position of the moved entity in relation to the parent.
    const Point3D old_pos = relativePos(m_entity.m_location, old_loc);

    assert(m_entity.m_contains != nullptr);
    for (const LocatedEntity* other: *m_entity.m_contains) {
        //No need to check if we iterate over ourselved; that won't happen if we've disappeared

        assert(other != nullptr);

        // Build appear and disappear lists, and send disappear operations
        // to perceptive entities saying that we are disappearing
        if (other->isPerceptive()) {
            float old_dist = squareDistance(other->m_location.pos(), old_pos);
            if ((fromSquSize / old_dist) > consts::square_sight_factor) {
                // Send operation to the entity in question so it
                // knows it is losing sight of us.
                Disappearance d;
                d->setArgs1(this_ent);
                d->setTo(other->getId());
                res.push_back(d);
            }
        }
    }
}


float PhysicalDomain::checkCollision(LocatedEntity& entity, CollisionData& collisionData)
{
    assert(entity.m_location.m_loc != 0);
    assert(entity.m_location.m_loc->m_contains != 0);
    assert(entity.m_location.m_pos.isValid());
    assert(entity.m_location.m_velocity.isValid());
    // Check to see whether a collision is going to occur from now until the
    // the next tick in consts::move_tick seconds
    float coll_time = consts::move_tick;
    debug_print( "checking " << entity.getId()
                     << entity.m_location.pos()
                     << entity.m_location.velocity() << " in "
                     << entity.m_location.m_loc->getId()
                     << " against");
    collisionData.collEntity = nullptr;
    collisionData.isCollision = false;
    // Check against everything within the current container
    // If this entity doesn't have a bbox, it can't collide currently.
    if (!entity.m_location.bBox().isValid()) {
        return coll_time;
    }
    for (LocatedEntity* other_entity : *entity.m_location.m_loc->m_contains) {
        // Don't check for collisions with ourselves
        if (&entity == other_entity) {
            continue;
        }
        const Location & other_location = other_entity->m_location;
        if (!other_location.bBox().isValid() || !other_location.isSolid()) {
            continue;
        }
        debug( std::cout << " " << other_entity->getId(); );
        Vector3D normal;
        float t = consts::move_tick + 1;
        if (!predictCollision(entity.m_location, other_location, t, normal) || (t < 0)) {
            continue;
        }
        debug( std::cout << other_entity->getId() << other_location.pos() << other_location.velocity(); );
        debug( std::cout << "[" << t << "]"; );
        if (t <= coll_time) {
            collisionData.collEntity = other_entity;
            collisionData.collNormal = normal;
            coll_time = t;
        }
    }
    if (collisionData.collEntity == nullptr) {
        return consts::move_tick;
    }
    debug( std::cout << std::endl << std::flush; );
    collisionData.isCollision = true;
    if (!collisionData.collEntity->m_location.isSimple()) {
        debug(std::cout << "Collision with complex object" << std::endl
                        << std::flush;);
        // Non solid container - check for collision with its contents.
        const Location & lc2 = collisionData.collEntity->m_location;
        Location rloc(entity.m_location);
        rloc.m_loc = collisionData.collEntity;
        if (lc2.orientation().isValid()) {
            rloc.m_pos = entity.m_location.m_pos.toLocalCoords(lc2.pos(), lc2.orientation());
        } else {
            static const Quaternion identity(1, 0, 0, 0);
            rloc.m_pos = entity.m_location.m_pos.toLocalCoords(lc2.pos(), identity);
        }
        float coll_time_2 = consts::move_tick;
        // rloc is now m_entity.m_location of character with loc set to m_collEntity
        if (collisionData.collEntity->m_contains != nullptr) {
            for (const LocatedEntity* other_entity : *collisionData.collEntity->m_contains) {
                const Location & other_location = other_entity->m_location;
                if (!other_location.bBox().isValid()) {
                    continue;
                }
                Vector3D normal;
                float t = consts::move_tick + 1;
                if (!predictCollision(rloc, other_location, t, normal) ||
                    t < 0) {
                    continue;
                }
                if (t <= coll_time_2) {
                    coll_time_2 = t;
                }
                // What to do with the normal?
            }
        }
        // There is a small possibility that if
        // coll_time_2 == coll_time == move_tick, we will miss a collision
        if (coll_time_2 - coll_time > consts::move_tick / 10) {
            debug( std::cout << "passing into it " << coll_time << ":"
                             << coll_time_2 << std::endl << std::flush;);
            // We are entering collEntity.
            // Once we have entered, subsequent collision detection won't
            // really work.
            // FIXME Modifiy the predicted collision time.
        }
    }
    assert(collisionData.collEntity != nullptr);
    debug( std::cout << "COLLISION" << std::endl << std::flush; );
    debug( std::cout << "Setting target loc to "
                     << entity.m_location.pos() << "+"
                     << entity.m_location.velocity() << "*" << coll_time;);
    return coll_time;
}
