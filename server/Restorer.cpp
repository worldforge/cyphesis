// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#include "Restorer_impl.h"

#include <rulesets/Creator.h>
#include <rulesets/World.h>
#include <rulesets/Plant.h>
#include <rulesets/Food.h>
#include <rulesets/Area.h>
#include <rulesets/Line.h>
#include <rulesets/Structure.h>
#include <rulesets/Stackable.h>

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

void Restorer<Character>::rCharacter(DatabaseResult & dr)
{
    restoreFloat(dr.field("drunkness"), drunkness);
    restoreString(dr.field("sex"), sex);
    restoreFloat(dr.field("food"), food);
}

void Restorer<Character>::populate(const std::string & id, DatabaseResult & dr)
{
    setId(id);
    rEntity(dr);
    rCharacter(dr);
}
