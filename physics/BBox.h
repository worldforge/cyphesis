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

#ifndef PHYSICS_BBOX_H
#define PHYSICS_BBOX_H

#include <wfmath/axisbox.h>

typedef WFMath::AxisBox<3> BBox;

template<typename F>
inline F square(F f) { return f * f; }

/// What is the size of the box?
WFMath::CoordType boxSquareSize(const BBox & box);

inline WFMath::CoordType boxSize(const BBox & box)
{
    return sqrtf(boxSquareSize(box));
}

WFMath::CoordType boxSquareBoundingRadius(const BBox & box);

inline WFMath::CoordType boxBoundingRadius(const BBox & box)
{
    return sqrtf(boxSquareBoundingRadius(box));
}

#endif // PHYSICS_BBOX_H
