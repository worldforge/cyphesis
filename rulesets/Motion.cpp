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

#include "rulesets/Entity.h"

#include "physics/Vector3D.h"
#include "physics/Collision.h"

#include "common/compose.hpp"
#include "common/debug.h"
#include "common/const.h"
#include "common/log.h"

#include <iostream>

static const bool debug_flag = true;

Motion::Motion(Entity & body) : m_entity(body)
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

void Motion::adjustPostion()
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
    // Check to see whether a collision is going to occur from now until the
    // the next tick in consts::move_tick seconds
    float collTime = consts::move_tick;
    debug( std::cout << "checking " << m_entity.getId()
                     << m_entity.m_location.pos()
                     << m_entity.m_location.velocity() << " in "
                     << m_entity.m_location.m_loc->getId()
                     << " against"; );
    m_collEntity = NULL;
    m_collLocChange = false;
    m_collision = false;
    // Check against everything within the current container
    EntitySet::const_iterator I = m_entity.m_location.m_loc->m_contains.begin();
    EntitySet::const_iterator Iend = m_entity.m_location.m_loc->m_contains.end();
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
        if (t <= collTime) {
            m_collEntity = *I;
            m_collNormal = normal;
            collTime = t;
        }
    }
    debug( std::cout << std::endl << std::flush; );
    if (m_collEntity == NULL) {
        // Check whethe we are moving out of parents bounding box
        // If ref has no bounding box, or itself has no ref, then we can't
        // Move out of it.
        const Location & parent_location = m_entity.m_location.m_loc->m_location;
        if (!parent_location.bBox().isValid() || (parent_location.m_loc == 0)) {
            return consts::move_tick;
        }
        // float t = m_entity.m_location.timeToExit(parent_location);
        float t = 0;
        predictEmergence(m_entity.m_location, parent_location, t);
        // if (t == 0) { return; }
        // if (t < 0) { t = 0; }
        if (t > consts::move_tick) { return t; }
        collTime = t;
        debug(std::cout << "Collision with parent bounding box in "
                        << collTime << std::endl << std::flush;);
        m_collEntity = m_entity.m_location.m_loc;
        m_collLocChange = true;
    } else if (!m_collEntity->m_location.isSimple()) {
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
        float coll2Time = consts::move_tick;
        // rloc is now m_entity.m_location of character with loc set to m_collEntity
        I = m_collEntity->m_contains.begin();
        Iend = m_collEntity->m_contains.end();
        for (; I != Iend; ++I) {
            const Location & other_location = (*I)->m_location;
            if (!other_location.bBox().isValid()) { continue; }
            Vector3D normal;
            float t = consts::move_tick + 1;
            if (!predictCollision(rloc, other_location, t, normal) || (t < 0)) {
                continue;
            }
            if (t <= coll2Time) {
                coll2Time = t;
            }
            // What to do with the normal?
        }
        // There is a small possibility that if
        // coll2Time == collTime == move_tick, we will miss a collision
        if ((coll2Time - collTime) > (consts::move_tick / 10)) {
            debug( std::cout << "passing into it " << collTime << ":"
                             << coll2Time << std::endl << std::flush;);
            // We are entering collEntity.
            m_collLocChange = true;
        }
    }
    assert(m_collEntity != NULL);
    m_collision = true;
    debug( std::cout << "COLLISION" << std::endl << std::flush; );
    debug( std::cout << "Setting target loc to "
                     << m_entity.m_location.pos() << "+"
                     << m_entity.m_location.velocity() << "*" << collTime;);
    return collTime;
}

bool Motion::resolveCollision()
{
    Location & location(m_entity.m_location);
    bool moving = true;

    if (m_collLocChange) {
        // We are changing container (LOC)
        static const Quaternion identity(Quaternion().identity());
        debug(std::cout << "CONTACT " << m_collEntity->getId()
                        << std::endl << std::flush;);
        if (m_collEntity == location.m_loc) {
            // Passing out of current container
            debug(std::cout << "OUT"
                            << m_collEntity->m_location.pos()
                            << std::endl << std::flush;);
            const Quaternion & collOrientation = m_collEntity->m_location.orientation().isValid() ?
                                                 m_collEntity->m_location.orientation() :
                                                 identity;
            location.m_pos = location.m_pos.toParentCoords(m_collEntity->m_location.pos(), collOrientation);
            location.m_orientation *= collOrientation;
            location.m_velocity.rotate(collOrientation);
            location.m_loc = m_collEntity->m_location.m_loc;
        } else if (m_collEntity->m_location.m_loc == location.m_loc) {
            // Passing into new container
            debug(std::cout << "IN" << std::endl << std::flush;);
            const Quaternion & collOrientation = m_collEntity->m_location.orientation().isValid() ?
                                                 m_collEntity->m_location.orientation() :
                                                 identity;
            location.m_pos = location.m_pos.toLocalCoords(m_collEntity->m_location.pos(), collOrientation);
            location.m_orientation /= collOrientation;
            location.m_velocity.rotate(collOrientation.inverse());
            location.m_loc = m_collEntity;
        } else {
            // Container we are supposed to changing to is wrong.
            // Just stop where we currently are. Debugging is required to work out
            // why this happens
            log(ERROR, String::compose("BAD COLLISION: %1(%2) with %3(%4)%5 when LOC is currently %6(%7)%8.",
                                       m_entity.getId(),
                                       m_entity.getType(),
                                       m_collEntity->getId(),
                                       m_collEntity->getType(),
                                       location.m_pos,
                                       location.m_loc->getId(),
                                       location.m_loc->getType(),
                                       location.m_pos).c_str());
            // reset();
            location.m_velocity = Vector3D(0,0,0);
            moving = false;
        }
        m_collEntity = NULL;
        m_collLocChange = false;
    } else {
        // We have arrived at our target position and must
        // stop, or be deflected
        if (location.m_loc != m_collEntity->m_location.m_loc) {
            // Race condition
            // This occurs if we get asked for a new update before
            // the last move has taken effect, so we make the new
            // pos exactly as it was when the last collision was
            // predicted.
            log(ERROR, "NON COLLISION");
        } else {
            // FIXME Generate touch ops
            // This code relies on m_collNormal being a unit vector
            float vel_square_mag = location.velocity().sqrMag();
            location.m_velocity -= m_collNormal * Dot(m_collNormal, location.m_velocity);
            if ((location.m_velocity.mag() / consts::base_velocity) > 0.05) {
                m_collEntity = NULL;
                location.m_velocity.normalize();
                location.m_velocity *= sqrt(vel_square_mag);
            } else {
                // reset();
                location.m_velocity = Vector3D(0,0,0);
                moving = false;
            }
        }
    }
    return moving;
}
