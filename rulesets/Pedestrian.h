// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef PEDESTRIAN_H
#define PEDESTRIAN_H

#include <physics/Vector3D.h>

#include "Movement.h"

class Character;

class Pedestrian : public Movement {
    friend class Character;
  public:
    Pedestrian(Character & body);
    virtual ~Pedestrian();

    double getTickAddition(const Vector3D & coordinates) const;
    Move * genFaceOperation(const Location &);
    Move * genMoveOperation(Location *,const Location &);
    Move * genMoveOperation(Location *);
};

#endif // PEDESTRIAN_H
