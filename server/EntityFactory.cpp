// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

#include "EntityFactory_impl.h"

#include "rulesets/Character.h"
#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Stackable.h"
#include "rulesets/World.h"

using Atlas::Message::MapType;

template <>
LocatedEntity * EntityFactory<World>::newEntity(const std::string & id,
                                                long intId)
{
    return 0;
}

template class EntityFactory<Thing>;
template class EntityFactory<Character>;
template class EntityFactory<Creator>;
template class EntityFactory<Plant>;
template class EntityFactory<Stackable>;
template class EntityFactory<World>;
