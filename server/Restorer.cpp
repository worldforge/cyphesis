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

#include "Restorer_impl.h"

#include "rulesets/Creator.h"
#include "rulesets/World.h"
#include "rulesets/Plant.h"
#include "rulesets/Food.h"
#include "rulesets/Area.h"
#include "rulesets/Line.h"
#include "rulesets/Structure.h"
#include "rulesets/Stackable.h"
#include "rulesets/World.h"

#include "common/terrain_utils.h"

template <> Persistor<Entity> Restorer<Entity>::m_persist(true);
template <> Persistor<Thing> Restorer<Thing>::m_persist(true);
template <> Persistor<Character> Restorer<Character>::m_persist(true);
template <> Persistor<Creator> Restorer<Creator>::m_persist(true);
template <> Persistor<Plant> Restorer<Plant>::m_persist(true);
template <> Persistor<Food> Restorer<Food>::m_persist(true);
template <> Persistor<Area> Restorer<Area>::m_persist(true);
template <> Persistor<Line> Restorer<Line>::m_persist(true);
template <> Persistor<Stackable> Restorer<Stackable>::m_persist(true);
template <> Persistor<Structure> Restorer<Structure>::m_persist(true);
template <> Persistor<World> Restorer<World>::m_persist(true);

template<>
void Restorer<Character>::rCharacter(DatabaseResult::const_iterator & dr)
{
    dr.readColumn("drunkness", m_drunkness);
    dr.readColumn("sex", m_sex);
    dr.readColumn("food", m_food);
    dr.readColumn("right_hand_wield", m_rightHandWield);
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
template class Restorer<Food>;
template class Restorer<Area>;
template class Restorer<Line>;
template class Restorer<Stackable>;
template class Restorer<Structure>;
template class Restorer<World>;
