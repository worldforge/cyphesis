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
#include "server/TeleportProperty.h"

#include "rules/simulation/LineProperty.h"
#include "rules/SolidProperty.h"
#include "rules/simulation/StatusProperty.h"
#include "rules/simulation/StatisticsProperty.h"
#include "rules/simulation/TerrainModProperty.h"
#include "rules/simulation/TerrainProperty.h"
#include "rules/simulation/TransientProperty.h"
#include "rules/simulation/ServerBBoxProperty.h"
#include "rules/simulation/MindProperty.h"
#include "rules/simulation/InternalProperties.h"
#include "rules/simulation/SpawnProperty.h"
#include "rules/simulation/AreaProperty.h"
#include "rules/simulation/VisibilityProperty.h"
#include "rules/simulation/SuspendedProperty.h"
#include "rules/simulation/TasksProperty.h"
#include "rules/simulation/EntityProperty.h"
#include "rules/simulation/SpawnerProperty.h"
#include "rules/simulation/RespawningProperty.h"
#include "rules/simulation/DefaultLocationProperty.h"
#include "rules/simulation/DomainProperty.h"
#include "rules/simulation/LimboProperty.h"
#include "rules/simulation/ModeProperty.h"
#include "rules/simulation/PropelProperty.h"
#include "rules/simulation/DensityProperty.h"
#include "rules/simulation/AngularFactorProperty.h"
#include "rules/simulation/GeometryProperty.h"
#include "rules/QuaternionProperty.h"
#include "rules/Vector3Property.h"
#include "rules/simulation/PerceptionSightProperty.h"
#include "rules/ScaleProperty.h"
#include "rules/python/ScriptsProperty.h"
#include "rules/simulation/UsagesProperty.h"
#include "rules/simulation/MindsProperty.h"
#include "rules/simulation/PlantedOnProperty.h"
#include "rules/simulation/AttachmentsProperty.h"
#include "rules/simulation/AdminProperty.h"

#include "common/operations/Teleport.h"

#include "common/types.h"
#include "common/Inheritance.h"
#include "common/PropertyFactory_impl.h"

#include "common/debug.h"

#include <Atlas/Objects/Operation.h>

#include <iostream>
#include <rules/simulation/AmountProperty.h>
#include <rules/simulation/SimulationSpeedProperty.h>
#include <rules/simulation/ModeDataProperty.h>
#include "rules/simulation/CalendarProperty.h"
#include "rules/simulation/WorldTimeProperty.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;

static const bool debug_flag = false;

template<typename T>
PropertyFactory<Property<T>>* CorePropertyManager::installBaseProperty(const std::string & type_name,
                                                                       const std::string & parent)
{
    return this->installProperty<Property<T>>(type_name, parent);
}

template<typename PropertyT>
PropertyFactory<PropertyT>* CorePropertyManager::installProperty(const std::string & type_name,
                                                                 const std::string & parent)
{
    auto factory = new PropertyFactory<PropertyT>{};
    //Attach visibility flags. Properties that starts with "__" are private, "_" are protected and the rest are public.
    factory->m_flags = PropertyBase::flagsForPropertyName(type_name);
    installFactory(type_name,
                   atlasType(type_name, parent),
                   std::unique_ptr<PropertyFactory<PropertyT>>(factory));
    return factory;
}

template<typename PropertyT>
PropertyFactory<PropertyT>* CorePropertyManager::installProperty(const std::string & type_name)
{
    return this->installProperty<PropertyT>(type_name, PropertyT::property_atlastype);
}

template<typename PropertyT>
PropertyFactory<PropertyT>* CorePropertyManager::installProperty()
{
    return this->installProperty<PropertyT>(PropertyT::property_name, PropertyT::property_atlastype);
}


