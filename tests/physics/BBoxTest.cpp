// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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

#include "physics/BBox.h"

#include "common/const.h"

#include <wfmath/axisbox.h>
#include <wfmath/point.h>

#include <cassert>
#include <physics/Vector3D.h>

int main()
{

    {
        WFMath::Point<2> p(2, 2);
        auto mag = sqrMag(p);
        assert(mag == 8.0);
    }

    {

        assert(boxSquareSize({{0, 0, 0},
                              {1, 1, 1}}) != 0);
    }

    {
        boxSquareBoundingRadius({{0, 0, 0},
                                 {1, 1, 1}});
    }

    {
        auto radius = std::sqrt(boxSquareHorizontalBoundingRadius({{-1, 0, -1},
                                                         {1,  1, 1}}));
        assert(WFMath::Equal(1.414215, radius, 0.001));
    }

    return 0;
}
