// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MISSILE_H
#define MISSILE_H

// This file is an idea which I am not going to implement before Acorn 0.4.
// The idea is based on writing a class based on Movement which handles
// projectiles. This object receives a specially tagged move operation when
// the projectile is launched. the Movement object is used to convert this
// into a real move op which is passed through to Thing::Operation(Move).
// And a Tick is scheduled for when the projectile reaches its zenith.
// At this point, the Movement object is again used to generate a new move
// operation in order to move the projectile towards its target.

// At both stages collision detection is done. Parabolic motion under gravity
// is simplified to two linear segments, one ascending and one descending.
// It is still possible to fire over stationart objects.

#include "Thing.h"

class Movement;

class Missile : public Thing {
  protected:
    
  public:
    virtual oplist MoveOperation(const Move & op);
    virtual oplist TickOperation(const Tick & op);
};

#endif // MISSILE_H
