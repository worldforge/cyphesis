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

#include <Python.h>

#include "PersistantThingFactory_impl.h"
#include "ScriptFactory.h"
#include "EntityFactory.h"

#include "rulesets/Entity.h"
#include "rulesets/Thing.h"
#include "rulesets/Character.h"
#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Structure.h"
#include "rulesets/Stackable.h"
#include "rulesets/World.h"

#include "common/log.h"

EntityKit::EntityKit() : m_scriptFactory(0), m_createdCount(0)
{
}

EntityKit::~EntityKit()
{
    if (m_scriptFactory != 0) {
        delete m_scriptFactory;
    }
}

template <>
World * ThingFactory<World>::newThing(const std::string & id,
                                                long intId)
{
    return 0;
}

template<>
int ThingFactory<Character>::populate(Entity & e)
{
    EntityBuilder::instance()->addStatisticsScript((Character &)e);
    return 0;
}

template class ThingFactory<Thing>;
template class ThingFactory<Character>;
template class ThingFactory<Creator>;
template class ThingFactory<Plant>;
template class ThingFactory<Stackable>;
template class ThingFactory<Structure>;
template class ThingFactory<World>;
