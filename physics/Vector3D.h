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

#if 1
typedef WFMath::Vector<3> Vector3D;
#else
class Vector3D {
    double _x, _y, _z;
    bool _set;
  public:
    static const int cX = 0;    // Used to indicate which axis
    static const int cY = 1;
    static const int cZ = 2;

    Vector3D() : _x(0), _y(0), _z(0), _set(false) { }
    explicit Vector3D(double size) : _x(size), _y(size), _z(size), _set(true) { }
    Vector3D(double x, double y, double z) : _x(x), _y(y), _z(z), _set(true) { }
    explicit Vector3D(const Atlas::Message::Element::ListType&vector):_set(true){
        Atlas::Message::Element::ListType::const_iterator I = vector.begin();
        _x = I->asNum();
        _y = (++I)->asNum();
        _z = (++I)->asNum();
    }

    double & x() { return _x; }
    double & y() { return _y; }
    double & z() { return _z; }

    double x() const { return _x; }
    double y() const { return _y; }
    double z() const { return _z; }

    bool operator==(const Vector3D & other) const {
        //"Check if two vector are equal";
        return ((_x==other._x) && (_y==other._y) && (_z==other._z));
    }

    bool operator!=(const Vector3D & other) const {
        //"Check if two vector are equal";
        return ((_x!=other._x) || (_y!=other._y) || (_z!=other._z));
    }

    Vector3D & operator+=(const Vector3D & other) {
        _x += other._x;
        _y += other._y;
        _z += other._z;
        return *this;
    }

    const Vector3D operator+(double other) const {
        // Increaese size (in all direction for boxes)
        return Vector3D(_x + other, _y + other, _z + other);
    }

    Vector3D & operator+=(double other) {
        _x += other;
        _y += other;
        _z += other;
        return *this;
    }

    Vector3D & operator-=(const Vector3D & other) {
        _x -= other._x;
        _y -= other._y;
        _z -= other._z;
        return *this;
    }

    const Vector3D operator-(double other) const {
        // Decrease size (in all direction for boxes)
        return Vector3D(_x - other, _y - other, _z - other);
    }

    Vector3D & operator-=(double other) {
        _x -= other;
        _y -= other;
        _z -= other;
        return *this;
    }

    const Vector3D operator*(double other) const {
        // Multiply vector by number
        return Vector3D(_x * other, _y * other, _z * other);
    }

    Vector3D & operator*=(double other) {
        _x *= other;
        _y *= other;
        _z *= other;
        return *this;
    }

    const Vector3D operator/(double other) const {
        // Divide vector by number
        return Vector3D(_x / other, _y / other, _z / other);
    }

    Vector3D & operator/=(double other) {
        _x /= other;
        _y /= other;
        _z /= other;
        return *this;
    }

    bool isValid() const {
        return _set;
    }

    bool isZero() const {
        return (_set && (_x == 0) && (_y == 0) && (_z == 0));
    }

    double & operator[](int index) {
        switch(index) {
            case cX:
                return _x;
            case cY:
                return _y;
            case cZ:
                return _z;
            default:
                //Throw an exception here maybe
                return _z;
        }
    }

    void set() {
        _set = true;
    }

    double dot(const Vector3D & v) const {
        //"Dot product of a vector";
        return _x * v._x + _y * v._y + _z * v._z;
    }

    const Vector3D cross(const Vector3D & v) const {
        //"Cross product of a vector";
        return Vector3D(_y * v._z - _z * v._y,
                        _z * v._x - _x * v._z,
                        _x * v._y - _y * v._x);
    }

    void rotateX(double angle) {
        //"Rotate a vector around x axis by angle in radians" ;
        _z = cos(angle) * _z + sin(angle) * _x;
        _x = sin(angle) * _z + cos(angle) * _x;
    }

    void rotateY(double angle) {
        //"Rotate a vector around y axis by angle in radians"        ;
        _y = cos(angle) * _y + sin(angle) * _z;
        _z = sin(angle) * _y + cos(angle) * _z;
    }

    void rotateZ(double angle) {
        //"Rotate a vector around z axis by angle in radians" ;
        _x = cos(angle) * _x + sin(angle) * _y;
        _y = sin(angle) * _x + cos(angle) * _y;
    }

    double mag() const {
        //"Find the magnitude of a vector";
        return sqrt(_x * _x + _y * _y + _z * _z);
    }

    double sqrMag() const {
        //"Find relative magnitude. This value is cheaper to calculate
        // than the real magnitude, so should be used for comparison.
        return (_x * _x + _y * _y + _z * _z);
    }

    double angle(const Vector3D & v) const {
        //"Find the angle between two vectors";
        double d = v._x * _x + v._y * _y + v._z * _z;
        return d / (v.mag() * mag());
    }

