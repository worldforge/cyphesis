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

#include "QuaternionProperty.h"

#include <wfmath/atlasconv.h>

QuaternionProperty::QuaternionProperty(const QuaternionProperty& rhs) = default;

int QuaternionProperty::get(Atlas::Message::Element& val) const
{
    if (m_data.isValid()) {
        val = m_data.toAtlas();
        return 0;
    }
    return 1;

}

void QuaternionProperty::set(const Atlas::Message::Element& val)
{
    if (val.isList()) {
        m_data.fromAtlas(val.List());
    }
}

QuaternionProperty* QuaternionProperty::copy() const
{
    return new QuaternionProperty(*this);
}

void QuaternionProperty::add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const
{
    if (m_data.isValid()) {
        PropertyBase::add(key, ent);
    }
}

