// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef VECTOR_3D_H
#define VECTOR_3D_H

#include <Atlas/Message/Object.h>

#include <math.h>
#include <algo.h>

using std::cos;
using std::sin;

typedef std::pair<double, double> range;

using Atlas::Message::Object;

static inline range hitTime(double s, double r, double u, double p, double q) {
    return range((r - s + p + q)/u, (r - s - p - q)/u);
}

static inline range hitTime(double s, double r, double u, double v, double p, double q) {
    return range((r - s + p + q)/(u - v), (r - s - p - q)/(u - v));
}

static inline range inTime(double s, double r, double u, double p, double q) {
    return range((r - q + p - s)/u, (r + q - p - s)/u);
}

static inline double max(range r) {
    return max(r.first, r.second);
}

static inline double min(range r) {
    return min(r.first, r.second);
}

class Vector3D {
    double x,y,z;
    bool _set;
  public:

    Vector3D() : x(0), y(0), z(0), _set(false) { }
    Vector3D(double size) : x(size), y(size), z(size), _set(true) { }
    Vector3D(double x, double y, double z) : x(x), y(y), z(z), _set(true) { }
    Vector3D(const Object::ListType & vector) {
        Object::ListType::const_iterator I = vector.begin();
        x = I->AsNum(); I++;
        y = I->AsNum(); I++;
        z = I->AsNum();
    }

    double X() const { return x; }
    double Y() const { return y; }
    double Z() const { return z; }

    bool operator==(const Vector3D & other) const {
        //"Check if two vector are equal";
        return ((x==other.x) && (y==other.y) && (z==other.z));
    }

    Vector3D operator+(const Vector3D & other) const {
        // Add two vectors
        return Vector3D(x+other.x, y+other.y, z+other.z);
    }

    Vector3D operator+(double other) const {
        // Increaese size (in all direction for boxes)
        return Vector3D(x+other, y+other, z+other);
    }

    Vector3D operator-(const Vector3D & other) const {
        // Subtract two vectors
        return Vector3D(x-other.x, y-other.y, z-other.z);
    }

    Vector3D operator*(double other) const {
        // Multiply vector by number
        return Vector3D(x*other,y*other,z*other);
    }

    Vector3D operator/(double other) const {
        // Divide vector by number
        return Vector3D(x/other,y/other,z/other);
    }

    operator bool() const {
        return(_set);
    }

    double & operator[](int index) {
	switch(index) {
            case 0:
                return(x);
            case 1:
                return(y);
            case 2:
                return(z);
            default:
                //Throw an exception here maybe
                return(z);
	}
    }

    void set() {
        _set = true;
    }