    Vector3D & normalize() {
        // Make this a unit vector
        double m = mag();
        _x /= m; _y /= m; _z /= m;
        return *this;
    }

    const Vector3D unitVector() const {
        //"return the unit vector of a vector";
        // This is could throw a wobbly
        return Vector3D(*this)/=(mag());
    }

    const Atlas::Message::Element toAtlas() const {
        Atlas::Message::Element::ListType coords;
        coords.push_back(Atlas::Message::Element(_x));
        coords.push_back(Atlas::Message::Element(_y));
        coords.push_back(Atlas::Message::Element(_z));
        return Atlas::Message::Element(coords);
    }

    const Atlas::Message::Element::ListType asList() const {
        Atlas::Message::Element::ListType coords;
        coords.push_back(Atlas::Message::Element(_x));
        coords.push_back(Atlas::Message::Element(_y));
        coords.push_back(Atlas::Message::Element(_z));
        return coords;
    }

    friend const Vector3D operator+(const Vector3D&, const Vector3D&);
    friend const Vector3D operator-(const Vector3D&, const Vector3D&);
    friend std::ostream & operator<<(std::ostream& s, const Vector3D& v);
};

inline const Vector3D operator+(const Vector3D & lhs, const Vector3D & rhs) {
    // Add two vectors
    return Vector3D(lhs._x + rhs._x, lhs._y + rhs._y, lhs._z + rhs._z);
}

inline const Vector3D operator-(const Vector3D & lhs, const Vector3D & rhs) {
    // Subtract two vectors
    return Vector3D(lhs._x - rhs._x, lhs._y - rhs._y, lhs._z - rhs._z);
}

inline std::ostream & operator<<(std::ostream& s, const Vector3D& v) {
    return s << "[" << v._x << "," << v._y << "," << v._z << "]";
}

inline double Dot(const Vector3D & u, const Vector3D & v)
{
    //"Dot product of a vector";
    return u.x() * v.x() + u.y() * v.y() + u.z() * v.z();
}

inline const Vector3D Cross(const Vector3D & u, const Vector3D & v)
{
    //"Cross product of a vector";
    return Vector3D(u.y() * v.z() - u.z() * v.y(),
                    u.z() * v.x() - u.x() * v.z(),
                    u.x() * v.y() - u.y() * v.x());
}

#endif // USE_WFMATH

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

// Next generation collision prediction for oriented boxes. This function
// predicts a collision between a point and a plane, both moving.

inline
bool timeToHit(const Vector3D & p,     // Position of point
               const Vector3D & u,     // Velocity of point
               // double point_time,   // Time since position set
               const Vector3D & l,     // Position on plane
               const Vector3D & n,     // Plane normal
               const Vector3D & v,     // Velocity of plane
               // double plane_time,   // Time since position set
               double time,            // Collision time return
               Vector3D & normal)      // Collision normal return
//
//
//                point_vel   |     \ plane_normal
//                     ___    |    __\ plane_vel
//               point \      |      / plane
//                      \     |     /
//                       \    |    /
//                        \   |   /
//                         \  |  /
//                          \ | /
//  _________________________\|/___________________________
//
//  The time when point hits plane is as follows:
//
//  ( (point + point_vel * t) - (plane + plane_vel * t) ) . plane_normal = 0
//
//  dot product ( . ) is x*x + y*y + z*z
//
//  (p.x + u.x * t - l.x - v.x * t) * n.x +
//  (p.y + u.y * t - l.y - v.y * t) * n.y +
//  (p.z + u.z * t - l.z - v.z * t) * n.z = 0
//
//  p.x * n.x + u.x * n.x * t - l.x * n.x - v.x * n.x * t +
//  p.y * n.y + u.y * n.y * t - l.y * n.y - v.y * n.y * t +
//  p.z * n.z + u.z * n.z * t - l.z * n.z - v.z * n.z * t = 0
//
//
// ( v.x * n.x + v.y * n.y + v.z * n.z - u.x * n.x - u.y * n.y - u.z * n.z ) * t
// = ( p.x * n.x - l.x * n.x + p.y * n.y - l.y * n.y + p.z * n.z - l.z * n.z )
//
// t =
// ( p.x * n.x - l.x * n.x + p.y * n.y - l.y * n.y + p.z * n.z - l.z * n.z ) /
// ( v.x * n.x + v.y * n.y + v.z * n.z - u.x * n.x - u.y * n.y - u.z * n.z )
//
// return value should indicate whether we are infront of or behind the 
// plane. There is math in common, but I'm not sure how much it will help
{
    return 0.;
}

inline bool isZero(const Vector3D & u)
{
    return (u.isValid() && (u.x() == 0) && (u.y() == 0) && (u.z() == 0));
}

typedef std::vector<Vector3D> CoordList;

#endif // PHYSICS_VECTOR_3D_H
