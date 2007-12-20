// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2002 Alistair Riddoch
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

// $Id: Restorer.cpp,v 1.22 2007-12-20 21:07:51 alriddoch Exp $

#include "Restorer_impl.h"

#include "rulesets/Creator.h"
#include "rulesets/World.h"
#include "rulesets/Plant.h"
#include "rulesets/Structure.h"
#include "rulesets/Stackable.h"
#include "rulesets/World.h"

#include "common/terrain_utils.h"

template<>
void Restorer<Character>::rCharacter(DatabaseResult::const_iterator & dr)
{
}

template<>
void Restorer<Character>::populate(DatabaseResult::const_iterator & dr)
{
    rEntity(dr);
    rCharacter(dr);
}

template<>
void Restorer<Plant>::rPlant(DatabaseResult::const_iterator & dr)
{
    dr.readColumn("fruits", m_fruits);
    dr.readColumn("sizeAdult", m_sizeAdult);
    dr.readColumn("fruitChance", m_fruitChance);
    dr.readColumn("fruitName", m_fruitName);
}

template<>
void Restorer<Plant>::populate(DatabaseResult::const_iterator & dr)
{
    rEntity(dr);
    rPlant(dr);
}

template<>
void Restorer<World>::populate(DatabaseResult::const_iterator & dr)
{
    rEntity(dr);

    // Restore the terrain
    loadTerrain(getId(), m_terrain);
}

template class Restorer<Entity>;
template class Restorer<Thing>;
template class Restorer<Character>;
template class Restorer<Creator>;
template class Restorer<Plant>;
template class Restorer<Stackable>;
template class Restorer<Structure>;
template class Restorer<World>;
