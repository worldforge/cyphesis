// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "StatisticsProperty.h"

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

StatisticsProperty::StatisticsProperty(Statistics & data, unsigned int flags) :
                    PropertyBase(flags), m_data(data)
{
}

void StatisticsProperty::get(Element & ent) const
{
}

void StatisticsProperty::set(const Element & ent)
{
}
