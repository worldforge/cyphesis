// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_MOVEMENT_H
#define RULESETS_MOVEMENT_H

class Character;

#include <physics/Vector3D.h>
#include <physics/Quaternion.h>

class Movement {
  protected:
    Thing & m_body;
    double m_lastMovementTime;
    Vector3D m_targetPos;
    Vector3D m_updatedPos;
    Vector3D m_velocity;
    Quaternion m_orientation;
    int m_serialno;
    Vector3D m_collPos;
    Entity * m_collEntity;
    bool m_collRefChange;
    int m_collAxis;

    bool updateNeeded(const Location & location) const;
    void checkCollisions(const Location & loc);

    friend class Character;
  public:
    explicit Movement(Thing & body);
    virtual ~Movement();

    void reset();
    bool moving() const;

    virtual double getTickAddition(const Vector3D & coordinates) const = 0;
    virtual Move * genFaceOperation() = 0;
    virtual Move * genMoveOperation(Location *,const Location &) = 0;
    virtual Move * genMoveOperation(Location *) = 0;
};

#endif // RULESETS_MOVEMENT_H
