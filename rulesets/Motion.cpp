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

#include "Domain.h"

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

float Motion::checkCollisions(Domain& domain)
{
    Domain::CollisionData collData;
    float collision_time = domain.checkCollision(m_entity, collData);
    m_collEntity = collData.collEntity;
    m_collision = collData.isCollision;
    m_collNormal = collData.collNormal;
    return collision_time;

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
