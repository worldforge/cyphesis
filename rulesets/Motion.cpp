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

#include <rulesets/Entity.h>

#include <physics/Vector3D.h>
#include <physics/Collision.h>

#include <common/debug.h>
#include <common/const.h>

#include <iostream>

static const bool debug_flag = false;

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
    Entity * m_collEntity;
    bool m_collLocChange;
    Vector3D m_collNormal;

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
        const Location & other_location = m_entity.m_location.m_loc->m_location;
        if (!other_location.bBox().isValid() || (other_location.m_loc == 0)) {
            return consts::move_tick;
        }
        // float t = m_entity.m_location.timeToExit(other_location);
        float t = 0;
        predictEmergence(m_entity.m_location, other_location, t);
        // if (t == 0) { return; }
        // if (t < 0) { t = 0; }
        if (t > consts::move_tick) { return t; }
        collTime = t;
        debug(std::cout << "Collision with parent bounding box in "
                        << collTime << std::endl << std::flush;);
        m_collEntity = other_location.m_loc;
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
    debug( std::cout << "COLLISION" << std::endl << std::flush; );
    debug( std::cout << "Setting target loc to "
                     << m_entity.m_location.pos() << "+"
                     << m_entity.m_location.velocity() << "*" << collTime;);
    return collTime;
}
