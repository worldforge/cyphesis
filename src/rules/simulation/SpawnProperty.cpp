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


#include "SpawnProperty.h"

#include "rules/LocatedEntity.h"

#include "rules/simulation/BaseWorld.h"

using Atlas::Message::MapType;

SpawnProperty * SpawnProperty::copy() const
{
    return new SpawnProperty(*this);
}

void SpawnProperty::apply(LocatedEntity& ent)
{
    BaseWorld::instance().registerSpawner(ent.getId());
}

void SpawnProperty::remove(LocatedEntity& ent, const std::string&)
{
    BaseWorld::instance().unregisterSpawner(ent.getId());
}
