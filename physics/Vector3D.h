// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef PHYSICS_VECTOR_3D_H
#define PHYSICS_VECTOR_3D_H

#include <Atlas/Message/Object.h>

#include <cmath>
#include <algorithm>
#include <iostream>

using std::cos;
using std::sin;

typedef std::pair<double, double> range;

using Atlas::Message::Object;

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

class Vector3D {
    double x,y,z;
    bool _set;
  public:
    static const int cX = 0;	// Used to indicate which axis
    static const int cY = 1;
    static const int cZ = 2;

    Vector3D() : x(0), y(0), z(0), _set(false) { }
    explicit Vector3D(double size) : x(size), y(size), z(size), _set(true) { }
    Vector3D(double x, double y, double z) : x(x), y(y), z(z), _set(true) { }
    explicit Vector3D(const Object::ListType & vector) : _set(true) {
        Object::ListType::const_iterator I = vector.begin();
        x = I->AsNum();
        y = (++I)->AsNum();
        z = (++I)->AsNum();
    }

    double X() const { return x; }
    double Y() const { return y; }
    double Z() const { return z; }

    bool operator==(const Vector3D & other) const {
        //"Check if two vector are equal";
        return ((x==other.x) && (y==other.y) && (z==other.z));
    }

    const Vector3D operator+(const Vector3D & other) const {
        // Add two vectors
        return Vector3D(x+other.x, y+other.y, z+other.z);
    }

