// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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

#include "ExternalProperty.h"

#include "common/Router.h"

#include <Atlas/Objects/RootEntity.h>

ExternalProperty::ExternalProperty(Router * & data) :
                  PropertyBase(0), m_data(data)
{
}

bool ExternalProperty::get(Atlas::Message::Element & val) const
{
    val = (m_data == 0) ? 0 : 1;
    return true;
}

void ExternalProperty::set(const Atlas::Message::Element & val)
{
}

void ExternalProperty::add(const std::string & s,
                         Atlas::Message::MapType & map) const
{
    map[s] = (m_data == 0) ? 0 : 1;
}

void ExternalProperty::add(const std::string & s,
                         const Atlas::Objects::Entity::RootEntity & ent) const
{
    ent->setAttr(s, (m_data == 0) ? 0 : 1);
}