    double dot(const Vector3D & v) const {
        //"Dot product of a vector";
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3D cross(const Vector3D & v) const {
        //"Cross product of a vector";
        return Vector3D(y*v.z - z*v.x, z*v.x - x*v.z, x*v.y - y*v.z);
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

    double angle(const Vector3D & v) const {
        //"Find the angle between two vectors";
        double d = v.x * x + v.y * y + v.z * z;
        return d / (v.mag() * mag());
    }

    double mag() const {
        //"Find the magnitude of a vector";
        return sqrt(x*x + y*y + z*z);
    }


    Vector3D unit_vector() const {
        //"return the unit vector of a vector";
	// This is could throw a wobbly
	return operator/(mag());
    }

    Vector3D unit_vector_to_another_vector(const Vector3D & v) const {
        // return the unit vector in the direction of another vector;
        Vector3D difference_vector = v - (*this);
        return difference_vector.unit_vector();
    }

    double distance(const Vector3D & v) const {
        //"Find the distance between two vectors";
        return sqrt((x - v.x)*(x - v.x) + (y - v.y)*(y - v.y) + (z - v.z)*(z - v.z));
    }

    bool inBox(const Vector3D & median, const Vector3D & size) const {
        // Is this vector inside a box of size centered around median
        return ((x < (median.x + size.x)) &&
                (x > (median.x - size.x)) &&
                (y < (median.y + size.y)) &&
                (y > (median.y - size.y)) &&
                (z < (median.z + size.z)) &&
                (z > (median.z - size.z)));
    }

    bool hitBox(const Vector3D & m, const Vector3D & s,
                const Vector3D & om, const Vector3D & os) const {
        // Is box defined by this vector, with median offset m of size s
        // colliding with other box of median om of size s.
        return (((x+m.x+s.x) < (om.x+s.x) && (x+m.x+s.x) > (om.x-s.x)) ||
                ((x+m.x-s.x) < (om.x+s.x) && (x+m.x-s.x) > (om.x-s.x)) &&
                ((y+m.y+s.y) < (om.y+s.y) && (y+m.y+s.y) > (om.y-s.y)) ||
                ((y+m.y-s.y) < (om.y+s.y) && (y+m.y-s.y) > (om.y-s.y)) &&
                ((z+m.z+s.z) < (om.z+s.z) && (z+m.z+s.z) > (om.z-s.z)) ||
                ((z+m.z-s.z) < (om.z+s.z) && (z+m.z-s.z) > (om.z-s.z)));
    }

    double hitTime(const Vector3D & m, const Vector3D & s, const Vector3D & v,
                   const Vector3D & om, const Vector3D & os) const {
        // When is box defined by this vector, with median offset m of size s
        // velocity v, colliding with other box of median om of size s.
        // If this function returns a -ve value, it is possible they are
        // currently in a collided state, or they may never collide.
        // On a PIII 700 this function can perform approx 1M collision
        // predictions per second, and approx 3M collision predictions per
        // second optimised.
        // Calculate range of times each intersect
        range xt = ::hitTime(x+m.x, om.x, v.x, s.x, os.x);
        range yt = ::hitTime(y+m.y, om.y, v.y, s.y, os.y);
        range zt = ::hitTime(z+m.z, om.z, v.z, s.z, os.z);
        // Find the time that the last coordinate starts intersect
        double start = max(min(xt), max(min(yt),min(zt)));
        // Find the time that the first coordinate stops intersect
        double end   = min(max(xt), min(max(yt),max(zt)));
        // If the start is before the end, then there is a collision
        if (end < start) { return -1; }
        return start;
    }

    double hitTime(const Vector3D & m, const Vector3D & s,
                   const Vector3D & v, const Vector3D & ov,
                   const Vector3D & om, const Vector3D & os) const {
        // This is the same as the above function but it accepts an
        // additional arguemnt, the velocity of the other entity
        // Calculate range of times each intersect
        range xt = ::hitTime(x+m.x, om.x, v.x, ov.x, s.x, os.x);
        range yt = ::hitTime(y+m.y, om.y, v.y, ov.y, s.y, os.y);
        range zt = ::hitTime(z+m.z, om.z, v.z, ov.z, s.z, os.z);
        // Find the time that the last coordinate starts intersect
        double start = max(min(xt), max(min(yt),min(zt)));
        // Find the time that the first coordinate stops intersect
        double end   = min(max(xt), min(max(yt),max(zt)));
        // If the start is before the end, then there is a collision
        if (end < start) { return -1; }
        return start;
    }

    double inTime(const Vector3D & m, const Vector3D & s, const Vector3D & v,
                   const Vector3D & om, const Vector3D & os) const {
        range xt = ::inTime(x+m.x, om.x, v.x, s.x, os.x);
        range yt = ::inTime(y+m.y, om.y, v.y, s.y, os.y);
        range zt = ::inTime(z+m.z, om.z, v.z, s.z, os.z);
        double enter = max(min(xt), max(min(yt),min(zt)));
        double leave = min(max(xt), min(max(yt),max(zt)));
        cout << "E" << enter << " L" << leave << endl << flush;
        if (enter > 0) { return -1; }
        return leave;
    }

    Object asObject() const {
        Object::ListType coords;
        coords.push_back(Object(x));
        coords.push_back(Object(y));
        coords.push_back(Object(z));
        return(Object(coords));
    }


    friend ostream & operator<<(ostream& s, const Vector3D& v);
};

inline ostream & operator<<(ostream& s, const Vector3D& v) {
    return s << "[" << v.x << "," << v.y << "," << v.z << "]";
}


#endif VECTOR_3D_H
