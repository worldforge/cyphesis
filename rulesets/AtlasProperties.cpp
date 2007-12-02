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

// $Id: AtlasProperties.cpp,v 1.2 2007-12-02 23:49:06 alriddoch Exp $

#include "AtlasProperties.h"

#include "LocatedEntity.h"

#include "common/type_utils.h"
#include "common/debug.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Objects::Entity::RootEntity;

IdProperty::IdProperty(const std::string & data) :
                       ImmutableProperty<std::string>(data)
{
}

void IdProperty::add(const std::string & s, const RootEntity & ent) const
{
    ent->setId(m_data);
}

NameProperty::NameProperty(std::string & data, unsigned int flags) :
                           Property<std::string>(data, flags)
{
}

void NameProperty::add(const std::string & s, const RootEntity & ent) const
{
    ent->setName(m_data);
}

ContainsProperty::ContainsProperty(LocatedEntitySet & data) :
                                   ImmutableProperty<LocatedEntitySet>(data)
{
}

void ContainsProperty::add(const std::string & s, const RootEntity & ent) const
{
    std::list<std::string> & contains = ent->modifyContains();
    contains.clear();
    LocatedEntitySet::const_iterator Iend = m_data.end();
    for (LocatedEntitySet::const_iterator I = m_data.begin(); I != Iend; ++I) {
        contains.push_back((*I)->getId());
    }
}
