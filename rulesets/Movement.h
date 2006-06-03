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
#include "physics/Quaternion.h"

#include "common/types.h"

class Entity;
class Location;

/// \brief Base class for handling Character movement
///
/// This class should be replaced by a base class for handling all entity
/// movement.
class Movement {
  protected:
    Entity & m_body;
    Point3D m_targetPos;
    int m_serialno;

    // float checkCollisions(const Location & loc);
  public:
    explicit Movement(Entity & body);
    virtual ~Movement();

    int serialno() const {
        return m_serialno;
    }

    void setTarget(const Point3D & target) {
        m_targetPos = target;
    }

    void reset();
    bool updateNeeded(const Location & location) const;

    virtual double getTickAddition(const Point3D & coordinates,
                                   const Vector3D & velocity) const = 0;
    virtual int getUpdatedLocation(Location &) = 0;
    virtual Operation generateMove(Location &) = 0;
};

#endif // RULESETS_MOVEMENT_H
