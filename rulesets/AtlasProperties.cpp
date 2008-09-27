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

#include "AtlasProperties.h"

#include "LocatedEntity.h"

#include "common/type_utils.h"
#include "common/debug.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

IdProperty::IdProperty(const std::string & data) : PropertyBase(per_ephem),
                                                   m_data(data)
{
}

bool IdProperty::get(Atlas::Message::Element & e) const
{
    e = m_data;
    return true;
}

void IdProperty::set(const Atlas::Message::Element & e)
{
}

void IdProperty::add(const std::string & key,
                     Atlas::Message::MapType & ent) const
{
    ent[key] = m_data;
}

void IdProperty::add(const std::string & key, const RootEntity & ent) const
{
    ent->setId(m_data);
}

NameProperty::NameProperty(unsigned int flags) : Property<std::string>(flags)
{
}

void NameProperty::add(const std::string & s, const RootEntity & ent) const
{
    ent->setName(m_data);
}

ContainsProperty::ContainsProperty(LocatedEntitySet & data) :
      PropertyBase(per_ephem), m_data(data)
{
}

bool ContainsProperty::get(Element & e) const
{
    // FIXME Not sure if this is best. Why did we bother to virtualise
    // addToMessage() if we have to do this here?
    e = ListType();
    ListType & contlist = e.List();
    LocatedEntitySet::const_iterator Iend = m_data.end();
    for (LocatedEntitySet::const_iterator I = m_data.begin(); I != Iend; ++I) {
        contlist.push_back((*I)->getId());
    }
    return true;
}

void ContainsProperty::set(const Element & e)
{
}

// We do not implement add, as it is probably not going to be used.

void ContainsProperty::add(const std::string & s, const RootEntity & ent) const
{
    std::list<std::string> & contains = ent->modifyContains();
    contains.clear();
    LocatedEntitySet::const_iterator Iend = m_data.end();
    for (LocatedEntitySet::const_iterator I = m_data.begin(); I != Iend; ++I) {
        contains.push_back((*I)->getId());
    }
}
