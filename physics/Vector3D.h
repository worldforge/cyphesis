// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef PHYSICS_VECTOR_3D_H
#define PHYSICS_VECTOR_3D_H

#include <wfmath/vector.h>
#include <wfmath/point.h>

#include <cmath>
#include <vector>

/// Used to indicate which axis
static const int cX = 0;
static const int cY = 1;
static const int cZ = 2;

typedef WFMath::Point<3> Point3D;
typedef WFMath::Vector<3> Vector3D;

inline float sqrMag(const Point3D & p)
{
    return p.x() * p.x() + p.y() * p.y() + p.z() * p.z();
}

/// Find relative distance, to be used when the result is only
/// going to be compared with other distances
float squareDistance(const Point3D & u, const Point3D & v);

/// Find the distance between two points
inline float distance(const Point3D & u, const Point3D & v)
{
    return sqrt(squareDistance(u, v));
}

inline bool isZero(const Vector3D & u)
{
    return (u.isValid() && (u.x() == 0) && (u.y() == 0) && (u.z() == 0));
}

typedef std::vector<Point3D> CoordList;
typedef std::vector<Vector3D> VectorList;

#endif // PHYSICS_VECTOR_3D_H
