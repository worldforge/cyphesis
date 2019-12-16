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

#include <LinearMath/btVector3.h>
#include <LinearMath/btQuaternion.h>
#include <cassert>
#include <ostream>

/**
 * Output operator for streams, to help with logging only.
 */
std::ostream& operator<<(std::ostream& os, const btVector3& m);

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
    return {static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z())};
}

inline btVector3 Convert::toBullet(const WFMath::Point<3>& p)
{
    assert(p.isValid() && "Never convert an invalid WFMath point into Bullet as there will only be pain on the other side.");
    return {static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z())};
}

inline btQuaternion Convert::toBullet(const WFMath::Quaternion& aq)
{
    assert(aq.isValid() && "Never convert an invalid WFMath quaternion into Bullet as there will only be pain on the other side.");
    return {static_cast<float>(aq.vector().x()), static_cast<float>(aq.vector().y()), static_cast<float>(aq.vector().z()), static_cast<float>(aq.scalar())};
}

template<>
inline WFMath::Point<3> Convert::toWF<WFMath::Point<3>>(const btVector3& p)
{
    assert(std::isfinite(p.x()));
    assert(std::isfinite(p.y()));
    assert(std::isfinite(p.z()));
    return WFMath::Point<3>(p.x(), p.y(), p.z());
}

template<>
inline WFMath::Vector<3> Convert::toWF<WFMath::Vector<3>>(const btVector3& p)
{
    assert(std::isfinite(p.x()));
    assert(std::isfinite(p.y()));
    assert(std::isfinite(p.z()));
    return WFMath::Vector<3>(p.x(), p.y(), p.z());
}

inline WFMath::Quaternion Convert::toWF(const btQuaternion& aq)
{
    assert(std::isfinite(aq.x()));
    assert(std::isfinite(aq.y()));
    assert(std::isfinite(aq.z()));
    assert(std::isfinite(aq.w()));
    return WFMath::Quaternion(aq.w(), aq.x(), aq.y(), aq.z());
}

inline std::ostream& operator<<(std::ostream& os, const btVector3& m)
{
  os << "x: " << m.x() << " y: " << m.y() << " z: " << m.z();
  return os;
}


#endif //PHYSICS_CONVERT_H
