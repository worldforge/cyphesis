// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef PHYSICS_BBOX_H
#define PHYSICS_BBOX_H

#include <wfmath/axisbox.h>

typedef WFMath::AxisBox<3> BBox;

#include "physics/Vector3D.h"

// FIXME This boxContains function is deprecated
// Is point other inside this box, when this box is increased in each direction
inline bool boxContains(const BBox& box, const Vector3D& other, float increase)
{
    return ((other.x() > (box.lowCorner().x() - increase)) &&
            (other.x() < (box.highCorner().x() + increase)) &&
            (other.y() > (box.lowCorner().y() - increase)) &&
            (other.y() < (box.highCorner().y() + increase)) &&
            (other.z() > (box.lowCorner().z() - increase)) &&
            (other.z() < (box.highCorner().z() + increase)));
}

template<typename F>
inline F square(F f) { return f * f; }

/// What is the size of the box?
WFMath::CoordType boxSquareSize(const BBox & box);

inline WFMath::CoordType boxSize(const BBox & box)
{
    return sqrtf(boxSquareSize(box));
}

#endif // PHYSICS_BBOX_H
