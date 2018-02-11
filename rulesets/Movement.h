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


#ifndef RULESETS_MOVEMENT_H
#define RULESETS_MOVEMENT_H

#include "physics/Vector3D.h"

#include <wfmath/point.h>

#include <Atlas/Objects/ObjectsFwd.h>

class LocatedEntity;
class Location;

/// \brief Base class for handling Character movement
///
/// This class should be replaced by a base class for handling all entity
/// movement.
class Movement {
  protected:
    /// The Entity this Movement is tracking.
    LocatedEntity & m_body;
    /// The destination position.
    Point3D m_targetPos;
    int m_serialno;

  public:
    Movement(const Movement &) = delete;
    explicit Movement(LocatedEntity & body);
    virtual ~Movement() = default;

    Movement & operator=(const Movement &) = delete;

    int serialno() const {
        return m_serialno;
    }

    bool hasTarget() const {
        return m_targetPos.isValid();
    }

    void setTarget(const Point3D & target) {
        m_targetPos = target;
    }

    void reset();
    bool updateNeeded(const Location & location) const;

    /// \brief Determine the time before the next update is required.
    ///
    /// Calculate how long before the Entity will reach its intended target.
    /// If there is no target, or the target is some distance away, then
    /// the default tick is returned.
    /// @param coordinates the current position.
    /// @param velocity the current velocity.
    virtual double getTickAddition(const Point3D & coordinates,
                                   const Vector3D & velocity) const = 0;
    /// \brief Calculate the update position of the entity.
    ///
    /// @param return_location the returned Location data.
    virtual int getUpdatedLocation(Location & return_location) = 0;
    /// \brief Generate a new Move operation to implement the movement.
    ///
    /// Create a Move operation object, and set up the argument to describe
    /// how the Entity is moving.
    /// @param new_location Location data about the entity once movement has
    /// changed.
    virtual Atlas::Objects::Operation::RootOperation generateMove(const Location & new_location) = 0;
};

#endif // RULESETS_MOVEMENT_H
