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
inline WFMath::CoordType boxSquareSize(const BBox & box)
{
    WFMath::CoordType ans = 0;

    for(int i = 0; i < 3; ++i) {
        ans += square(box.highCorner()[i] - box.lowCorner()[i]);
    }

    return ans;
}

inline WFMath::CoordType boxSize(const BBox & box)
{
    return sqrtf(boxSquareSize(box));
}

/// Do this two boxes intersect
inline bool hit(const BBox & box, const BBox & other)
{
    return hit(box.lowCorner(), box.highCorner(),
               other.lowCorner(), other.highCorner());
}

/// When will box, moving with velocity vel hit box o, and on what axis
inline float timeToHit(const BBox & box, const Vector3D & vel,
                       const BBox & o, int & axis) 
{
    return timeToHit(box.lowCorner(), box.highCorner(), vel,
                     o.lowCorner(), o.highCorner(), axis);
}

/// When will box, moving with velocity vel leave box o
inline float timeToExit(const BBox & box, const Vector3D & vel, const BBox & o)
{
    return timeToExit(box.lowCorner(), box.highCorner(), vel,
                      o.lowCorner(), o.highCorner());
}

#endif // PHYSICS_BBOX_H
