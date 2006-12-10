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

// $Id: StatisticsProperty.cpp,v 1.5 2006-12-10 22:48:02 alriddoch Exp $

#include "StatisticsProperty.h"

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
StatisticsProperty::StatisticsProperty(Statistics & data, unsigned int flags) :
                    PropertyBase(flags), m_data(data)
{
}

bool StatisticsProperty::get(Element & ent) const
{
    return false;
}

void StatisticsProperty::set(const Element & ent)
{
}
