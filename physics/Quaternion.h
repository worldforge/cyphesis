// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef PHYSICS_QUATERNION_H
#define PHYSICS_QUATERNION_H

#include <wfmath/quaternion.h>

typedef WFMath::Quaternion Quaternion;

#if 0
class Quaternion : public WFMath::Quaternion {
    double x, y, z, w;
    bool _set;
  public:
    static const int cX = 0;    // Used to indicate which axis
    static const int cY = 1;
    static const int cZ = 2;
    static const int cW = 3;

    Quaternion() : x(0), y(0), z(0), w(1), _set(false) { }
    Quaternion(double x, double y, double z, double w) : x(x), y(y), z(z), w(w),
                                                         _set(true) { }
    explicit Quaternion(const Atlas::Message::Element::ListType &l) : _set(true)
    {
        x = l[0].asNum();
        y = l[1].asNum();
        z = l[2].asNum();
        w = l[3].asNum();
    }
    Quaternion(const Vector3D & from, const Vector3D & to);

    double & X() { return x; }
    double & Y() { return y; }
    double & Z() { return z; }
    double & W() { return w; }

    double X() const { return x; }
    double Y() const { return y; }
    double Z() const { return z; }
    double W() const { return w; }

    double & operator[](int index) {
        switch(index) {
            case cX:
                return x;
            case cY:
                return y;
            case cZ:
                return z;
            case cW:
                return w;
            default:
                //Throw an exception here maybe
                return z;
        }
    }

    bool operator==(const Quaternion & other) const {
        //"Check if two vector are equal";
        return ((x==other.x) && (y==other.y) && (z==other.z) && (w==other.w));
    }

    bool operator!=(const Quaternion & other) const {
        //"Check if two vector are equal";
        return ((x!=other.x) || (y!=other.y) || (z!=other.z) || (w!=other.w));
    }

    void set() {
        _set = true;
    }

    bool isValid() const {
        return _set;
    }

    const Atlas::Message::Element asObject() const {
        Atlas::Message::Element::ListType quat;
        quat.push_back(Atlas::Message::Element(x));
        quat.push_back(Atlas::Message::Element(y));
        quat.push_back(Atlas::Message::Element(z));
        quat.push_back(Atlas::Message::Element(w));
        return Atlas::Message::Element(quat);
    }

    const Atlas::Message::Element::ListType asList() const {
        Atlas::Message::Element::ListType quat;
        quat.push_back(Atlas::Message::Element(x));
        quat.push_back(Atlas::Message::Element(y));
        quat.push_back(Atlas::Message::Element(z));
        quat.push_back(Atlas::Message::Element(w));
        return quat;
    }

    friend std::ostream & operator<<(std::ostream& s, const Quaternion& q);
};

inline std::ostream & operator<<(std::ostream& s, const Quaternion& q) {
    return s << "[" << q.x << "," << q.y << "," << q.z << "," << q.w << "]";
}

#endif // 0

#include <physics/Vector3D.h>

template<class V>
const Quaternion quaternionFromTo(const V & from, const V & to);

#endif // PHYSICS_QUATERNION_H
