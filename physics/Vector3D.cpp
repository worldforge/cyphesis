// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

// Remember kids, it takes 7 times longer to call mag() than to call relMag(),
// so if you are comparing vector magnitudes, always use relMag().

#include "Vector3D.h"

float squareDistance(const Vector3D & u, const Vector3D & v)
{
    return ((u.x() - v.x())*(u.x() - v.x()) + (u.y() - v.y())*(u.y() - v.y()) + (u.z() - v.z())*(u.z() - v.z()));
}

