// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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


#include "BBox.h"
#include "Vector3D.h"

#include <wfmath/axisbox.h>
#include <wfmath/point.h>

WFMath::CoordType boxSquareSize(const BBox & box)
{
    return square(box.highCorner().x() - box.lowCorner().x()) +
           square(box.highCorner().y() - box.lowCorner().y()) +
           square(box.highCorner().z() - box.lowCorner().z());
}


WFMath::CoordType boxSquareBoundingRadius(const BBox & box)
{
    return std::max(sqrMag(box.lowCorner()), sqrMag(box.highCorner()));
}

WFMath::CoordType boxSquareHorizontalBoundingRadius(const BBox & box)
{
    return std::max(sqrMag(WFMath::Point<2>(box.lowCorner().x(), box.lowCorner().z())),
            sqrMag(WFMath::Point<2>(box.highCorner().x(), box.highCorner().z())));
}

void boxScale(BBox& box, float scale)
{
    float side_scale = std::pow(scale, 0.33333f);

    box = BBox(WFMath::Point<3>(box.lowCorner().x() * side_scale,
                        box.lowCorner().y() * side_scale,
                        box.lowCorner().z() * side_scale),
            WFMath::Point<3>(box.highCorner().x() * side_scale,
                        box.highCorner().y() * side_scale,
                        box.highCorner().z() * side_scale));
}


