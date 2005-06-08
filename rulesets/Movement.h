// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_MOVEMENT_H
#define RULESETS_MOVEMENT_H

#include <physics/Vector3D.h>
#include <physics/Quaternion.h>

class Entity;
class Character;
class Location;

namespace Atlas { namespace Objects { namespace Operation {
  class Move;
} } }

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
    Vector3D m_velocity;
    Quaternion m_orient;
    int m_serialno;
    Point3D m_collPos;
    Entity * m_collEntity;
    bool m_collLocChange;
    Vector3D m_collNormal;

    bool updateNeeded(const Location & location) const;
    void checkCollisions(const Location & loc);

    friend class Character;
  public:
    explicit Movement(Entity & body);
    virtual ~Movement();

    void reset();
    bool moving() const;

    virtual double getTickAddition(const Point3D & coordinates,
                                   const Vector3D & velocity) const = 0;
    virtual int getUpdatedLocation(Location &) = 0;
    virtual Atlas::Objects::Operation::Move * generateMove(const Location&) = 0;
    virtual Atlas::Objects::Operation::Move * genFaceOperation() = 0;
    virtual Atlas::Objects::Operation::Move * genMoveUpdate(Location *) = 0;
    virtual Atlas::Objects::Operation::Move * genMoveOperation(Location *,
                                                        const Location &) = 0;
};

#endif // RULESETS_MOVEMENT_H
