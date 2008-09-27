// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

// $Id$

#include "AreaProperty.h"

#include "common/type_utils.h"
#include "common/debug.h"

#include <wfmath/atlasconv.h>

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;

/// \brief AreaProperty constructor
///
/// @param flags Flags used to persist this property
AreaProperty::AreaProperty(unsigned int flags) : PropertyBase(flags)
{
}

bool AreaProperty::get(Element & ent) const
{
    MapType & area = (ent = MapType()).Map();
    m_line.get(area["points"]);
    return true;
}

void AreaProperty::set(const Element & ent)
{
    if (ent.isMap()) {
        const MapType & area = ent.Map();
        MapType::const_iterator I = area.find("points");
        if (I != area.end()) {
            m_line.set(I->second);
        }
    }
}

void AreaProperty::add(const std::string & s, MapType & ent) const
{
    get(ent[s]);
}
