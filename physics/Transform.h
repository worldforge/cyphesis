// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2015 Erik Ogenvik
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

#ifndef PHYSICS_TRANSFORM_H_
#define PHYSICS_TRANSFORM_H_

#include <wfmath/vector.h>
#include <wfmath/quaternion.h>
#include <Atlas/Message/Element.h>

/**
 * Represents a common transformation, affecting both position and orientation.
 */
class Transform {
    public:

        /**
         * A translation.
         */
        WFMath::Vector<3> translate;

        /**
         * A rotation.
         */
        WFMath::Quaternion rotate;

        /**
         * A translation dependent on the size of the entity.
         * The values are normalized.
         */
        WFMath::Vector<3> translateScaled;

        int get(Atlas::Message::MapType &) const;
        void set(const Atlas::Message::Element &);

};

#endif /* PHYSICS_TRANSFORM_H_ */
