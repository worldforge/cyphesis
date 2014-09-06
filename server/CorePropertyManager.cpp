// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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


#include "CorePropertyManager.h"

#include "server/Juncture.h"
#include "server/ServerRouting.h"
#include "server/TeleportProperty.h"

#include "rulesets/LineProperty.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/SolidProperty.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/StatisticsProperty.h"
#include "rulesets/TerrainModProperty.h"
#include "rulesets/TerrainProperty.h"
#include "rulesets/TransientProperty.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/BiomassProperty.h"
#include "rulesets/BurnSpeedProperty.h"
#include "rulesets/DecaysProperty.h"
#include "rulesets/MindProperty.h"
#include "rulesets/InternalProperties.h"
#include "rulesets/SpawnProperty.h"
#include "rulesets/AreaProperty.h"
#include "rulesets/VisibilityProperty.h"
#include "rulesets/SuspendedProperty.h"
#include "rulesets/TasksProperty.h"
#include "rulesets/EntityProperty.h"
#include "rulesets/SpawnerProperty.h"
#include "rulesets/ImmortalProperty.h"
#include "rulesets/RespawningProperty.h"
#include "rulesets/DefaultLocationProperty.h"

#include "common/Eat.h"
#include "common/Burn.h"
#include "common/Teleport.h"

#include "common/types.h"
#include "common/Inheritance.h"
#include "common/PropertyFactory_impl.h"

#include "common/debug.h"

#include <Atlas/Objects/Operation.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;

static const bool debug_flag = false;

template<typename T>
void CorePropertyManager::installBaseProperty(const std::string & type_name,
                                              const std::string & parent)
{
    installFactory(type_name,
                   atlasType(type_name, parent, true),
                   new PropertyFactory<Property<T>>);
}

template<typename PropertyT>
void CorePropertyManager::installProperty(const std::string & type_name,
                                          const std::string & parent)
{
    installFactory(type_name,
                   atlasType(type_name, parent),
                   new PropertyFactory<PropertyT>);
}

CorePropertyManager::CorePropertyManager()
{
    // Core types, for inheritence only generally.
    installBaseProperty<int>("int", "root_type");
    installBaseProperty<double>("float", "root_type");
    installBaseProperty<std::string>("string", "root_type");
    installBaseProperty<ListType>("list", "root_type");
    installBaseProperty<MapType>("map", "root_type");

    installProperty<Property<double>>("stamina", "float");
    installProperty<LineProperty>("coords", "list");
    installProperty<LineProperty>("points", "list");
    installProperty<Property<IdList> >("start_intersections", "list");
    installProperty<Property<IdList> >("end_intersections", "list");
    installProperty<DecaysProperty>("decays", "string");
    installProperty<OutfitProperty>("outfit", "map");
    installProperty<SolidProperty>("solid", "int");
    installProperty<SimpleProperty>("simple", "int");
    installProperty<StatusProperty>("status", "float");
    installProperty<BiomassProperty>("biomass", "float");
    installProperty<BurnSpeedProperty>("burn_speed", "float");
    installProperty<TransientProperty>("transient", "float");
    installProperty<Property<double> >("food", "float");
    installProperty<Property<double> >("mass", "float");
    installProperty<BBoxProperty>("bbox", "list");
    installProperty<MindProperty>("mind", "map");
    installProperty<SetupProperty>("init", "int");
    installProperty<TickProperty>("ticks", "float");
    installProperty<StatisticsProperty>("statistics", "map");
    installProperty<SpawnProperty>("spawn", "map");
    installProperty<AreaProperty>("area", "map");
    installProperty<VisibilityProperty>("visibility", "float");
    installProperty<TerrainModProperty>("terrainmod", "map");
    installProperty<TerrainProperty>("terrain", "map");
    installProperty<TeleportProperty>("linked", "string");
    installProperty<SuspendedProperty>("suspended", "int");
    installProperty<TasksProperty>("tasks", "map");
    installProperty<EntityProperty>("right_hand_wield", "string");
    installProperty<SpawnerProperty>("spawner", "map");
    installProperty<ImmortalProperty>("immortal", "int");
    installProperty<RespawningProperty>("respawning", "string");
    installProperty<DefaultLocationProperty>("default_location", "int");
}

CorePropertyManager::~CorePropertyManager()
{
}

int CorePropertyManager::installFactory(const std::string & type_name,
                                        const Root & type_desc,
                                        PropertyKit * factory)
{
    Inheritance & i = Inheritance::instance();
    if (i.addChild(type_desc) == 0) {
        return -1;
    }

    PropertyManager::installFactory(type_name, factory);

    return 0;
}

PropertyBase * CorePropertyManager::addProperty(const std::string & name,
                                                int type)
{
    assert(!name.empty());
    assert(name != "objtype");
    PropertyBase * p = 0;
    PropertyFactoryDict::const_iterator I = m_propertyFactories.find(name);
    if (I == m_propertyFactories.end()) {
        switch (type) {
          case Element::TYPE_INT:
            p = new Property<int>;
            break;
          case Element::TYPE_FLOAT:
            p = new Property<double>;
            break;
          case Element::TYPE_STRING:
            p = new Property<std::string>;
            break;
          default:
            p = new SoftProperty;
            break;
        }
    } else {
        p = I->second->newProperty();
    }
    debug(std::cout << name << " property found. " << std::endl << std::flush;);
    return p;
}
