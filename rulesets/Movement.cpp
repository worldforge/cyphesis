// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Move.h>

#include <common/Tick.h>

#include "Character.h"
#include "Movement.h"

#include <common/const.h>
#include <common/debug.h>

using Atlas::Message::Object;

static const bool debug_flag = false;

Movement::Movement(Thing & body) : m_body(body), m_lastMovementTime(-1),
                                   m_velocity(0,0,0), m_serialno(0),
                                   m_collEntity(NULL), m_collRefChange(false),
                                   m_collAxis(-1)
{
}

Movement::~Movement()
{
}

bool Movement::updateNeeded(const Location & location) const
{
    return((m_velocity!=Vector3D(0,0,0))||(location.velocity!=Vector3D(0,0,0)));
}

void Movement::checkCollisions(const Location & loc)
{
    // Check to see whether a collision is going to occur from now until the
    // the next tick in consts::basic_tick seconds
    double collTime = consts::basic_tick + 1;
    elist_t::const_iterator I;
    debug( std::cout << "checking " << m_body.getId() << loc.coords << loc.velocity << " against "; );
    m_collEntity = NULL;
    for(I = loc.ref->contains.begin(); I != loc.ref->contains.end(); I++) {
        // if ((*I) == loc.ref) { continue; }
        if ((*I) == &m_body) { continue; }
        const Location & oloc = (*I)->location;
        if (!oloc.bBox) { continue; }
        int axis;
        double t = loc.timeToHit(oloc, axis);
        if (t < 0) { continue; }
        debug( std::cout << (*I)->getId() << oloc.coords << oloc.velocity; );
        debug( std::cout << "[" << t << "]"; );
        if (t < collTime) {
            m_collEntity = *I;
            m_collAxis = axis;
        }
        collTime = std::min(collTime, t);
    }
    debug( std::cout << std::endl << std::flush; );
    if (collTime > consts::basic_tick) {
        // Check whethe we are moving out of parents bounding box
        // If ref has no bounding box, or itself has no ref, then we can't
        // Move out of it.
        const Location & oloc = loc.ref->location;
        if (!oloc.bBox || (oloc.ref == NULL)) {
            return;
        }
        double t = loc.timeToExit(oloc);
        if (t < 0) { t=0; }
        collTime = std::min(collTime, t);
        if (collTime > consts::basic_tick) { return; }
        debug(std::cout << "Collision with parent bounding box" << std::endl
                        << std::flush;);
        m_collEntity = oloc.ref;
        m_collRefChange = true;
    } else if (!m_collEntity->location.solid) {
        debug(std::cout << "Collision with non-solid object" << std::endl
                        << std::flush;);
        // Non solid container - check for collision with its contents.
        const Location & lc2 = m_collEntity->location;
        Location rloc(loc);
        rloc.ref = m_collEntity; rloc.coords = loc.coords - lc2.coords;
        double coll2Time = consts::basic_tick + 1;
        // rloc is coords of character with ref to m_collEntity
        for(I = m_collEntity->contains.begin(); I != m_collEntity->contains.end(); I++) {
            const Location & oloc = (*I)->location;
            if (!oloc.bBox) { continue; }
            int axis;
            double t = rloc.timeToHit(oloc, axis);
            if (t < 0) { continue; }
            coll2Time = std::min(coll2Time, t);
        }
        if (coll2Time > collTime) {
            debug(std::cout << "passing into it " << collTime << ":"
                       << coll2Time << std::endl << std::flush;);
            // We are entering collEntity.
            // Set collRef ????????????????
            m_collRefChange = true;
            // if (coll2Time > consts::basic_tick) { return; }
        }
    }
    debug( std::cout << "COLLISION" << std::endl << std::flush; );
    if (collTime < getTickAddition(loc.coords)) {
        debug(std::cout << "Setting target loc to " << loc.coords << "+"
                   << loc.velocity << "*" << collTime;);
        m_collPos = loc.coords + loc.velocity * collTime;
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
    m_targetPos=Vector3D();
    m_updatedPos=Vector3D();
    m_velocity=Vector3D(0,0,0);
    m_lastMovementTime=m_body.world->getTime();
}
