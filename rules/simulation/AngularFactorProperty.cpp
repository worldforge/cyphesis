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

#include "AngularFactorProperty.h"

#include "rules/LocatedEntity.h"

#include "common/debug.h"
#include <Atlas/Objects/RootEntity.h>

#include <wfmath/atlasconv.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;


static const bool debug_flag = false;

int AngularFactorProperty::get(Element & val) const
{
    if (m_data.isValid()) {
        val = m_data.toAtlas();
        return 0;
    }
    return -1;
}

void AngularFactorProperty::set(const Element & val)
{
    try {
        m_data.fromAtlas(val.asList());
    }
    catch (Atlas::Message::WrongTypeException &) {
        log(ERROR, "BBoxProperty::set: Box bbox data");
    }
}

void AngularFactorProperty::add(const std::string & key,
                       MapType & map) const
{
    if (m_data.isValid()) {
        map[key] = m_data.toAtlas();
    }
}

void AngularFactorProperty::add(const std::string & key,
                       const RootEntity & ent) const
{
    if (m_data.isValid()) {
        ent->setAttr(key, m_data.toAtlas());
    }
}


AngularFactorProperty * AngularFactorProperty::copy() const
{
    return new AngularFactorProperty(*this);
}

