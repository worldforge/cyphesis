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

#include "Domain.h"
#include "LocatedEntity.h"

#include "common/BaseWorld.h"
#include "common/const.h"
#include "common/debug.h"

#include "common/Tick.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

/// \brief Constructor
///
/// @param body the Entity this Movement is tracking.
Pedestrian::Pedestrian(LocatedEntity & body) : Movement(body)
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
    if (m_targetPos.isValid()) {
        WFMath::CoordType basic_square_distance = velocity.sqrMag()
                                                  * consts::square_basic_tick;
        WFMath::CoordType square_distance = squareDistance(coordinates, m_targetPos);
        debug( std::cout << "basic_distance: " << basic_square_distance
                         << std::endl << std::flush;);
        debug( std::cout << "distance: " << square_distance << std::endl
                         << std::flush;);
        if (basic_square_distance > square_distance) {
            debug( std::cout << "\tshortened tick" << std::endl << std::flush;);
            return std::sqrt(square_distance / basic_square_distance)
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
    WFMath::CoordType time_diff = (WFMath::CoordType)(BaseWorld::instance().getTime() - m_body.m_location.timeStamp());

    if (!updateNeeded(m_body.m_location)) {
        debug( std::cout << "No update" << std::endl << std::flush;);
        return 1;
    }

    Location new_location(m_body.m_location);

    // Update location
    Point3D new_coords = m_body.m_location.pos();
    new_coords += (m_body.m_location.velocity() * time_diff);
    if (m_targetPos.isValid()) {
        Point3D new_coords2 = new_coords;
        new_coords2 += (m_body.m_location.velocity() * (consts::basic_tick / 10.f));
        // The values returned by squareDistance are squares, so
        // cannot be used except for comparison
        WFMath::CoordType dist = squareDistance(m_targetPos, new_coords);
        WFMath::CoordType dist2 = squareDistance(m_targetPos, new_coords2);
        debug( std::cout << "dist: " << dist << "," << dist2 << std::endl
                         << std::flush;);
        if (dist2 > dist) {
            // If dist2 is larger than dist, then further movement
            // is away from m_targetPos, so we know we have arrived.
            debug( std::cout << "m_targetPos achieved";);
            new_location.m_pos = m_targetPos;

            // We have arrived at our target position and must
            // stop, or be deflected
            // Arrived at intended destination
            reset();
            new_location.m_velocity = Vector3D::ZERO();
        } else {
            new_location.m_pos = new_coords;
        }
    } else {
        new_location.m_pos = new_coords;
    }
    // FIXME Use the movement_domain to apply the constraints.

//    if (m_body.m_location.m_loc) {
//        auto domain = m_body.m_location.m_loc->getDomain();
//        if (domain) {
//            float z = domain->constrainHeight(m_body, new_location.m_loc,
//                                                                  new_location.m_pos,
//                                                                  "standing");
//            debug(std::cout << "Height adjustment " << z << " " << new_location.m_pos.z()
//                            << std::endl << std::flush;);
//
//            new_location.m_pos.z() = z;
//        }
//    }
    return_location = new_location;

    return 0;
}

Operation Pedestrian::generateMove(const Location & new_location)
{
    // Create move operation
    Move moveOp;
    moveOp->setTo(m_body.getId());
    moveOp->setSeconds(BaseWorld::instance().getTime());

    // Set up argument for operation
    Anonymous move_arg;
    move_arg->setId(m_body.getId());

    // FIXME Query from the movement_domain what the constraints are.
    // On water, on land or falling?

    new_location.addToEntity(move_arg);
    moveOp->setArgs1(move_arg);

    return moveOp;
}