    Vector3D & operator+=(const Vector3D & other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    const Vector3D operator+(double other) const {
        // Increaese size (in all direction for boxes)
        return Vector3D(x+other, y+other, z+other);
    }

    Vector3D & operator+=(double other) {
        x += other;
        y += other;
        z += other;
        return *this;
    }

    const Vector3D operator-(const Vector3D & other) const {
        // Subtract two vectors
        return Vector3D(x-other.x, y-other.y, z-other.z);
    }

    Vector3D & operator-=(const Vector3D & other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    const Vector3D operator-(double other) const {
        // Decrease size (in all direction for boxes)
        return Vector3D(x-other, y-other, z-other);
    }

    Vector3D & operator-=(double other) {
        x -= other;
        y -= other;
        z -= other;
        return *this;
    }

    const Vector3D operator*(double other) const {
        // Multiply vector by number
        return Vector3D(x*other,y*other,z*other);
    }

    Vector3D & operator*=(double other) {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    const Vector3D operator/(double other) const {
        // Divide vector by number
        return Vector3D(x/other,y/other,z/other);
    }

    Vector3D & operator/=(double other) {
        x /= other;
        y /= other;
        z /= other;
        return *this;
    }

    operator bool() const {
        return _set;
    }

    double & operator[](int index) {
	switch(index) {
            case cX:
                return x;
            case cY:
                return y;
            case cZ:
                return z;
            default:
                //Throw an exception here maybe
                return z;
	}
    }

    void set() {
        _set = true;
    }

    double dot(const Vector3D & v) const {
        //"Dot product of a vector";
        return x * v.x + y * v.y + z * v.z;
    }

    const Vector3D cross(const Vector3D & v) const {
        //"Cross product of a vector";
        return Vector3D(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }

    void rotatex(double angle) {
        //"Rotate a vector around x axis by angle in radians" ;
        z = cos(angle) * z + sin(angle) * x;
        x = sin(angle) * z + cos(angle) * x;
    }

    void rotatey(double angle) {
        //"Rotate a vector around y axis by angle in radians"        ;
        y = cos(angle) * y + sin(angle) * z;
        z = sin(angle) * y + cos(angle) * z;
    }

    void rotatez(double angle) {
        //"Rotate a vector around z axis by angle in radians" ;
        x = cos(angle) * x + sin(angle) * y;
        y = sin(angle) * x + cos(angle) * y;
    }

    double mag() const {
        //"Find the magnitude of a vector";
        return sqrt(x*x + y*y + z*z);
    }

    double angle(const Vector3D & v) const {
        //"Find the angle between two vectors";
        double d = v.x * x + v.y * y + v.z * z;
        return d / (v.mag() * mag());
    }

    const Vector3D unitVector() const {
        //"return the unit vector of a vector";
	// This is could throw a wobbly
	return operator/(mag());
    }

    const Vector3D unitVectorTo(const Vector3D & v) const {
        // return the unit vector in the direction of another vector;
        Vector3D difference_vector = v - (*this);
        return difference_vector.unitVector();
    }

    double distance(const Vector3D & v) const {
        //"Find the distance between two vectors";
        return sqrt((x - v.x)*(x - v.x) + (y - v.y)*(y - v.y) + (z - v.z)*(z - v.z));
    }

    bool in(const Vector3D & n, const Vector3D & f) const {
        // Is this vector inside a box defined by near point n and point f
        return ((x < f.x) && (x > n.x) &&
                (x < f.x) && (x > n.x) &&
                (x < f.x) && (x > n.x));
    }

    bool hit(const Vector3D& f, const Vector3D& on, const Vector3D& of) const {
        // Is a box defined by this vector, and f in collision with a box
        // defined by on and of.
        return ((x > on.x) && (x < of.x) || (f.x > on.x) && (f.x < of.x) &&
                (y > on.y) && (y < of.y) || (f.y > on.y) && (f.y < of.y) &&
                (z > on.z) && (z < of.z) || (f.z > on.z) && (f.z < of.z));
    }

    double timeToHit(const Vector3D& f, const Vector3D &v,
                     const Vector3D& on, const Vector3D& of,
                     int & axis) const {
        // When is a box defined by this vector and f, travelling with velocity
        // v going to hit box defined by on and of.
        // If this function returns a -ve value, it is possible they are
        // currently in a collided state, or they may never collide.
        // Calculate range of times each intersect
        range xt = ::timeToHit(x, f.x, v.x, on.x, of.x);
        range yt = ::timeToHit(y, f.y, v.y, on.y, of.y);
        range zt = ::timeToHit(z, f.z, v.z, on.z, of.z);
        // Find the time that the last coordinate starts intersect
        double start = std::max(min(xt), std::max(min(yt),min(zt)));
        // Find the time that the first coordinate stops intersect
        double end   = std::min(max(xt), std::min(max(yt),max(zt)));
        // If the start is before the end, then there is a collision
        if (end < start) { return -1; }
        axis = ((start == min(xt)) ? cX : ((start == min(yt)) ? cY : cZ));
        return start;
    }

    double timeToExit(const Vector3D& f, const Vector3D &v,
                      const Vector3D& on, const Vector3D& of) const {
        range xt = ::timeToHit(x, f.x, v.x, on.x, of.x);
        range yt = ::timeToHit(y, f.y, v.y, on.y, of.y);
        range zt = ::timeToHit(z, f.z, v.z, on.z, of.z);
        double leave = std::min(max(xt), std::min(max(yt),max(zt)));
        double enter = std::max(min(xt), std::max(min(yt),min(zt)));
        // This check is required to make sure we don't accidentally
        // get stuck in an entity outside its bbox.
        if (enter > 0) {
            std::cout << "We are " << enter << " outside our containers bbox"
                      << std::endl << std::flush;
            return -1;
        }
        return leave;
    }

    const Object asObject() const {
        Object::ListType coords;
        coords.push_back(Object(x));
        coords.push_back(Object(y));
        coords.push_back(Object(z));
        return Object(coords);
    }

    const Object::ListType asList() const {
        Object::ListType coords;
        coords.push_back(Object(x));
        coords.push_back(Object(y));
        coords.push_back(Object(z));
        return coords;
    }

    friend std::ostream & operator<<(std::ostream& s, const Vector3D& v);
};

inline std::ostream & operator<<(std::ostream& s, const Vector3D& v) {
    return s << "[" << v.x << "," << v.y << "," << v.z << "]";
}


#endif // PHYSICS_VECTOR_3D_H
