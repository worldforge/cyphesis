// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Move.h>

#include <common/Tick.h>

#include <varconf/Config.h>

extern "C" {
    #include <stdlib.h>
}

#include "Character.h"
#include "Pedestrian.h"

#include <modules/Location.h>

#include <common/const.h>
#include <common/debug.h>

using Atlas::Message::Object;

static const bool debug_flag = false;

Pedestrian::Pedestrian(Character & body) : Movement(body)
{
}

Pedestrian::~Pedestrian()
{
}

double Pedestrian::getTickAddition(const Vector3D & coordinates) const
{
    double basic_distance=velocity.mag()*consts::basic_tick;
    const Vector3D & target = collPos ? collPos : targetPos;
    if (target) {
        double distance=coordinates.distance(target);
        debug( cout << "basic_distance: " << basic_distance << endl << flush;);
        debug( cout << "distance: " << distance << endl << flush;);
        if (basic_distance>distance) {
            debug( cout << "\tshortened tick" << endl << flush;);
            return distance/basic_distance*consts::basic_tick;
        }
    }
    return consts::basic_tick;
}

Move * Pedestrian::genFaceOperation(const Location & loc)
{
    if (face != loc.face) {
        face = loc.face;
        debug( cout << "Turning" << endl << flush;);
        Move * moveOp = new Move(Move::Instantiate());
        moveOp->SetTo(body.fullid);
        Object::MapType _map;
        Object ent(_map);
        Object::MapType & entmap = ent.AsMap();
        entmap["id"] = body.fullid;
        loc.addToObject(ent);
        Object::ListType args(1,ent);
        moveOp->SetArgs(args);
        return moveOp;
    }
    return NULL;
}

Move * Pedestrian::genMoveOperation(Location * rloc)
{
    return genMoveOperation(rloc, body.location);
}

Move * Pedestrian::genMoveOperation(Location * rloc, const Location & loc)
{
        debug( cout << "genMoveOperation: status: Pedestrian(" << serialno
             << "," << collPos << "," << targetPos << "," << velocity << ","
             << lastMovementTime << ")" << endl << flush;);
    if (updateNeeded(loc)) {
        debug(cout << "genMoveOperation: Update needed..." << endl << flush;);

        // Sort out time difference, and set updated time
        const double & current_time=body.world->getTime();
        double time_diff=current_time-lastMovementTime;
        debug( cout << "time_diff:" << time_diff << endl << flush;);
        lastMovementTime=current_time;

        face = loc.face;
        
        Location new_loc(loc);
        new_loc.velocity=velocity;

        // Create move operation
        Move * moveOp = new Move(Move::Instantiate());
        moveOp->SetTo(body.fullid);

        // Set up argument for operation
        Object::MapType _map;
        Object ent(_map);
        Object::MapType & entmap = ent.AsMap();
        entmap["id"] = body.fullid;

        // Walk out what the mode of the character should be.
        double vel_mag = velocity.mag();
        double speed_ratio;
        if (vel_mag == 0.0) {
            speed_ratio = 0.0;
        } else {
            speed_ratio = vel_mag/consts::base_velocity;
        }
        string mode;
        if (speed_ratio > 0.5) {
            mode = string("running");
        } else if (speed_ratio > 0.05) {
            mode = string("walking");
        } else {
            mode = string("standing");
        }
        debug( cout << "Mode set to " << mode << endl << flush;);
        entmap["mode"] = Object(mode);

        // If velocity is not set, return this simple operation.
        if (!velocity) {
            debug( cout << "only velocity changed." << endl << flush;);
            new_loc.addToObject(ent);
            Object::ListType args(1,ent);
            moveOp->SetArgs(args);
            if (NULL != rloc) {
                *rloc = new_loc;
            }
            return moveOp;
        }

        // Update location
        Vector3D new_coords;
        if (updatedPos) {
            new_coords=updatedPos+(velocity*time_diff);
        } else {
            new_coords=loc.coords+(velocity*time_diff);
        }
        const Vector3D & target = collPos ? collPos : targetPos;
        if (target) {
            Vector3D new_coords2 = new_coords+velocity/consts::basic_tick/10.0;
            double dist=target.distance(new_coords);
            double dist2=target.distance(new_coords2);
            debug( cout << "dist: " << dist << "," << dist2 << endl << flush;);
            if (dist2>dist) {
                debug( cout << "target achieved";);
                new_coords=target;
                if (collRef != NULL) {
                    cout << "CONTACT " << collRef->fullid << endl << flush;
                    if (collRef == new_loc.ref->location.ref) {
                        cout << "OUT" << target << new_loc.ref->location.coords << endl << flush;
                        new_coords=target + new_loc.ref->location.coords;
                    } else {
                        cout << "IN" << endl << flush;
                        new_coords=target - collRef->location.coords;
                    }
                    new_loc.ref = collRef;
                    collRef = NULL;
                    // Transform targetPos to new ref?
                    collPos = Vector3D();
                } else {
                    if (collPos) {
                        velocity[collAxis] = 0;
                        collPos = Vector3D();
                    } else {
                        reset();
                        entmap["mode"] = Object("standing");
                    }
                    new_loc.velocity=velocity;
                }
            }
        }
        new_loc.coords = new_coords;
        updatedPos = new_coords;

        // Check for collisions
        checkCollisions(new_loc);

        debug( cout << "new coordinates: " << new_coords << endl << flush;);
        new_loc.addToObject(ent);
        Object::ListType args2(1,ent);
        moveOp->SetArgs(args2);
        if (NULL != rloc) {
            *rloc = new_loc;
        }
        return moveOp;
    }
    return NULL;
}
