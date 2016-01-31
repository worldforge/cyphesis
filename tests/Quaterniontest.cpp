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

#include "physics/Quaternion.h"

#include <wfmath/quaternion.h>
#include <wfmath/vector.h>

#include <cassert>

int main()
{
    WFMath::Vector<3> east(1,0,0), north(0,1,0), west(-1,0,0);
    WFMath::Vector<3> up(0,0,1), down(0,0,-1);

    Quaternion rotation;

    // Normal 90 degree rotation
    rotation = quaternionFromTo(east, north, up);

    // No rotation to cover special case
    rotation = quaternionFromTo(east, east, up);

    // Exact 180 to cover special case
    rotation = quaternionFromTo(east, west, up);

    // Exact 180 to cover other part of same case
    rotation = quaternionFromTo(up, down, west);

    return 0;
}
