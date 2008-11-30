// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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

#include <Python.h>

#include "StatisticsProperty.h"

#include "rulesets/ArithmeticScript.h"
#include "rulesets/ArithmeticFactory.h"

#include "common/log.h"

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

/// \brief StatisticsProperty constructor
///
/// @param data variable that holds the Property value
/// @param flags flags to indicate how this property is stored
StatisticsProperty::StatisticsProperty() : PropertyBase(0), m_script(0)
{
}

void StatisticsProperty::install(Entity * ent)
{
}

void StatisticsProperty::apply(Entity * ent)
{
    if (m_script == 0) {
        Entity * instance = 0;
        if (flags() & flag_class) {
        } else {
            instance = ent;
        }

        PythonArithmeticFactory paf("world.statistics.Statistics", "Statistics");
        m_script = paf.newScript(instance);
    }
    std::map<std::string, double>::const_iterator I = m_data.begin();
    std::map<std::string, double>::const_iterator Iend = m_data.end();
    for (; I != Iend; ++I) {
        m_script->set(I->first, I->second);
    }
}

bool StatisticsProperty::get(Element & val) const
{
    val = MapType();
    MapType & val_map = val.Map();

    std::map<std::string, double>::const_iterator I = m_data.begin();
    std::map<std::string, double>::const_iterator Iend = m_data.end();
    for (; I != Iend; ++I) {
        val_map[I->first] = I->second;
    }
    return true;
}

void StatisticsProperty::set(const Element & ent)
{
    if (!ent.isMap()) {
        log(WARNING, "Non map statistics");
        return;
    }
    const MapType & smap = ent.Map();
    MapType::const_iterator I = smap.begin();
    MapType::const_iterator Iend = smap.end();
    for (; I != Iend; ++I) {
        if (!I->second.isNum()) {
            log(WARNING, "Non numeric stat");
            continue;
        }
        m_data[I->first] = I->second.asNum();
    }
}
