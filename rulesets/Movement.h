// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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
    double m_lastMovementTime;
    Point3D m_targetPos;
    Point3D m_updatedPos;
    int m_serialno;
    Point3D m_collPos;
    Entity * m_collEntity;
    bool m_collLocChange;
    Vector3D m_collNormal;

    bool updateNeeded(const Location & location) const;
    void checkCollisions(const Location & loc);
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

    virtual double getTickAddition(const Point3D & coordinates,
                                   const Vector3D & velocity) const = 0;
    virtual int getUpdatedLocation(Location &) = 0;
    virtual Operation * generateMove(const Location&) = 0;
};

#endif // RULESETS_MOVEMENT_H
