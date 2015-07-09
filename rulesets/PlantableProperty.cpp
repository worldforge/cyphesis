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

#include "PlantableProperty.h"

#include "common/type_utils_impl.h"
#include "common/debug.h"

#include <wfmath/atlasconv.h>

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

PlantableProperty::PlantableProperty() :
        m_offset(0.f) {
}

int PlantableProperty::get(Element & ent) const {
    MapType map;
    map["offset"] = m_offset;
    if (m_orientation.isValid()) {
        map["orientation"] = m_orientation.toAtlas();
    }
    ent = map;
    return 0;
}

void PlantableProperty::set(const Element & ent) {
    if (ent.isMap()) {
        auto map = ent.Map();
        auto offsetI = map.find("offset");
        if (offsetI != map.end() && offsetI->second.isFloat()) {
            m_offset = offsetI->second.Float();
        } else {
            m_offset = 0;
        }

        auto orientationI = map.find("orientation");
        if (orientationI != map.end() && orientationI->second.isList()) {
            m_orientation = Quaternion(orientationI->second);
        } else {
            m_orientation = Quaternion();
        }
    }
}

PlantableProperty * PlantableProperty::copy() const {
    return new PlantableProperty(*this);
}

float PlantableProperty::getOffset() const
{
    return m_offset;
}

const Quaternion& PlantableProperty::getOrientation() const
{
    return m_orientation;
}

