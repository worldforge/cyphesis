// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

inline void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}

inline void addToEntity(const Vector3D & v, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = v[0];
    vd[1] = v[1];
    vd[2] = v[2];
}

template <typename FloatT>
int fromStdVector(Point3D & p, const std::vector<FloatT> & vf)
{
    if (vf.size() != 3) {
        return -1;
    }
    p[0] = vf[0];
    p[1] = vf[1];
    p[2] = vf[2];
    p.setValid();
    return 0;
}

template <typename FloatT>
int fromStdVector(Vector3D & v, const std::vector<FloatT> & vf)
{
    if (vf.size() != 3) {
        return -1;
    }
    v[0] = vf[0];
    v[1] = vf[1];
    v[2] = vf[2];
    v.setValid();
    return 0;
}

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
