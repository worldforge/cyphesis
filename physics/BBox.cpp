// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "BBox.h"

#include "common/const.h"

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
