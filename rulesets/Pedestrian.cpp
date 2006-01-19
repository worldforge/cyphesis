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

#include "Pedestrian.h"

#include "Entity.h"

#include "common/const.h"
#include "common/debug.h"
#include "common/log.h"
#include "common/compose.hpp"

#include "common/Tick.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

Pedestrian::Pedestrian(Entity & body) : Movement(body)
{
}

Pedestrian::~Pedestrian()
{
}

double Pedestrian::getTickAddition(const Point3D & coordinates,
                                   const Vector3D & velocity) const
{
    // This may seem a little weird. Everything is handled in squares to
    // reduce the number of square roots that have to be calculated. In
    // this case only one is required.
    double basic_square_distance = velocity.sqrMag()
                                   * consts::square_basic_tick;
    const Point3D & target = m_collPos.isValid() ? m_collPos : m_targetPos;
    if (target.isValid()) {
        double square_distance = squareDistance(coordinates, target);
        debug( std::cout << "basic_distance: " << basic_square_distance
                         << std::endl << std::flush;);
        debug( std::cout << "distance: " << square_distance << std::endl
                         << std::flush;);
        if (basic_square_distance > square_distance) {
            debug( std::cout << "\tshortened tick" << std::endl << std::flush;);
            return sqrt(square_distance / basic_square_distance)
                        * consts::basic_tick;
        }
    }
    return consts::basic_tick;
}

