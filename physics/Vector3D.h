// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef PHYSICS_VECTOR_3D_H
#define PHYSICS_VECTOR_3D_H

#include <wfmath/vector.h>

#include <cmath>
#include <vector>

/// Used to indicate which axis
static const int cX = 0;
static const int cY = 1;
static const int cZ = 2;

typedef WFMath::Vector<3> Vector3D;

/// Find relative distance, to be used when the result is only
/// going to be compared with other distances
float squareDistance(const Vector3D & u, const Vector3D & v);

/// Find the distance between two vectors
inline float distance(const Vector3D & u, const Vector3D & v)
{
    return sqrt(squareDistance(u, v));
}

inline bool isZero(const Vector3D & u)
{
    return (u.isValid() && (u.x() == 0) && (u.y() == 0) && (u.z() == 0));
}

typedef std::vector<Vector3D> CoordList;

#endif // PHYSICS_VECTOR_3D_H
