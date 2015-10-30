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


#ifndef PHYSICS_QUATERNION_H
#define PHYSICS_QUATERNION_H

#include <wfmath/const.h>

typedef WFMath::Quaternion Quaternion;

/**
 * @brief Creates a quaternion for rotation from one vector to another.
 * @param from The from vector.
 * @param to The to vector.
 * @param fallbackAxis Rotate 180 degrees around this axis if the vectors are anti-parallel.
 * @return The required rotation.
 */
template<class V>
const Quaternion quaternionFromTo(const V & from, const V & to, const V& fallbackAxis);

#endif // PHYSICS_QUATERNION_H
