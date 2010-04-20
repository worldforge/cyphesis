// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include "rulesets/MemEntity.h"

#include <cassert>

int main()
{
    MemEntity * me = new MemEntity("1", 1);

    delete me;
    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}

// stubs

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

bool LocatedEntity::getAttr(const std::string & name, Atlas::Message::Element & attr) const
{
    return false;
}

bool LocatedEntity::getAttrType(const std::string & name,
                                Atlas::Message::Element & attr,
                                int type) const
{
    return false;
}

void LocatedEntity::setAttr(const std::string & name, const Atlas::Message::Element & attr)
{
    return;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

void LocatedEntity::onContainered()
{
}

void LocatedEntity::onUpdated()
{
}

Router::Router(const std::string & id, long intId) : m_id(id),
                                                             m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

Location::Location()
{
}
