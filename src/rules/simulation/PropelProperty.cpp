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

#include "PropelProperty.h"
#include "common/log.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

PropelProperty::PropelProperty() : PropertyBase(prop_flag_persistence_ephem)
{
}

int PropelProperty::get(Element& val) const
{
    if (mData.isValid()) {
        val = mData.toAtlas();
        return 0;
    }
    return -1;
}

void PropelProperty::set(const Element& val)
{
    if (val.isList()) {
        try {
            mData.fromAtlas(val.List());
        } catch (...) {
            mData = {};
            log(ERROR, "PropelProperty::set: Data was not in format which could be parsed into 3d vector.");
        }
    } else if (val.isNone()) {
        mData = {};
    } else {
        mData = {};
        log(ERROR, "PropelProperty::set: Data was not in format which could be parsed into 3d vector.");
    }

}

void PropelProperty::add(const std::string& key, MapType& map) const
{
    if (mData.isValid()) {
        map[key] = mData.toAtlas();
    }
}

void PropelProperty::add(const std::string& key, const RootEntity& ent) const
{
    if (mData.isValid()) {
        ent->setAttr(key, mData.toAtlas());
    }
}

PropelProperty* PropelProperty::copy() const
{
    return new PropelProperty(*this);
}

