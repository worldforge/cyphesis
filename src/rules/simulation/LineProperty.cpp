// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004-2006 Alistair Riddoch
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


#include "LineProperty.h"

#include "common/type_utils_impl.h"

#include <wfmath/atlasconv.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

LineProperty::LineProperty() = default;

int LineProperty::get(Element & ent) const
{
    ent = ListType();
    objectListAsMessage(m_data, ent.asList());
    return 0;
}

void LineProperty::set(const Element & ent)
{
    if (ent.isList()) {
        objectListFromMessage<Point3D, CoordList>(ent.asList(), m_data);
    }
}

void LineProperty::add(const std::string & s, MapType & ent) const
{
    if (!m_data.empty()) {
        get(ent[s]);
    }
}

LineProperty * LineProperty::copy() const
{
    return new LineProperty(*this);
}
