// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "LineProperty.h"

#include "common/type_utils.h"
#include "common/debug.h"

#include <wfmath/atlasconv.h>

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

LineProperty::LineProperty(CoordList & data, unsigned int flags) :
                           PropertyBase(flags), m_data(data)
{
}

void LineProperty::get(Element & ent)
{
    ent = ListType();
    objectListAsMessage(m_data, ent.asList());
}

void LineProperty::set(const Element & ent)
{
    if (ent.isList()) {
        objectListFromMessage<Point3D, CoordList>(ent.asList(), m_data);
    }
}

void LineProperty::add(const std::string & s, MapType & ent)
{
    if (!m_data.empty()) {
        get(ent[s]);
    }
}
