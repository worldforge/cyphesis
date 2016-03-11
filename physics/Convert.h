// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2016 Erik Ogenvik
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

#ifndef PHYSICS_CONVERT_H
#define PHYSICS_CONVERT_H

#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>
#include <wfmath/const.h>

#include <bullet/LinearMath/btVector3.h>
#include <bullet/LinearMath/btQuaternion.h>
#include <cassert>

class Convert {
    public:
        static btVector3 toBullet(const WFMath::Vector<3>& p);
        static btVector3 toBullet(const WFMath::Point<3>& p);

        static btQuaternion toBullet(const WFMath::Quaternion& aq);

        template<typename T>
        static T toWF(const btVector3& p);
        static WFMath::Quaternion toWF(const btQuaternion& aq);

};

///Implementations

inline btVector3 Convert::toBullet(const WFMath::Vector<3>& p)
{
    assert(p.isValid() && "Never convert an invalid WFMath point into Bullet as there will only be pain on the other side.");
    return btVector3(p.x(), p.z(), -p.y());
}

inline btVector3 Convert::toBullet(const WFMath::Point<3>& p)
{
    assert(p.isValid() && "Never convert an invalid WFMath point into Bullet as there will only be pain on the other side.");
    return btVector3(p.x(), p.z(), -p.y());
}

inline btQuaternion Convert::toBullet(const WFMath::Quaternion& aq)
{
    assert(aq.isValid() && "Never convert an invalid WFMath quaternion into Bullet as there will only be pain on the other side.");
    return btQuaternion(aq.scalar(), aq.vector().x(), aq.vector().z(), -aq.vector().y());
}

template<>
inline WFMath::Point<3> Convert::toWF<WFMath::Point<3>>(const btVector3& p)
{
    return WFMath::Point<3>(p.x(), -p.z(), p.y());
}

template<>
inline WFMath::Vector<3> Convert::toWF<WFMath::Vector<3>>(const btVector3& p)
{
    return WFMath::Vector<3>(p.x(), -p.z(), p.y());
}

inline WFMath::Quaternion Convert::toWF(const btQuaternion& aq)
{
    return WFMath::Quaternion(aq.w(), aq.x(), -aq.z(), aq.y());
}

#endif //PHYSICS_CONVERT_H
