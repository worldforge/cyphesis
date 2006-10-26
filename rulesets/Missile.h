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

// $Id: Missile.h,v 1.9 2006-10-26 00:48:09 alriddoch Exp $

#ifndef RULESETS_MISSILE_H
#define RULESETS_MISSILE_H

#include "Thing.h"

class Movement;

typedef Thing Missile_parent;

/// \brief This class is work in progress for missile objects.
///
/// This file is an idea which I am not going to implement before Acorn 0.4.
/// The idea is based on writing a class based on Movement which handles
/// projectiles. This object receives a specially tagged move operation when
/// the projectile is launched. the Movement object is used to convert this
/// into a real move op which is passed through to Thing::Operation(Move).
/// And a Tick is scheduled for when the projectile reaches its zenith.
/// At this point, the Movement object is again used to generate a new move
/// operation in order to move the projectile towards its target.
///
/// At both stages collision detection is done. Parabolic motion under gravity
/// is simplified to two linear segments, one ascending and one descending.
/// It is still possible to fire over stationart objects.
///
/// The ideas in this file will probably be made obsolete by ideas for moving
/// fundamental movement simulation into the PhysicalEntity (Thing) class.
class Missile : public Missile_parent {
  protected:
    
  public:
    virtual void MoveOperation(const RootOperation &, OpVector &);
    virtual void TickOperation(const RootOperation &, OpVector &);
};

#endif // RULESETS_MISSILE_H
