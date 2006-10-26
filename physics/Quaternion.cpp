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

// $Id: Quaternion.cpp,v 1.8 2006-10-26 00:48:07 alriddoch Exp $

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
