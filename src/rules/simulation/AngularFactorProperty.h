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

#ifndef RULESETS_ANGULARFACTOR_PROPERTY_H
#define RULESETS_ANGULARFACTOR_PROPERTY_H

#include "common/Property.h"
#include <wfmath/vector.h>

/**
 * \brief Angular factor property determines how much the entity is allows angular movement.
 * It's normalized, with 0 being no movement, and 1 being full movement.
 * \ingroup PropertyClasses
 */
class AngularFactorProperty: public PropertyBase
{
    protected:
        AngularFactorProperty(const AngularFactorProperty& rhs) = default;
        WFMath::Vector<3> m_data;
    public:

        static constexpr const char* property_name = "angularfactor";
        static constexpr const char* property_atlastype = "list";

        AngularFactorProperty() = default;

        AngularFactorProperty * copy() const override;

        const WFMath::Vector<3> & data() const { return m_data; }
        WFMath::Vector<3> & data() { return m_data; }

        int get(Atlas::Message::Element & val) const override;

        void set(const Atlas::Message::Element & val) override;

        void add(const std::string & key, Atlas::Message::MapType & map) const override;

        void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const override;
};

#endif // RULESETS_ANGULARFACTOR_PROPERTY_H
