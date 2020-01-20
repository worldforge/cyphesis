// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "rules/simulation/Movement.h"

#include "rules/simulation/Entity.h"

#include "common/const.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

class TestMovement : public Movement {
  public:
    TestMovement(Entity & body) : Movement(body) { }

    virtual double getTickAddition(const Point3D & coordinates,
                                   const Vector3D & velocity) const {
        return 0.;
    }
    virtual int getUpdatedLocation(Location & return_location) {
        return 0;
    }
    virtual Atlas::Objects::Operation::RootOperation generateMove(const Location & new_location) {
        return Atlas::Objects::Operation::Move();
    }

};

int main()
{
    TestMovement * m = new TestMovement(*(Entity*)0);

    Location loc;
    m->updateNeeded(loc);

    m->reset();

    delete m;

    return 0;
}

#include "../stubs/rules/stubLocation.h"
