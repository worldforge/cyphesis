// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef PHYSICS_VECTOR_3D_H
#define PHYSICS_VECTOR_3D_H

#include <wfmath/vector.h>

#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>

using std::cos;
using std::sin;

typedef std::pair<double, double> range;

static inline const range timeToHit(double n, double f, double u,
                                    double on, double of) {
    return range((on - f)/u, (of - n)/u);
}

// This version is for checking the item is totally or partly inside the box
// It is actually the same as timeToHit, but the other way around.
static inline const range timeToExit(double n, double f, double u,
                                     double on, double of) {
    return range((on - n)/u, (of - f)/u);

}

static inline double max(range r) {
    return std::max(r.first, r.second);
}

static inline double min(range r) {
    return std::min(r.first, r.second);
}

/// Used to indicate which axis
static const int cX = 0;
static const int cY = 1;
static const int cZ = 2;

typedef WFMath::Vector<3> Vector3D;

/// Find relative distance, to be used when the result is only
/// going to be compared with other distances
inline double squareDistance(const Vector3D & u, const Vector3D & v)
{
    return ((u.x() - v.x())*(u.x() - v.x()) + (u.y() - v.y())*(u.y() - v.y()) + (u.z() - v.z())*(u.z() - v.z()));
}

/// Find the distance between two vectors
inline double distance(const Vector3D & u, const Vector3D & v)
{
    return sqrt(squareDistance(u, v));
}

/// Is vector u less than size in every direction
inline bool in(const Vector3D & u, double size)
{
    return ((u.x() < size) && (u.y() < size) && (u.z() < size));
}

// Is vector u inside a box defined by center point p, size in all directions
inline bool in(const Vector3D & u, const Vector3D & p, const double size)
{
    return ((u.x() < (p.x() + size)) && (u.x() > (p.x() - size)) &&
            (u.y() < (p.y() + size)) && (u.y() > (p.y() - size)) &&
            (u.z() < (p.z() + size)) && (u.z() > (p.z() - size)));
}

// Is a box defined by n, and f in collision with a box defined by on and of.
template<class P>
inline bool hit(const P& n, const P& f,
         const P& on, const P& of)
{
    return ((n.x() > on.x()) && (n.x() < of.x()) ||
            (f.x() > on.x()) && (f.x() < of.x()) &&
            (n.y() > on.y()) && (n.y() < of.y()) ||
            (f.y() > on.y()) && (f.y() < of.y()) &&
            (n.z() > on.z()) && (n.z() < of.z()) ||
            (f.z() > on.z()) && (f.z() < of.z()));
}

/** When is a box defined by n and f, travelling with velocity
    v going to hit box defined by on and of.
    If this function returns a -ve value, it is possible they are
    currently in a collided state, or they may never collide.
    Calculate range of times each intersect
 */
template<class P>
inline double timeToHit(const P & near, const P & far, const Vector3D &vel,
                        const P & onear, const P & ofar, int & axis)
{
    range xtime = timeToHit(near.x(), far.x(), vel.x(), onear.x(), ofar.x());
    range ytime = timeToHit(near.y(), far.y(), vel.y(), onear.y(), ofar.y());
    range ztime = timeToHit(near.z(), far.z(), vel.z(), onear.z(), ofar.z());
    // Find the time that the last coordinate starts intersect
    double start = std::max(min(xtime), std::max(min(ytime),min(ztime)));
    // Find the time that the first coordinate stops intersect
    double end   = std::min(max(xtime), std::min(max(ytime),max(ztime)));
    // If the start is before the end, then there is a collision
    if (end < start) { return -1; }
    axis = ((start == min(xtime)) ? cX : ((start == min(ytime)) ? cY : cZ));
    return start;
}

template<class P>
inline double timeToExit(const P & near, const P & far, const Vector3D &vel,
                         const P & onear, const P & ofar)
{
    range xtime = timeToHit(near.x(), far.x(), vel.x(), onear.x(), ofar.x());
    range ytime = timeToHit(near.y(), far.y(), vel.y(), onear.y(), ofar.y());
    range ztime = timeToHit(near.z(), far.z(), vel.z(), onear.z(), ofar.z());
    double leave = std::min(max(xtime), std::min(max(ytime), max(ztime)));
    double enter = std::max(min(xtime), std::max(min(ytime), min(ztime)));
    // This check is required to make sure we don't accidentally
    // get stuck in an entity outside its bbox.
    if (enter > 0) {
        // debug(std::cout<<"We are "<<enter<<" outside our containers bbox"
                       // << std::endl << std::flush;);
        return -1;
    }
    return leave;
}

inline bool isZero(const Vector3D & u)
{
    return (u.isValid() && (u.x() == 0) && (u.y() == 0) && (u.z() == 0));
}

typedef std::vector<Vector3D> CoordList;

#endif // PHYSICS_VECTOR_3D_H
