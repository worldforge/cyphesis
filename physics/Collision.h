// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H

#include "physics/Vector3D.h"

class Location;

bool timeToHit(const Location & l,     // Location data of this object
               const Location & o,     // Location data of other object
               double & time,          // Returned time to collision
               Vector3D & normal);     // Returned collision normal

#endif // PHYSICS_COLLISION_H
