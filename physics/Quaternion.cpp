// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Quaternion.h"

#include "Vector3D.h"

// The arguments to this function have been swapped over because in
// the form provided in the example code, the result appeared to be
// a rotation in the wrong direction. This may be a bug in my apogee
// code, or it may be this code.

template<class V>
const Quaternion quaternionFromTo(const V & from, const V & to)
{
    float cosT = Dot(from, to);
    if (cosT > 0.99999f) {
        return Quaternion(1.f, 0.f, 0.f, 0.f);
    } else if (cosT < -0.99999f) {
        V t(0.0, from.x(), -from.y());

        if (t.sqrMag() < 1e-12) {
            t = V(-from.z(), 0.0, from.x());
        }

        t.normalize();

        return Quaternion(0.f, t.x(), t.y(), t.z());
    }
    V t = Cross(from, to);

    t.normalize();

    float ss = sqrtf(0.5 * (1.0 - cosT));

    float x = t.x() * ss;
    float y = t.y() * ss;
    float z = t.z() * ss;
    float w = std::sqrt(0.5 * (1.0 + cosT));
    return Quaternion(w, x, y, z);
}

template
const Quaternion quaternionFromTo<Vector3D>(const Vector3D &, const Vector3D&);
