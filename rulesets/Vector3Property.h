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

#ifndef RULESETS_VECTOR3PROPERTY_H_
#define RULESETS_VECTOR3PROPERTY_H_

#include "common/Property.h"
#include <wfmath/vector.h>

/**
 * \brief A property containing a WFMath::Vector<3>.
 *
 * \ingroup PropertyClasses
 */
class Vector3Property: public PropertyBase
{
    public:

        static const std::string property_atlastype;

        explicit Vector3Property();
        explicit Vector3Property(const Vector3Property& rhs);
        virtual ~Vector3Property();

        virtual int get(Atlas::Message::Element & val) const;
        virtual void set(const Atlas::Message::Element &);

        const WFMath::Vector<3>& data() const { return m_data; }
        WFMath::Vector<3>& data() { return m_data; }

        virtual Vector3Property * copy() const;
    protected:
        WFMath::Vector<3> m_data;
};

#endif /* RULESETS_VECTOR3PROPERTY_H_ */
