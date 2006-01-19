// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

#include "Movement.h"

#include "Character.h"

#include "physics/Collision.h"

#include "common/const.h"
#include "common/debug.h"

#include "common/Tick.h"

#include <Atlas/Objects/Operation.h>

static const bool debug_flag = false;

Movement::Movement(Entity & body) : m_body(body), m_lastMovementTime(-1),
                                    m_lastCollisionTime(-1),
                                    m_serialno(0),
                                    m_collEntity(NULL),
                                    m_collLocChange(false),
                                    m_diverted(false)
{
}

Movement::~Movement()
{
}

bool Movement::updateNeeded(const Location & location) const
{
    return (location.velocity().isValid() && location.velocity() != Vector3D(0,0,0));
}


float Movement::checkCollisions(const Location & location)
{
    // Check to see whether a collision is going to occur from now until the
    // the next tick in consts::basic_tick seconds
    float collTime = consts::basic_tick;
    debug( std::cout << "checking " << m_body.getId() << location.pos()
                     << location.velocity() << " in " << location.m_loc->getId()
                     << " against"; );
    m_collEntity = NULL;
    m_collLocChange = false;
    m_collPos.setValid(false);
    // Check against everything within the current container
    EntitySet::const_iterator I = location.m_loc->m_contains.begin();
    EntitySet::const_iterator Iend = location.m_loc->m_contains.end();
    for (; I != Iend; ++I) {
        // Don't check for collisions with ourselves
        if ((*I) == &m_body) { continue; }
        const Location & other_location = (*I)->m_location;
        if (!other_location.bBox().isValid() || !other_location.isSolid()) {
            continue;
        }
        debug( std::cout << " " << (*I)->getId(); );
        Vector3D normal;
        float t = consts::basic_tick + 1;
        if (!predictCollision(location, other_location, t, normal) || (t < 0)) {
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
        const Location & other_location = location.m_loc->m_location;
        if (!other_location.bBox().isValid() || (other_location.m_loc == 0)) {
            return consts::basic_tick;
        }
        // float t = location.timeToExit(other_location);
        float t = 0;
        predictEmergence(location, other_location, t);
        // if (t == 0) { return; }
        // if (t < 0) { t = 0; }
        if (t > consts::basic_tick) { return t; }
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
        Location rloc(location);
        rloc.m_loc = m_collEntity;
        if (lc2.orientation().isValid()) {
            rloc.m_pos = location.m_pos.toLocalCoords(lc2.pos(), lc2.orientation());
        } else {
            static const Quaternion identity(1, 0, 0, 0);
            rloc.m_pos = location.m_pos.toLocalCoords(lc2.pos(), identity);
        }
        float coll2Time = consts::basic_tick;
        // rloc is now location of character with loc set to m_collEntity
        I = m_collEntity->m_contains.begin();
        Iend = m_collEntity->m_contains.end();
        for (; I != Iend; ++I) {
            const Location & other_location = (*I)->m_location;
            if (!other_location.bBox().isValid()) { continue; }
            Vector3D normal;
            float t = consts::basic_tick + 1;
            if (!predictCollision(rloc, other_location, t, normal) || (t < 0)) {
                continue;
            }
            if (t <= coll2Time) {
                coll2Time = t;
            }
            // What to do with the normal?
        }
        // There is a small possibility that if
        // coll2Time == collTime == basic_tick, we will miss a collision
        if ((coll2Time - collTime) > (consts::basic_tick / 10)) {
            debug( std::cout << "passing into it " << collTime << ":"
                             << coll2Time << std::endl << std::flush;);
            // We are entering collEntity.
            m_collLocChange = true;
        }
    }
    assert(m_collEntity != NULL);
    debug( std::cout << "COLLISION" << std::endl << std::flush; );
    if (collTime < getTickAddition(location.pos(), location.velocity())) {
        debug( std::cout << "Setting target loc to " << location.pos() << "+"
                         << location.velocity() << "*" << collTime;);
        m_collPos = location.pos();
        m_collPos += (location.velocity() * collTime);
    } else {
        m_collEntity = NULL;
        m_collLocChange = false;
    }
    return collTime;
}

void Movement::reset()
{
    ++m_serialno;
    m_collEntity = NULL;
    m_collLocChange = false;
    m_diverted = false;
    m_collPos.setValid(false);
    m_targetPos.setValid(false);
    m_updatedPos.setValid(false);
    m_lastMovementTime = m_body.m_world->getTime();
}
