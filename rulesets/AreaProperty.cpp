// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "AreaProperty.h"

#include "common/type_utils.h"
#include "common/debug.h"

#include <wfmath/atlasconv.h>

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;

AreaProperty::AreaProperty(unsigned int flags) : PropertyBase(flags),
                                                 m_line(*new CoordList, flags)
{
}

void AreaProperty::get(Element & ent)
{
    MapType & area = (ent = MapType()).asMap();
    m_line.get(area["points"]);
}

void AreaProperty::set(const Element & ent)
{
    if (ent.isMap()) {
        const MapType & area = ent.asMap();
        MapType::const_iterator I = area.find("points");
        if (I != area.end()) {
            m_line.set(I->second);
        }
    }
}

void AreaProperty::add(const std::string & s, MapType & ent)
{
    get(ent[s]);
}
