// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_PEDESTRIAN_H
#define RULESETS_PEDESTRIAN_H

#include <physics/Vector3D.h>

#include "Movement.h"

class Character;

class Pedestrian : public Movement {
    friend class Character;
  public:
    explicit Pedestrian(Character & body);
    virtual ~Pedestrian();

    double getTickAddition(const Vector3D & coordinates) const;
    Move * genFaceOperation();
    Move * genMoveOperation(Location *,const Location &);
    Move * genMoveOperation(Location *);
};

#endif // RULESETS_PEDESTRIAN_H
