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
    Quaternion(const Vector3D & from, const Vector3D & to) : _set(true) {
        double cosT = from.dot(to);
        if (cosT > 0.99999f) {
            x = y = z = 0.0;
            w = 1.0;

            return;
        } else if (cosT < -0.99999f) {
            Vector3D t(0.0, from.X(), -from.Y());

            if (t.mag() < 1e-6) {
                t = Vector3D(-from.Z(), 0.0, from.X());
            }

            t = t.unitVector();

            x = t.X();
            y = t.Y();
            z = t.Z();
            w = 0.0;

            return;
        }
        Vector3D t = from.cross(to);

        t = t.unitVector();

        double ss = std::sqrt(0.5 * (1.0 * cosT));

        x = t.X() * ss;
        y = t.Y() * ss;
        z = t.Z() * ss;
        w = std::sqrt(0.5 * (1.0 + cosT));
    }

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
