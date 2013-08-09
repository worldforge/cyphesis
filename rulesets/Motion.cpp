// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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


#include "Motion.h"

#include "rulesets/LocatedEntity.h"

#include "physics/Collision.h"

#include "common/compose.hpp"
#include "common/debug.h"
#include "common/const.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <iostream>

using String::compose;

static const bool debug_flag = false;

Motion::Motion(LocatedEntity & body) : m_entity(body), m_serialno(0),
                                       m_collision(false), m_collEntity(0),
                                       m_collisionTime(0.f)
{
}

Motion::~Motion()
{
}

void Motion::setMode(const std::string & mode)
{
    m_mode = mode;
    // FIXME Re-configure stuff, and possible schedule an update?
}

void Motion::adjustPosition()
{
}

Operation * Motion::genUpdateOperation()
{
    return 0;
}

Operation * Motion::genMoveOperation()
{
    return 0;
}

float Motion::checkCollisions()
{
    assert(m_entity.m_location.m_loc != 0);
    assert(m_entity.m_location.m_loc->m_contains != 0);
    assert(m_entity.m_location.m_pos.isValid());
    assert(m_entity.m_location.m_velocity.isValid());
    // Check to see whether a collision is going to occur from now until the
    // the next tick in consts::move_tick seconds
    float coll_time = consts::move_tick;
    debug( std::cout << "checking " << m_entity.getId()
                     << m_entity.m_location.pos()
                     << m_entity.m_location.velocity() << " in "
                     << m_entity.m_location.m_loc->getId()
                     << " against"; );
    m_collEntity = NULL;
    m_collision = false;
    // Check against everything within the current container
    // If this entity doesn't have a bbox, it can't collide currently.
    if (!m_entity.m_location.bBox().isValid()) {
        return coll_time;
    }
    LocatedEntitySet::const_iterator I = m_entity.m_location.m_loc->m_contains->begin();
    LocatedEntitySet::const_iterator Iend = m_entity.m_location.m_loc->m_contains->end();
    for (; I != Iend; ++I) {
        // Don't check for collisions with ourselves
        if ((*I) == &m_entity) { continue; }
        const Location & other_location = (*I)->m_location;
        if (!other_location.bBox().isValid() || !other_location.isSolid()) {
            continue;
        }
        debug( std::cout << " " << (*I)->getId(); );
        Vector3D normal;
        float t = consts::move_tick + 1;
        if (!predictCollision(m_entity.m_location, other_location, t, normal) || (t < 0)) {
            continue;
        }
        debug( std::cout << (*I)->getId() << other_location.pos() << other_location.velocity(); );
        debug( std::cout << "[" << t << "]"; );
        if (t <= coll_time) {
            m_collEntity = *I;
            m_collNormal = normal;
            coll_time = t;
        }
    }
    if (m_collEntity == NULL) {
        return consts::move_tick;
    }
    debug( std::cout << std::endl << std::flush; );
    m_collision = true;
    if (!m_collEntity->m_location.isSimple()) {
        debug(std::cout << "Collision with complex object" << std::endl
                        << std::flush;);
        // Non solid container - check for collision with its contents.
        const Location & lc2 = m_collEntity->m_location;
        Location rloc(m_entity.m_location);
        rloc.m_loc = m_collEntity;
        if (lc2.orientation().isValid()) {
            rloc.m_pos = m_entity.m_location.m_pos.toLocalCoords(lc2.pos(), lc2.orientation());
        } else {
            static const Quaternion identity(1, 0, 0, 0);
            rloc.m_pos = m_entity.m_location.m_pos.toLocalCoords(lc2.pos(), identity);
        }
        float coll_time_2 = consts::move_tick;
        // rloc is now m_entity.m_location of character with loc set to m_collEntity
        if (m_collEntity->m_contains != 0) {
            I = m_collEntity->m_contains->begin();
            Iend = m_collEntity->m_contains->end();
            for (; I != Iend; ++I) {
                const Location & other_location = (*I)->m_location;
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
    assert(m_collEntity != NULL);
    debug( std::cout << "COLLISION" << std::endl << std::flush; );
    debug( std::cout << "Setting target loc to "
                     << m_entity.m_location.pos() << "+"
                     << m_entity.m_location.velocity() << "*" << coll_time;);
    return coll_time;
}

bool Motion::resolveCollision()
{
    Location & location(m_entity.m_location);
    bool moving = true;

    assert(m_collision);
    assert(m_collEntity != 0);
    assert(m_collEntity->m_location.m_loc != 0);

    // We have arrived at our target position and must
    // stop, or be deflected

    // FIXME This should become an assertion.
    if (location.m_loc != m_collEntity->m_location.m_loc) {
        // Race condition
        // This occurs if we get asked for a new update before
        // the last move has taken effect, so we make the new
        // pos exactly as it was when the last collision was
        // predicted.
        log(ERROR, compose("Collision Error: %1(%2) with parent %3 "
                           "colliding with %4(%5) with parent %6",
                           m_entity.getId(), m_entity.getType()->name(),
                           location.m_loc->getId(), m_collEntity->getId(),
                           m_collEntity->getType()->name(),
                           m_collEntity->m_location.m_loc->getId()));
    } else {
        // FIXME Generate touch ops
        // This code relies on m_collNormal being a unit vector
        float vel_square_mag = location.velocity().sqrMag();
        location.m_velocity -= m_collNormal * Dot(m_collNormal, location.m_velocity);
        if (location.m_velocity.mag() / consts::base_velocity > 0.05) {
            m_collEntity = NULL;
            location.m_velocity.normalize();
            location.m_velocity *= std::sqrt(vel_square_mag);
        } else {
            // reset();
            location.m_velocity = Vector3D(0,0,0);
            moving = false;
        }
    }

    clearCollision();
    return moving;
}
