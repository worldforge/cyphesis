// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

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

Persistor<Entity> Restorer<Entity>::m_persist(true);
Persistor<Thing> Restorer<Thing>::m_persist(true);
Persistor<Character> Restorer<Character>::m_persist(true);
Persistor<Creator> Restorer<Creator>::m_persist(true);
Persistor<Plant> Restorer<Plant>::m_persist(true);
Persistor<Food> Restorer<Food>::m_persist(true);
Persistor<Area> Restorer<Area>::m_persist(true);
Persistor<Line> Restorer<Line>::m_persist(true);
Persistor<Stackable> Restorer<Stackable>::m_persist(true);
Persistor<Structure> Restorer<Structure>::m_persist(true);
Persistor<World> Restorer<World>::m_persist(true);

void Restorer<Character>::rCharacter(DatabaseResult::const_iterator & dr)
{
    restoreFloat(dr.column("drunkness"), m_drunkness);
    restoreString(dr.column("sex"), m_sex);
    restoreFloat(dr.column("food"), m_food);
}

void Restorer<Character>::populate(DatabaseResult::const_iterator & dr)
{
    rEntity(dr);
    rCharacter(dr);
}

void Restorer<Plant>::rPlant(DatabaseResult::const_iterator & dr)
{
    restoreInt(dr.column("fruits"), m_fruits);
    restoreFloat(dr.column("sizeAdult"), m_sizeAdult);
    restoreInt(dr.column("fruitChance"), m_fruitChance);
    restoreString(dr.column("fruitName"), m_fruitName);
}

void Restorer<Plant>::populate(DatabaseResult::const_iterator & dr)
{
    rEntity(dr);
    rPlant(dr);
}
