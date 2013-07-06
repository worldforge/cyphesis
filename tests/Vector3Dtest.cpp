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

#include "physics/Vector3D.h"

#include <wfmath/point.h>
#include <wfmath/vector.h>

#include <cassert>

int main()
{
    Point3D point1(0,0,0), point2(1,1,1);

    assert(squareDistance(point1, point2) > 1);

    std::vector<double> empty, point_store, vector_store;

    addToEntity(point1, point_store);
 
    Vector3D vector1(0,0,0);

    addToEntity(vector1, vector_store);

    assert(fromStdVector(point1, empty) == -1);
    assert(fromStdVector(vector1, empty) == -1);

    assert(fromStdVector(point1, point_store) == 0);
    assert(fromStdVector(vector1, vector_store) == 0);

    sqrMag(point1);
    sqrMag(point2);

    return 0;
}
