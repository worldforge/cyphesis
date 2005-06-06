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

static const bool debug_flag = false;

Movement::Movement(Entity & body) : m_body(body), m_lastMovementTime(-1),
                                    m_velocity(0,0,0), m_serialno(0),
                                    m_collEntity(NULL), m_collLocChange(false),
                                    m_collAxis(-1)
{
    // FIXME I think setting velocity to (0,0,0) is a weird way to do it.
}

Movement::~Movement()
{
}

bool Movement::updateNeeded(const Location & location) const
{
    return((m_velocity.isValid() && m_velocity != Vector3D(0,0,0)) ||
           (location.m_velocity.isValid() && location.m_velocity != Vector3D(0,0,0)));
}


void Movement::checkCollisions(const Location & location)
{
    // Check to see whether a collision is going to occur from now until the
    // the next tick in consts::basic_tick seconds
    float collTime = consts::basic_tick;
    debug( std::cout << "checking " << m_body.getId() << location.m_pos
                     << location.m_velocity << " in " << location.m_loc->getId()
                     << " against"; );
    m_collEntity = NULL;
    // Check against everything within the current container
    EntitySet::const_iterator I = location.m_loc->m_contains.begin();
    EntitySet::const_iterator Iend = location.m_loc->m_contains.end();
    for (; I != Iend; ++I) {
        // Don't check for collisions with ourselves
        if ((*I) == &m_body) { continue; }
        const Location & other_location = (*I)->m_location;
        if (!other_location.m_bBox.isValid() || !other_location.isSolid()) {
            continue;
        }
        debug( std::cout << " " << (*I)->getId(); );
        Vector3D normal;
        float t = consts::basic_tick + 1;
        if (!predictCollision(location, other_location, t, normal) || (t < 0)) {
            continue;
        }
        debug( std::cout << (*I)->getId() << other_location.m_pos << other_location.m_velocity; );
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
        if (!other_location.m_bBox.isValid() || (other_location.m_loc == 0)) {
            return;
        }
        // float t = location.timeToExit(other_location);
        float t = 0;
        predictEmergence(location, other_location, t);
        // if (t == 0) { return; }
        // if (t < 0) { t = 0; }
        if (t > consts::basic_tick) { return; }
        collTime = t;
        debug(std::cout << "Collision with parent bounding box in "
                        << collTime << std::endl << std::flush;);
        m_collEntity = other_location.m_loc;
        m_collLocChange = true;
    } else if (!m_collEntity->m_location.isSimple()) {
        debug(std::cout << "Collision with non-solid object" << std::endl
                        << std::flush;);
        // Non solid container - check for collision with its contents.
        const Location & lc2 = m_collEntity->m_location;
        Location rloc(location);
        rloc.m_loc = m_collEntity;
        if (lc2.m_orientation.isValid()) {
            rloc.m_pos = location.m_pos.toLocalCoords(lc2.m_pos, lc2.m_orientation);
        } else {
            static const Quaternion identity(1, 0, 0, 0);
            rloc.m_pos = location.m_pos.toLocalCoords(lc2.m_pos, identity);
        }
        float coll2Time = consts::basic_tick;
        // rloc is now location of character with loc set to m_collEntity
        I = m_collEntity->m_contains.begin();
        Iend = m_collEntity->m_contains.end();
        for (; I != Iend; ++I) {
            const Location & other_location = (*I)->m_location;
            if (!other_location.m_bBox.isValid()) { continue; }
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
    debug( std::cout << "COLLISION" << std::endl << std::flush; );
    if (collTime < getTickAddition(location.m_pos)) {
        debug( std::cout << "Setting target loc to " << location.m_pos << "+"
                         << location.m_velocity << "*" << collTime;);
        m_collPos = location.m_pos;
        m_collPos += (location.m_velocity * collTime);
    } else {
        m_collEntity = NULL;
        m_collLocChange = false;
    }
}

void Movement::reset()
{
    ++m_serialno;
    m_collEntity = NULL;
    m_collLocChange = false;
    m_collPos = Point3D();
    m_collAxis = -1;
    m_targetPos = Point3D();
    m_updatedPos = Point3D();
    m_velocity = Vector3D(0,0,0);
    m_lastMovementTime = m_body.m_world->getTime();
}

bool Movement::moving() const
{
    return (m_velocity.isValid() && m_velocity != Vector3D(0,0,0));
}

