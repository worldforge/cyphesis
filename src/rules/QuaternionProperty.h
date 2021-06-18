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

#ifndef RULESETS_QUATERNIONPROPERTY_H_
#define RULESETS_QUATERNIONPROPERTY_H_

#include "common/Property.h"
#include <wfmath/quaternion.h>

/**
 * \brief A property containing a WFMath::Quaternion.
 *
 * \ingroup PropertyClasses
 */
class QuaternionProperty: public PropertyBase
{
    public:

        static constexpr const char* property_atlastype = "list";

        explicit QuaternionProperty() = default;

        QuaternionProperty(const QuaternionProperty& rhs);

        ~QuaternionProperty() override = default;

        int get(Atlas::Message::Element & val) const override;

        void set(const Atlas::Message::Element &) override;

        void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const override;

        const WFMath::Quaternion& data() const { return m_data; }
        WFMath::Quaternion& data() { return m_data; }

        QuaternionProperty * copy() const override;
    protected:
        WFMath::Quaternion m_data;
};

#endif /* RULESETS_QUATERNIONPROPERTY_H_ */