/// \brief Calculate the updated position of the entity since the last
/// move operation.
///
/// The does not modify the entity, but determines a new Location based
/// on the velocity of the entity, time elapsed, whether any collision
/// occurs, and whether the entity has reached its target.
/// @return 1 if no update was made, or 0 otherwise
int Pedestrian::getUpdatedLocation(Location & return_location)
{
    const double & current_time = m_body.m_world->getTime();
    double time_diff = current_time - m_body.m_location.timeStamp();
    // Don't update time yet, but FIXME it must be done when the operation
    // is actually generated. In fact FIXME it should be updated when the
    // operation is dispatched. It may be a good idea to put a time stamp
    // in Location, as it will be useful elsewhere, and will eliminate the
    // race condition.
    m_body.m_location.update(current_time);

    if (!updateNeeded(m_body.m_location)) {
        debug( std::cout << "No update" << std::endl << std::flush;);
        return 1;
    }

    Location new_location(m_body.m_location);
    // m_velocity and m_orient are of no interest yet. They contain old data,
    // and definitly should be the same as our current stuff.
    // new_location.m_velocity = m_velocity;
    // new_location.orientation() = m_orient;

    double vel_square_mag = m_body.m_location.velocity().sqrMag();

    // Update location
    Point3D new_coords = m_updatedPos.isValid() ? m_updatedPos
                                                : m_body.m_location.pos();
    // m_updatedPos is required later if we detect a race, so don't
    // invalidate it yet. Every path from this point on must either
    // invalidate it or store an update position, except the race condition
    // path. m_updatedPos is always invalidated by a call to reset().
    new_coords += (m_body.m_location.velocity() * time_diff);
    const Point3D & target = m_collPos.isValid() ? m_collPos : m_targetPos;
    if (target.isValid()) {
        Point3D new_coords2 = new_coords;
        new_coords2 += (m_body.m_location.velocity() * (consts::basic_tick / 10.0));
        // The values returned by squareDistance are squares, so
        // cannot be used except for comparison
        double dist = squareDistance(target, new_coords);
        double dist2 = squareDistance(target, new_coords2);
        debug( std::cout << "dist: " << dist << "," << dist2 << std::endl
                         << std::flush;);
        if (dist2 > dist) {
            // If dist2 is larger than dist, then further movement
            // is away from target, so we know we have arrived.
            debug( std::cout << "target achieved";);
            new_location.m_pos = target;

            bool collision_judder = false;
            if (m_collPos.isValid()) {
                assert(m_collEntity != 0);
                // If we are at a collision, note the time. Check if its very
                // short time since the last collision, and if so flag this.
                if ((m_body.m_world->getTime() - m_lastCollisionTime) < 0.01) {
                    collision_judder = true;
                }
                m_lastCollisionTime = m_body.m_world->getTime();
                debug(std::cout << "COLL: " << m_body.m_location.m_loc->getType()
                                << "."      << m_body.m_location.pos()
                                << " "      << new_coords
                                << "."      << target
                                << "<"      << m_collPos.isValid() << "," << m_targetPos.isValid() << ">"
                                << " "      << (m_updatedPos.isValid() ? m_updatedPos : Point3D(-1,-1,-1))
                                << "."      << (m_collLocChange ? "change" : "none")
                                << "."      << (m_collEntity->getType())
                                << std::endl << std::flush;);
            } else {
                assert(m_targetPos.isValid());
                assert(m_collEntity == 0);
                assert(!m_collLocChange);
            }
            if (collision_judder) {
                // If collision are getting messy, just stop.
                // This prevents the code getting in nasty tight loops where
                // loads of CPU gets chewed.
                reset();
                new_location.m_velocity = Vector3D(0,0,0);
            } else if (m_collLocChange) {
                assert(m_collPos.isValid());
                // We are changing container (LOC)
                static const Quaternion identity(Quaternion().identity());
                debug(std::cout << "CONTACT " << m_collEntity->getId()
                                << std::endl << std::flush;);
                if (m_collEntity == m_body.m_location.m_loc->m_location.m_loc) {
                    // Passing out of current container
                    debug(std::cout << "OUT" << target
                                    << m_body.m_location.m_loc->m_location.pos()
                                    << std::endl << std::flush;);
                    const Quaternion & collOrientation = m_body.m_location.m_loc->m_location.orientation().isValid() ?
                                                         m_body.m_location.m_loc->m_location.orientation() :
                                                         identity;
                    new_location.m_pos = new_location.m_pos.toParentCoords(m_body.m_location.m_loc->m_location.pos(), collOrientation);
                    new_location.m_orientation *= collOrientation;
                    new_location.m_velocity.rotate(collOrientation);
                    new_location.m_loc = m_collEntity;
                    if (m_targetPos.isValid()) {
                        m_targetPos = m_targetPos.toParentCoords(m_body.m_location.m_loc->m_location.pos(), collOrientation);
                    }
                } else if (m_collEntity->m_location.m_loc == m_body.m_location.m_loc) {
                    // Passing into new container
                    debug(std::cout << "IN" << std::endl << std::flush;);
                    const Quaternion & collOrientation = m_collEntity->m_location.orientation().isValid() ?
                                                         m_collEntity->m_location.orientation() :
                                                         identity;
                    new_location.m_pos = new_location.m_pos.toLocalCoords(m_collEntity->m_location.pos(), collOrientation);
                    new_location.m_orientation /= collOrientation;
                    new_location.m_velocity.rotate(collOrientation.inverse());
                    new_location.m_loc = m_collEntity;
                    if (m_targetPos.isValid()) {
                        m_targetPos = m_targetPos.toLocalCoords(m_collEntity->m_location.pos(), collOrientation);
                    }
                } else {
                    // Container we are supposed to changing to is wrong.
                    // Just stop where we currently are. Debugging is required to work out
                    // why this happens
                    log(ERROR, String::compose("BAD COLLISION: %1(%2) with %3(%4)%5 when LOC is currently %6(%7)%8.",
                                               m_body.getId(),
                                               m_body.getType(),
                                               m_collEntity->getId(),
                                               m_collEntity->getType(),
                                               new_location.m_pos,
                                               m_body.m_location.m_loc->getId(),
                                               m_body.m_location.m_loc->getType(),
                                               m_body.m_location.m_pos).c_str());
                    reset();
                    new_location.m_velocity = Vector3D(0,0,0);
                    new_location.m_pos = m_body.m_location.m_pos;
                }
                m_collEntity = NULL;
                m_collLocChange = false;
                m_collPos.setValid(false);
                m_updatedPos.setValid(false);
            } else {
                // We have arrived at our target position and must
                // stop, or be deflected
                if (m_collPos.isValid()) {
                    if (m_body.m_location.m_loc != m_collEntity->m_location.m_loc) {
                        // Race condition
                        // This occurs if we get asked for a new update before
                        // the last move has taken effect, so we make the new
                        // pos exactly as it was when the last collision was
                        // predicted. We don't modify m_updatedPos here because
                        // its value is still fine.
                        log(ERROR, "NON COLLISION");
                        new_location.m_pos = m_updatedPos.isValid() ? m_updatedPos
                                                                    : m_body.m_location.pos();
                    } else {
                        // Generate touch ops
                        // This code relies on m_collNormal being a unit vector
                        new_location.m_velocity -= m_collNormal * Dot(m_collNormal, new_location.m_velocity);
                        if ((new_location.m_velocity.mag() / consts::base_velocity) > 0.05) {
                            m_collPos.setValid(false);
                            m_collEntity = NULL;
                            new_location.m_velocity.normalize();
                            new_location.m_velocity *= sqrt(vel_square_mag);
                            if (m_targetPos.isValid()) {
                                m_diverted = true;
                            }
                            // m_updatedPos must only be set if no LOC change has occured.
                            m_updatedPos = new_location.m_pos;
                        } else {
                            reset();
                            new_location.m_velocity = Vector3D(0,0,0);
                        }
                    }
                } else {
                    // Arrived at intended destination
                    if (m_diverted) {
                        new_location.m_pos = new_coords;
                    }
                    reset();
                    new_location.m_velocity = Vector3D(0,0,0);
                }
            }
        } else {
            new_location.m_pos = new_coords;
            // m_updatedPos must only be set if no LOC change has occured.
            m_updatedPos = new_location.m_pos;
            m_diverted = false;
        }
    } else {
        new_location.m_pos = new_coords;
        // m_updatedPos must only be set if no LOC change has occured.
        m_updatedPos = new_location.m_pos;
    }

    std::string mode("standing");

    if (m_body.has("mode")) {
        Element mode_attr;
        m_body.get("mode", mode_attr);
        if (mode_attr.isString()) {
            mode = mode_attr.String();
        } else {
            log(ERROR, String::compose("Mode on entity is a %1 in Pedestrain::getUpdatedLocation", Element::typeName(mode_attr.getType())).c_str());
        }
    }

    float z = m_body.m_world->constrainHeight(new_location.m_loc, new_location.m_pos,
                                              mode);
    debug(std::cout << "Height adjustment " << z << " " << new_location.m_pos.z()
                    << std::endl << std::flush;);

    new_location.m_pos.z() = z;

    // new_location.m_pos = new_coords;

    return_location = new_location;

    return 0;
}

