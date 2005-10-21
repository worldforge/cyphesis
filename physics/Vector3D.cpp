// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

// Remember kids, it takes 7 times longer to call mag() than to call relMag(),
// so if you are comparing vector magnitudes, always use relMag().

#include "Vector3D.h"

static inline float sqr(float x)
{
    return x * x;
}

float squareDistance(const Point3D & u, const Point3D & v)
{
    return (sqr(u.x() - v.x()) + sqr(u.y() - v.y()) + sqr(u.z() - v.z()));
}

