// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef PHYSICS_QUATERNION_H
#define PHYSICS_QUATERNION_H

#include <wfmath/quaternion.h>

typedef WFMath::Quaternion Quaternion;

template<class V>
const Quaternion quaternionFromTo(const V & from, const V & to);

#endif // PHYSICS_QUATERNION_H
