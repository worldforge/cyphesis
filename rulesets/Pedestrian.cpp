// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Move.h>

#include <common/Tick.h>

#include "Character.h"
#include "Pedestrian.h"

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
    double basic_distance = m_velocity.mag() * consts::basic_tick;
    const Vector3D & target = m_collPos ? m_collPos : m_targetPos;
    if (target) {
        double distance=coordinates.distance(target);
        debug( cout << "basic_distance: " << basic_distance << endl << flush;);
        debug( cout << "distance: " << distance << endl << flush;);
        if (basic_distance>distance) {
            debug( cout << "\tshortened tick" << endl << flush;);
            return distance / basic_distance * consts::basic_tick;
        }
    }
    return consts::basic_tick;
}

Move * Pedestrian::genFaceOperation(const Location & loc)
{
    if (m_face != loc.face) {
        m_face = loc.face;
        debug( cout << "Turning" << endl << flush;);
        Move * moveOp = new Move(Move::Instantiate());
        moveOp->SetTo(m_body.fullid);
        Object::MapType _map;
        Object ent(_map);
        Object::MapType & entmap = ent.AsMap();
        entmap["id"] = m_body.fullid;
        loc.addToObject(ent);
        Object::ListType args(1,ent);
        moveOp->SetArgs(args);
        return moveOp;
    }
    return NULL;
}

Move * Pedestrian::genMoveOperation(Location * rloc)
{
    return genMoveOperation(rloc, m_body.location);
}

Move * Pedestrian::genMoveOperation(Location * rloc, const Location & loc)
{
    debug(std::cout << "genMoveOperation: Pedestrian(" << m_serialno << ","
               << m_collPos << "," << m_targetPos << "," << m_velocity << ","
               << m_lastMovementTime << ")" << std::endl << std::flush;);
    if (!updateNeeded(loc)) {
        return NULL;
    }

    debug(cout << "genMoveOperation: Update needed..." << endl << flush;);

    // Sort out time difference, and set updated time
    const double & current_time = m_body.world->getTime();
    double time_diff = current_time - m_lastMovementTime;
    debug( cout << "time_diff:" << time_diff << endl << flush;);
    m_lastMovementTime = current_time;

    m_face = loc.face;
    
    Location new_loc(loc);
    new_loc.velocity = m_velocity;

    // Create move operation
    Move * moveOp = new Move(Move::Instantiate());
    moveOp->SetTo(m_body.fullid);

    // Set up argument for operation
    Object::MapType _map;
    Object ent(_map);
    Object::MapType & entmap = ent.AsMap();
    entmap["id"] = m_body.fullid;

    // Walk out what the mode of the character should be.
    double vel_mag = m_velocity.mag();
    double speed_ratio;
    if (vel_mag == 0.0) {
        speed_ratio = 0.0;
    } else {
        speed_ratio = vel_mag / consts::base_velocity;
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
    if (!m_velocity) {
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
    if (m_updatedPos) {
        new_coords = m_updatedPos + (m_velocity * time_diff);
    } else {
        new_coords = loc.coords + (m_velocity * time_diff);
    }
    const Vector3D & target = m_collPos ? m_collPos : m_targetPos;
    if (target) {
        Vector3D new_coords2 = new_coords + m_velocity / consts::basic_tick / 10.0;
        double dist = target.distance(new_coords);
        double dist2 = target.distance(new_coords2);
        debug( cout << "dist: " << dist << "," << dist2 << endl << flush;);
        if (dist2>dist) {
            debug( cout << "target achieved";);
            new_coords = target;
            if (m_collRefChange) {
                debug(cout << "CONTACT " << m_collEntity->fullid << endl << flush;);
                if (m_collEntity == new_loc.ref->location.ref) {
                    debug(cout << "OUT" << target << new_loc.ref->location.coords << endl << flush;);
                    new_coords = target + new_loc.ref->location.coords;
                } else {
                    debug(cout << "IN" << endl << flush;);
                    new_coords = target - m_collEntity->location.coords;
                }
                new_loc.ref = m_collEntity;
                m_collEntity = NULL;
                m_collRefChange = false;
                // Transform m_targetPos to new ref?
                m_collPos = Vector3D();
            } else {
                if (m_collPos) {
                    // Generate touch ops
                    m_velocity[m_collAxis] = 0;
                    m_collPos = Vector3D();
                    if ((m_velocity.mag() / consts::base_velocity) > 0.05) {
                        new_loc.face = m_velocity;
                    } else {
                        reset();
                        entmap["mode"] = Object("standing");
                    }
                } else {
                    reset();
                    entmap["mode"] = Object("standing");
                }
                new_loc.velocity = m_velocity;
            }
        }
    }
    new_loc.coords = new_coords;
    m_updatedPos = new_coords;

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
