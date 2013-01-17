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

// $Id$

#include "CorePropertyManager.h"

#include "server/Juncture.h"
#include "server/ServerRouting.h"
#include "server/TeleportProperty.h"

#include "rulesets/ActivePropertyFactory_impl.h"

#include "rulesets/LineProperty.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/SolidProperty.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/StatisticsProperty.h"
#include "rulesets/TerrainModProperty.h"
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

#include "common/Eat.h"
#include "common/Burn.h"
#include "common/Teleport.h"

#include "common/types.h"
#include "common/PropertyFactory_impl.h"

#include "common/debug.h"

#include <Atlas/Objects/Operation.h>

#include <iostream>

using Atlas::Message::Element;

static const bool debug_flag = false;

HandlerResult test_handler(LocatedEntity *, const Operation &, OpVector & res)
{
    debug(std::cout << "TEST HANDLER CALLED" << std::endl << std::flush;);
    return OPERATION_IGNORED;
}

CorePropertyManager::CorePropertyManager()
{
    m_propertyFactories["stamina"] = new PropertyFactory<Property<double> >;
    m_propertyFactories["coords"] = new PropertyFactory<LineProperty>;
    m_propertyFactories["points"] = new PropertyFactory<LineProperty>;
    m_propertyFactories["start_intersections"] = new PropertyFactory<Property<IdList> >;
    m_propertyFactories["end_intersections"] = new PropertyFactory<Property<IdList> >;
    m_propertyFactories["attachment"] = new ActivePropertyFactory<int>(Atlas::Objects::Operation::MOVE_NO, test_handler);
    m_propertyFactories["decays"] = new ActivePropertyFactory<std::string>(Atlas::Objects::Operation::DELETE_NO, DecaysProperty::del_handler);
    m_propertyFactories["outfit"] = new PropertyFactory<OutfitProperty>;
    m_propertyFactories["solid"] = new PropertyFactory<SolidProperty>;
    m_propertyFactories["simple"] = new PropertyFactory<SimpleProperty>;
    m_propertyFactories["status"] = new PropertyFactory<StatusProperty>;
    m_propertyFactories["biomass"] = new ActivePropertyFactory<double>(Atlas::Objects::Operation::EAT_NO, BiomassProperty::eat_handler);
    m_propertyFactories["burn_speed"] = new ActivePropertyFactory<double>(Atlas::Objects::Operation::BURN_NO, BurnSpeedProperty::burn_handler);
    m_propertyFactories["transient"] = new PropertyFactory<TransientProperty>();
    m_propertyFactories["food"] = new PropertyFactory<Property<double> >;
    m_propertyFactories["mass"] = new PropertyFactory<Property<double> >;
    m_propertyFactories["bbox"] = new PropertyFactory<BBoxProperty>;
    m_propertyFactories["mind"] = new PropertyFactory<MindProperty>;
    m_propertyFactories["setup"] = new PropertyFactory<SetupProperty>;
    m_propertyFactories["tick"] = new PropertyFactory<TickProperty>;
    m_propertyFactories["statistics"] = new PropertyFactory<StatisticsProperty>;
    m_propertyFactories["spawn"] = new PropertyFactory<SpawnProperty>;
    m_propertyFactories["area"] = new PropertyFactory<AreaProperty>;
    m_propertyFactories["visibility"] = new PropertyFactory<VisibilityProperty>;
    
    HandlerMap terrainModHandles;
    terrainModHandles[Atlas::Objects::Operation::MOVE_NO] =
          TerrainModProperty::move_handler;
    terrainModHandles[Atlas::Objects::Operation::DELETE_NO] =
          TerrainModProperty::delete_handler;
    m_propertyFactories["terrainmod"] = new MultiActivePropertyFactory<TerrainModProperty>(terrainModHandles);

    m_propertyFactories["teleport"] = new ActivePropertyFactory<std::string>(Atlas::Objects::Operation::TELEPORT_NO, TeleportProperty::teleport_handler);
}

CorePropertyManager::~CorePropertyManager()
{
    std::map<std::string, PropertyKit *>::const_iterator I = m_propertyFactories.begin();
    std::map<std::string, PropertyKit *>::const_iterator Iend = m_propertyFactories.end();
    for (; I != Iend; ++I) {
        assert(I->second != 0);
        delete I->second;
    }
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
