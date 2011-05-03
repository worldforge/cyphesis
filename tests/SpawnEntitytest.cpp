// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/SpawnEntity.h"

#include "rulesets/AreaProperty.h"

#include "physics/Vector3D.h"

#include <Atlas/Message/Element.h>

#include <wfmath/point.h>

#include <cassert>

int main()
{
    {
        Atlas::Message::MapType spawn_data;
        Spawn * s = new SpawnEntity(0, spawn_data);

        delete s;
    }

    {
        Atlas::Message::MapType spawn_data;
        spawn_data["character_types"] = "bob";
        Spawn * s = new SpawnEntity(0, spawn_data);

        delete s;
    }

    {
        Atlas::Message::MapType spawn_data;
        spawn_data["character_types"] = Atlas::Message::ListType(1, "bob");
        Spawn * s = new SpawnEntity(0, spawn_data);

        delete s;
    }

    {
        Atlas::Message::MapType spawn_data;
        spawn_data["contains"] = "bob";
        Spawn * s = new SpawnEntity(0, spawn_data);

        delete s;
    }

    {
        Atlas::Message::MapType spawn_data;
        spawn_data["contains"] = Atlas::Message::ListType(1, "bob");
        Spawn * s = new SpawnEntity(0, spawn_data);

        delete s;
    }

    return 0;
}

// Stabs

Spawn::~Spawn()
{
}

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}

EntityRef::EntityRef(Entity* e) : m_inner(e)
{
}

EntityRef::EntityRef(const EntityRef& ref) : m_inner(ref.m_inner)
{
}

EntityRef& EntityRef::operator=(const EntityRef& ref)
{
    m_inner = ref.m_inner;

    return *this;
}

void EntityRef::onEntityDeleted()
{
}

AreaProperty::AreaProperty()
{
}

AreaProperty::~AreaProperty()
{
}

bool AreaProperty::get(Atlas::Message::Element & ent) const
{
    return true;

}

void AreaProperty::set(const Atlas::Message::Element & ent)
{
}

void AreaProperty::add(const std::string & s, Atlas::Message::MapType & ent) const
{
}

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(Entity *)
{
}

void PropertyBase::apply(Entity *)
{
}

void PropertyBase::add(const std::string & s,
                       Atlas::Message::MapType & ent) const
{
}

void PropertyBase::add(const std::string & s,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
}
