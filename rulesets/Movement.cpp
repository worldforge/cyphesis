// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Move.h>

#include <common/Tick.h>

#include <varconf/Config.h>

extern "C" {
    #include <stdlib.h>
}

#include "Character.h"
#include "Movement.h"

#include <modules/Location.h>

#include <common/const.h>
#include <common/debug.h>

using Atlas::Message::Object;

static const bool debug_flag = false;

Movement::Movement(Character & body) : body(body), targetRef(NULL)
{
    serialno=-1;
    reset();
    debug( cout << "MOVEMENTINFO: " << lastMovementTime <<  endl << flush;);
}

Movement::~Movement()
{
}

bool Movement::updateNeeded(const Location & location) const
{
    return((velocity!=Vector3D(0,0,0))||(location.velocity!=Vector3D(0,0,0)));
}

void Movement::checkCollisions(const Location & loc)
{
    // Check to see whether a collision is going to occur from now until the
    // the next tick in consts::basic_tick seconds
    double collTime = consts::basic_tick + 1;
    list_t::const_iterator I;
    // cout << "checking " << body->fullid << loc.coords << loc.velocity << " against ";
    BaseEntity * collEntity = NULL;
    for(I = loc.ref->contains.begin(); I != loc.ref->contains.end(); I++) {
        // if ((*I) == loc.ref) { continue; }
        if ((*I) == &body) { continue; }
        const Location & oloc = (*I)->location;
        if (!oloc.bbox) { continue; }
        double t = loc.hitTime(oloc);
        if (t < 0) { continue; }
        // cout << (*I)->fullid << oloc.coords << oloc.velocity;
        // cout << "[" << t << "]";
        if (t < collTime) {
            collEntity = *I;
        }
        collTime = min(collTime, t);
    }
    // cout << endl << flush;
    if (collTime > consts::basic_tick) {
        // Check whethe we are moving out of parents bounding box
        // If ref has no bounding box, or itself has no ref, then we can't
        // Move out of it.
        const Location & oloc = loc.ref->location;
        if (!oloc.bbox || (oloc.ref == NULL)) {
            return;
        }
        double t = loc.inTime(oloc);
        if (t < 0) { return; }
        collTime = min(collTime, t);
        if (collTime > consts::basic_tick) { return; }
        cout << "Collision with parent bounding box" << endl << flush;
        targetRef = oloc.ref;
    } else if (!collEntity->location.solid) {
        cout << "Collision with non-solid object" << endl << flush;
        // Non solid container - check for collision with its contents.
        const Location & lc2 = collEntity->location;
        Location rloc(loc);
        rloc.ref = collEntity; rloc.coords = loc.coords - lc2.coords;
        double coll2Time = consts::basic_tick + 1;
        // rloc is coords of character ref collEntity
        for(I = lc2.ref->contains.begin(); I != lc2.ref->contains.end(); I++) {
            const Location & oloc = (*I)->location;
            if (!oloc.bbox) { continue; }
            double t = rloc.hitTime(oloc);
            if (t < 0) { continue; }
            coll2Time = min(coll2Time, t);
        }
        if (coll2Time > collTime) {
            cout << "passing into it" << endl << flush;
            // We are entering collEntity.
            // Set targetRef ????????????????
            targetRef = collEntity;
            // if (coll2Time > consts::basic_tick) { return; }
        }
    }
    // cout << "COLLISION" << endl << flush;
    if (collTime < getTickAddition(loc.coords)) {
        cout << "Setting target loc to " << loc.coords << "+" << loc.velocity
             << "*" << collTime;
        targetLocation = loc.coords + loc.velocity * collTime;
    } else {
        targetRef = NULL;
    }
}

void Movement::reset()
{
    serialno = serialno+1;
    targetRef=NULL;
    targetLocation=Vector3D();
    updatedLocation=Vector3D();
    velocity=Vector3D(0,0,0);
    lastMovementTime=body.world->getTime();
}
