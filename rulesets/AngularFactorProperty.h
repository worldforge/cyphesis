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
 * Angular factor property determines how much the entity is allows angular movement.
 * It's normalized, with 0 being no movement, and 1 being full movement.
 */
class AngularFactorProperty: public PropertyBase
{
    protected:
        WFMath::Vector<3> m_data;
    public:

        static const std::string property_name;
        static const std::string property_atlastype;

        virtual AngularFactorProperty * copy() const;

        const WFMath::Vector<3> & data() const { return m_data; }
        WFMath::Vector<3> & data() { return m_data; }

        virtual int get(Atlas::Message::Element & val) const;
        virtual void set(const Atlas::Message::Element & val);
        virtual void add(const std::string & key, Atlas::Message::MapType & map) const;
        virtual void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const;
};

#endif // RULESETS_ANGULARFACTOR_PROPERTY_H