CorePropertyManager::CorePropertyManager()
{
    // Core types, for inheritance only generally.
    installBaseProperty<int>("int", "root_type");
    installBaseProperty<double>("float", "root_type");
    installBaseProperty<std::string>("string", "root_type");
    installBaseProperty<ListType>("list", "root_type");
    installBaseProperty<MapType>("map", "root_type");

    installProperty<EntityProperty>("entity_ref");
    installProperty<Property<double>>("stamina");
    installProperty<ModeProperty>();
    installProperty<LineProperty>("coords");
    installProperty<LineProperty>("points");
    //installProperty<Property<IdList> >("start_intersections", "list");
    //installProperty<Property<IdList> >("end_intersections", "list");
    installProperty<SolidProperty>();
    installProperty<StatusProperty>();
    installProperty<TransientProperty>();
    installProperty<Property<double>>("mass");
    installProperty<ServerBBoxProperty>();
    installProperty<MindProperty>();
    installProperty<SetupProperty>();
    installProperty<TickProperty>();
    installProperty<StatisticsProperty>();
    installProperty<SpawnProperty>();
    installProperty<AreaProperty>();
    installProperty<VisibilityProperty>();
    installProperty<TerrainModProperty>();
    installProperty<TerrainProperty>();
    installProperty<TeleportProperty>("linked");
    installProperty<SuspendedProperty>();
    installProperty<TasksProperty>();
    installProperty<SpawnerProperty>();
    installProperty<BoolProperty>("immortal");
    installProperty<RespawningProperty>();
    installProperty<DefaultLocationProperty>();
    installProperty<DomainProperty>();
    installProperty<LimboProperty>();
    installProperty<PropelProperty>();
    installProperty<DensityProperty>();
    installProperty<AdminProperty>();
    installProperty<CalendarProperty>();
    installProperty<WorldTimeProperty>();
    /**
     * Friction is used by the physics system. 0 is no friction, 1 is full friction.
     * This is for "sliding", see "friction-roll" and "friction-spin".
     */
    installProperty<Property<double>>("friction");
    /**
     * Friction for rolling is used by the physics system. 0 is no friction, 1 is full friction.
     */
    installProperty<Property<double>>("friction_roll");
    /**
     * Friction for spinning is used by the physics system. 0 is no friction, 1 is full friction.
     */
    installProperty<Property<double>>("friction_spin");

    installProperty<AngularFactorProperty>();
    installProperty<GeometryProperty>();

    /**
     * Refers to an entity on which another entity is planted on.
     */
    installProperty<PlantedOnProperty>();


    installProperty<BoolProperty>("floats");

    /**
     * Vertical offset to use when entity is planted, and adjusted to the height of the terrain.
     */
    installProperty<Property<double>>("planted_offset");

    /**
     * Vertical scaled offset to use when entity is planted, and adjusted to the height of the terrain.
     * The resulting offset is a product of this value and the height of the entity.
     */
    installProperty<Property<double>>("planted_scaled_offset");

    /**
     * The rotation applied to the entity when it's planted.
     */
    installProperty<QuaternionProperty>("planted_rotation");
    /**
     * The current extra rotation applied to the entity.
     * This is closely matched with "planted_rotation" to keep track of when the entity has the planted rotation applied and not.
     */
    installProperty<QuaternionProperty>("active_rotation");

    /**
     * Used for things that grows, to limit the size.
     */
    installProperty<Vector3Property>("maxsize");

    /**
     * Specifies how much the entity is allowed to step onto things when moving, as a factor of the entity's height.
     */
    installProperty<Property<double>>("step_factor");

    /**
     * Specifies a mesh, model or mapping to use for client side presentation.
     */
    installProperty<Property<std::string> >("present");

    /**
     * The max speed in meters per second (m/s) when moving over ground.
     */
    installProperty<Property<double>>("speed_ground");
    /**
     * The max speed in meters per second (m/s) when moving in water.
     */
    installProperty<Property<double>>("speed_water");
    /**
     * The max speed in meters per second (m/s) when flying.
     */
    installProperty<Property<double>>("speed_flight");
    /**
     * The max speed in meters per second (m/s) when jumping.
     */
    installProperty<Property<double>>("speed_jump");

    /**
     * If set to 1 the entity is a body of water, i.e. either an Ocean (if no bbox) or a lake/pond (if a bbox).
     */
    installProperty<BoolProperty>("water_body");

    installProperty<PerceptionSightProperty>();

    /**
     * How far away the entity can reach (used when interacting with things).
     */
    installProperty<Property<double>>("reach");

    installProperty<ScaleProperty>();

    /**
     * The max scale of an entity. This is a single value; when enforcing one dimension of the actual scale value will be used (often y).
     */
    installProperty<Property<double>>("maxscale");

    /**
     * "__scripts" is meant to be installed on the Type, whereas "__scripts_instance" is meant to be installed on the entity instance.
     * This way it should be possible to both override type scripts, and to alternatively add new ones to specific instances.
     */
    installProperty<ScriptsProperty>("__scripts");
    installProperty<ScriptsProperty>("__scripts_instance");

    installProperty<UsagesProperty>("usages");
    installProperty<UsagesProperty>("_usages");

    /**
     * Keeps track of when attachments are ready to be used again.
     * For example, if you swing a sword you can't do anything with the sword arm for a second or two.
     */
    installProperty<Property<MapType>>("_ready_at_attached")->m_flags |= persistence_ephem;

    /**
     * Keeps track of when singular entities are ready to be used again.
     * This could be used on a magic scroll for example.
     */
    installProperty<Property<double>>("ready_at")->m_flags |= persistence_ephem;

    installProperty<MindsProperty>();

    installProperty<AttachmentsProperty>();

    /**
     * Goals for the minds.
     */
    installProperty<Property<Atlas::Message::ListType>>("_goals");

    installProperty<AmountProperty>();

    installProperty<SimulationSpeedProperty>();

    installProperty<ModeDataProperty>();


}

int CorePropertyManager::installFactory(const std::string & type_name,
                                        const Root & type_desc,
                                        std::unique_ptr<PropertyKit> factory)
{
    Inheritance & i = Inheritance::instance();
    if (i.addChild(type_desc) == nullptr) {
        return -1;
    }

    PropertyManager::installFactory(type_name, std::move(factory));

    return 0;
}

PropertyBase * CorePropertyManager::addProperty(const std::string & name,
                                                int type)
{
    assert(!name.empty());
    assert(name != "objtype");
    PropertyBase * p = nullptr;
    auto I = m_propertyFactories.find(name);
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
            p = new SoftProperty();
            break;
        }
    } else {
        p = I->second->newProperty();
    }
    p->flags().addFlags(PropertyBase::flagsForPropertyName(name));
    debug_print(name << " property found. ")
    return p;
}
