// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef PHYSICS_QUATERNION_H
#define PHYSICS_QUATERNION_H

#include "Vector3D.h"

class Quaternion {
    double x, y, z, w;
    bool _set;
  public:
    Quaternion() : x(0), y(0), z(0), w(0), _set(false) { }
    Quaternion(double x, double y, double z, double w) : x(x), y(y), z(z), w(w),
                                                         _set(true) { }
    explicit Quaternion(const Atlas::Message::Object::ListType &l) : _set(true)
    {
        x = l[0].AsNum();
        y = l[1].AsNum();
        z = l[2].AsNum();
        w = l[3].AsNum();
    }
    Quaternion(const Vector3D & to, const Vector3D & from);

    double X() const { return x; }
    double Y() const { return y; }
    double Z() const { return z; }
    double W() const { return w; }

    operator bool() const {
        return _set;
    }

    const Atlas::Message::Object asObject() const {
        Atlas::Message::Object::ListType quat;
        quat.push_back(Object(x));
        quat.push_back(Object(y));
        quat.push_back(Object(z));
        quat.push_back(Object(w));
        return Atlas::Message::Object(quat);
    }

    const Atlas::Message::Object::ListType asList() const {
        Atlas::Message::Object::ListType quat;
        quat.push_back(Object(x));
        quat.push_back(Object(y));
        quat.push_back(Object(z));
        quat.push_back(Object(w));
        return quat;
    }
};

#endif // PHYSICS_QUATERNION_H
