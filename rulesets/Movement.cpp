// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Movement.h"

#include "Character.h"

#include "physics/Collision.h"

#include "common/const.h"
#include "common/debug.h"

#include "common/Tick.h"

#include <Atlas/Objects/Operation/Move.h>

static const bool debug_flag = true;

Movement::Movement(Entity & body) : m_body(body), m_lastMovementTime(-1),
                                    m_velocity(0,0,0), m_serialno(0),
                                    m_collEntity(NULL), m_collRefChange(false),
                                    m_collAxis(-1)
{
    // FIXME I think setting velocity to (0,0,0) is a weird way to do it.
}

Movement::~Movement()
{
}

bool Movement::updateNeeded(const Location & loc) const
{
    return((m_velocity.isValid() && m_velocity != Vector3D(0,0,0)) ||
           (loc.m_velocity.isValid() && loc.m_velocity != Vector3D(0,0,0)));
}


void Movement::checkCollisions(const Location & loc)
{
    // Check to see whether a collision is going to occur from now until the
    // the next tick in consts::basic_tick seconds
    float collTime = consts::basic_tick;
    debug( std::cout << "checking " << m_body.getId() << loc.m_pos
                     << loc.m_velocity << " in " << loc.m_loc->getId()
                     << " against"; );
    m_collEntity = NULL;
    // Check against everything within the current container
    EntitySet::const_iterator I = loc.m_loc->m_contains.begin();
    for(; I != loc.m_loc->m_contains.end(); I++) {
        // Don't check for collisions with ourselves
        if ((*I) == &m_body) { continue; }
        const Location & oloc = (*I)->m_location;
        if (!oloc.m_bBox.isValid()) { continue; }
        debug( std::cout << " " << (*I)->getId(); );
        Vector3D normal;
        float t = 4; // FIXME relate to tick time
        if (!predictCollision(loc, oloc, t, normal) || (t < 0)) { continue; }
        debug( std::cout << (*I)->getId() << oloc.m_pos << oloc.m_velocity; );
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
        const Location & oloc = loc.m_loc->m_location;
        if (!oloc.m_bBox.isValid() || (oloc.m_loc == NULL)) {
            return;
        }
        float t = loc.timeToExit(oloc);
        if (t == 0) { return; }
        if (t < 0) { t = 0; }
        if (t > consts::basic_tick) { return; }
        collTime = t;
        debug(std::cout << "Collision with parent bounding box in "
                        << collTime << std::endl << std::flush;);
        m_collEntity = oloc.m_loc;
        m_collRefChange = true;
    } else if (!m_collEntity->m_location.m_solid) {
        debug(std::cout << "Collision with non-solid object" << std::endl
                        << std::flush;);
        // Non solid container - check for collision with its contents.
        const Location & lc2 = m_collEntity->m_location;
        Location rloc(loc);
        rloc.m_loc = m_collEntity;
        rloc.m_pos = loc.m_pos - lc2.m_pos;
        float coll2Time = consts::basic_tick;
        // rloc is coords of character with ref to m_collEntity
        I = m_collEntity->m_contains.begin();
        for(; I != m_collEntity->m_contains.end(); I++) {
            const Location & oloc = (*I)->m_location;
            if (!oloc.m_bBox.isValid()) { continue; }
            Vector3D normal;
            float t = 4; // FIXME relate to tick time
            if (!predictCollision(rloc,oloc,t,normal) || (t < 0)) { continue; }
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
            m_collRefChange = true;
        }
    }
    debug( std::cout << "COLLISION" << std::endl << std::flush; );
    if (collTime < getTickAddition(loc.m_pos)) {
        debug( std::cout << "Setting target loc to " << loc.m_pos << "+"
                         << loc.m_velocity << "*" << collTime;);
        m_collPos = loc.m_pos;
        m_collPos += (loc.m_velocity * collTime);
    } else {
        m_collEntity = NULL;
        m_collRefChange = false;
    }
}

void Movement::reset()
{
    ++m_serialno;
    m_collEntity = NULL;
    m_collRefChange = false;
    m_collPos = Vector3D();
    m_collAxis = -1;
    m_targetPos = Vector3D();
    m_updatedPos = Vector3D();
    m_velocity = Vector3D(0,0,0);
    m_lastMovementTime = m_body.m_world->getTime();
}

bool Movement::moving() const
{
    return (m_velocity.isValid() && m_velocity != Vector3D(0,0,0));
}

