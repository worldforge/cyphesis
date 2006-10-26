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

// $Id: PersistantThingFactory.cpp,v 1.14 2006-10-26 00:48:14 alriddoch Exp $

#include <Python.h>

#include "PersistantThingFactory_impl.h"
#include "Persistor.h"
#include "ScriptFactory.h"
#include "EntityFactory.h"

#include "rulesets/Entity.h"
#include "rulesets/Thing.h"
#include "rulesets/Character.h"
#include "rulesets/Creator.h"
#include "rulesets/Food.h"
#include "rulesets/Line.h"
#include "rulesets/Area.h"
#include "rulesets/Plant.h"
#include "rulesets/Structure.h"
#include "rulesets/Stackable.h"
#include "rulesets/World.h"

#include "common/log.h"

FactoryBase::FactoryBase() : m_scriptFactory(0)
{
}

FactoryBase::~FactoryBase()
{
    if (m_scriptFactory != 0) {
        delete m_scriptFactory;
    }
}

template<>
int PersistantThingFactory<Character>::populate(Entity & e)
{
    EntityFactory::instance()->addStatisticsScript((Character &)e);
    return 0;
}

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

template class ForbiddenThingFactory<World>;
