// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MOVEMENT_H
#define MOVEMENT_H

class Character;

#include <physics/Vector3D.h>

class Movement {
  protected:
    Character & body;
    double lastMovementTime;
    Vector3D targetPos;
    Vector3D updatedPos;
    Vector3D velocity;
    Vector3D face;
    int serialno;
    Vector3D collPos;
    Entity * collRef;
    int collAxis;

    bool updateNeeded(const Location & location) const;
    void checkCollisions(const Location & loc);

    friend class Character;
  public:
    Movement(Character & body);
    virtual ~Movement();

    void reset();

    virtual double getTickAddition(const Vector3D & coordinates) const = 0;
    virtual Move * genFaceOperation(const Location &) = 0;
    virtual Move * genMoveOperation(Location *,const Location &) = 0;
    virtual Move * genMoveOperation(Location *) = 0;
};

#endif // MOVEMENT_H
