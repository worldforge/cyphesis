// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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
