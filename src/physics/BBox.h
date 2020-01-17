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


#ifndef PHYSICS_B_BOX_H
#define PHYSICS_B_BOX_H

#include <wfmath/const.h>

#include <cmath>

typedef WFMath::AxisBox<3> BBox;


/// What is the size of the box?
WFMath::CoordType boxSquareSize(const BBox & box);

inline WFMath::CoordType boxSize(const BBox & box)
{
    return std::sqrt(boxSquareSize(box));
}

WFMath::CoordType boxSquareBoundingRadius(const BBox & box);
WFMath::CoordType boxSquareHorizontalBoundingRadius(const BBox & box);

inline WFMath::CoordType boxBoundingRadius(const BBox & box)
{
    return std::sqrt(boxSquareBoundingRadius(box));
}

inline WFMath::CoordType boxHorizontalBoundingRadius(const BBox & box)
{
    return std::sqrt(boxSquareBoundingRadius(box));
}

void boxScale(BBox& box, float scale);

#endif // PHYSICS_B_BOX_H