Operation Pedestrian::generateMove(Location & new_location)
{
    // Create move operation
    Move moveOp;
    moveOp->setTo(m_body.getId());

    // Set up argument for operation
    Anonymous move_arg;
    move_arg->setId(m_body.getId());

    // Walk out what the mode of the character should be.
    // Performed in squares to save on that critical sqrt() call
    double vel_square_mag = 0;
    if (new_location.velocity().isValid()) {
        vel_square_mag = new_location.velocity().sqrMag();
    }
    double square_speed_ratio = vel_square_mag / consts::square_base_velocity;

    if (vel_square_mag > 0) {
        if (checkCollisions(new_location) <= 0) {
            // FIXME THis ignore the possiblity that the collision might
            // just deflect, or cause the entity to enter the collision
            // entity. It alse leaves the collEntity, and collPos set.
            new_location.m_velocity = Vector3D(0,0,0);
            vel_square_mag = 0;
            square_speed_ratio = 0;
        }
    }

    float height = 0;
    if (m_body.m_location.bBox().isValid()) {
        height = m_body.m_location.bBox().highCorner().z() - 
                 m_body.m_location.bBox().lowCorner().z();
    }

    if (new_location.pos().z() < (0 - height * 0.75)) {
        move_arg->setAttr("mode", "swimming");
    } else {
        if (square_speed_ratio > 0.25) { // 0.5 ^ 2
            move_arg->setAttr("mode", "running");
        } else if (square_speed_ratio > 0.0025) { // 0.05 ^ 2
            move_arg->setAttr("mode", "walking");
        } else {
            move_arg->setAttr("mode", "standing");
        }
    }
    debug(std::cout << move_arg->getAttr("mode").asString() << std::endl << std::flush;);

    new_location.addToEntity(move_arg);
    moveOp->setArgs1(move_arg);

    return moveOp;
}
