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

// $Id$

#include "BBox.h"

#include "common/const.h"

#include <wfmath/axisbox.h>
#include <wfmath/point.h>

WFMath::CoordType boxSquareSize(const BBox & box)
{
    if (!box.isValid()) {
        return consts::minSqrBoxSize;
    }

    return square(box.highCorner().x() - box.lowCorner().x()) +
           square(box.highCorner().y() - box.lowCorner().y()) +
           square(box.highCorner().z() - box.lowCorner().z());
}

static WFMath::CoordType sqrMag(const WFMath::Point<3> & p)
{
    return p.x() * p.x() + p.y() * p.y() + p.z() * p.z();
}

WFMath::CoordType boxSquareBoundingRadius(const BBox & box)
{
    return std::max(sqrMag(box.lowCorner()), sqrMag(box.highCorner()));
}
