// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef PHYSICS_BBOX_H
#define PHYSICS_BBOX_H

#include "Vector3D.h"

class BBox {
    Vector3D u;
    Vector3D v;
  public:
    BBox() { }
    explicit BBox(const Vector3D & f) : u(0,0,0), v(f) { }
    BBox(const Vector3D & n, const Vector3D & f) : u(n), v(f) { }
    explicit BBox(const Atlas::Message::Object::ListType & l) {
        if (l.size() == 6) {
            u = Vector3D(l);
            v = Vector3D(l[3].AsNum(), l[4].AsNum(), l[5].AsNum());
        } else {
            u = Vector3D(0,0,0);
            v = Vector3D(l);
        }
    }

    const Vector3D & nearPoint() const { return u; }
    Vector3D & nearPoint() { return u; }
    const Vector3D & farPoint() const { return u; }
    Vector3D & farPoint() { return u; }

    bool operator ==(const BBox & other) const {
        return ((u==other.u) && (v==other.v));
    }

    bool isValid() const {
        return v.isValid();
    }

    const BBox operator+(const Vector3D & other) const {
        return BBox(u + other, v + other);
    }

    BBox & operator+=(const Vector3D & other) {
        u += other;
        v += other;
        return *this;
    }

    bool contains(const Vector3D & o) const {
        // Is point o inside this box
        return ((o.X() > u.X()) && (o.X() < v.X()) &&
                (o.Y() > u.Y()) && (o.Y() < v.Y()) &&
                (o.Z() > u.Z()) && (o.Z() < v.Z()));
    }

    bool contains(const Vector3D & o, double increase) const {
        // Is point o inside this box, when this box is increased in each
        // direction
        return ((o.X() > (u.X() - increase)) && (o.X() < (v.X() + increase)) &&
                (o.Y() > (u.Y() - increase)) && (o.Y() < (v.Y() + increase)) &&
                (o.Z() > (u.Z() - increase)) && (o.Z() < (v.Z() + increase)));
    }

    const BBox grow(double o) const {
        return BBox(u - o, v + o);
    }

    BBox & grow(double o) {
        u -= o;
        v += o;
        return *this;
    }

    bool hit(const BBox & o) const {
        return u.hit(v, o.u, o.v);
    }

    double timeToHit(const Vector3D & vel, const BBox & o, int & axis) const {
        return u.timeToHit(v, vel, o.u, o.v, axis);
    }

    double timeToExit(const Vector3D & vel, const BBox & o) const {
        return u.timeToExit(v, vel, o.u, o.v);
    }

    const Atlas::Message::Object::ListType asList() const {
        if (!u.isZero()) {
            Atlas::Message::Object::ListType box = u.asList();
            box.push_back(v.X());
            box.push_back(v.Y());
            box.push_back(v.Z());
            return box;
        } else {
            Atlas::Message::Object::ListType box = v.asList();
            return v.asList();
        }
    }

    friend std::ostream & operator<<(std::ostream& s, const BBox & v);
};

inline std::ostream & operator<<(std::ostream& s, const BBox & b)
{
    return s << "{" << b.u << b.v << "}";
}

#endif // PHYSICS_BBOX_H
