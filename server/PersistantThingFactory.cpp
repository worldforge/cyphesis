// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "PersistantThingFactory_impl.h"
#include "Persistor.h"

#include <rulesets/Entity.h>
#include <rulesets/Thing.h>
#include <rulesets/Character.h>
#include <rulesets/Creator.h>
#include <rulesets/Food.h>
#include <rulesets/Line.h>
#include <rulesets/Area.h>
#include <rulesets/Plant.h>
#include <rulesets/Structure.h>
#include <rulesets/Stackable.h>
#include <rulesets/World.h>

template class PersistorConnection<Entity>;
template class PersistorConnection<Thing>;
template class PersistorConnection<Character>;
template class PersistorConnection<Creator>;
template class PersistorConnection<Food>;
template class PersistorConnection<Line>;
template class PersistorConnection<Area>;
template class PersistorConnection<Plant>;
template class PersistorConnection<Structure>;
template class PersistorConnection<Stackable>;
template class PersistorConnection<World>;

template class PersistantThingFactory<Entity>;
template class PersistantThingFactory<Thing>;
template class PersistantThingFactory<Character>;
template class PersistantThingFactory<Creator>;
template class PersistantThingFactory<Food>;
template class PersistantThingFactory<Line>;
template class PersistantThingFactory<Area>;
template class PersistantThingFactory<Plant>;
template class PersistantThingFactory<Structure>;
template class PersistantThingFactory<Stackable>;
template class PersistantThingFactory<World>;
