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

#include "rules/LocatedEntity.h"

#include "rules/simulation/LineProperty.h"
#include "rules/SolidProperty.h"
#include "rules/simulation/StatusProperty.h"
#include "rules/simulation/TerrainModProperty.h"
#include "rules/simulation/TerrainProperty.h"
#include "rules/simulation/TransientProperty.h"
#include "rules/simulation/ServerBBoxProperty.h"
#include "rules/simulation/AreaProperty.h"
#include "rules/simulation/VisibilityProperty.h"
#include "rules/simulation/SuspendedProperty.h"
#include "rules/simulation/TasksProperty.h"
#include "rules/simulation/EntityProperty.h"
#include "rules/simulation/DomainProperty.h"
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
#include "rules/simulation/AttachmentsProperty.h"
#include "rules/simulation/AdminProperty.h"
#include <rules/simulation/AmountProperty.h>
#include <rules/simulation/SimulationSpeedProperty.h>
#include <rules/simulation/ModeDataProperty.h>
#include <rules/simulation/ContainedVisibilityProperty.h>
#include <rules/simulation/ModifyProperty.h>
#include <rules/simulation/ModifiersProperty.h>
#include <rules/simulation/ModifySelfProperty.h>
#include "rules/simulation/CalendarProperty.h"
#include "rules/simulation/WorldTimeProperty.h"

#include "common/types.h"
#include "common/Inheritance.h"
#include "common/PropertyFactory_impl.h"

#include "common/debug.h"
#include "VisibilityDistanceProperty.h"
#include "TerrainPointsProperty.h"
#include "FilterProperty.h"
#include "AliasProperty.h"

#include <Atlas/Objects/Operation.h>

#include <iostream>
#include "rules/AtlasProperties.h"
#include "rules/PhysicalProperties.h"


using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;

static const bool debug_flag = false;


CorePropertyManager::CorePropertyManager(Inheritance& inheritance)
        : m_inheritance(inheritance)
{
    // Core types, for inheritance only generally.
    installBaseProperty<int>("int", "root_type");
    installBaseProperty<double>("float", "root_type");
    installBaseProperty<std::string>("string", "root_type");
    installBaseProperty<ListType>("list", "root_type");
    installBaseProperty<MapType>("map", "root_type");

    installProperty<PositionProperty>();
    installProperty<VelocityProperty>();
    installProperty<AngularVelocityProperty>();
    installProperty<OrientationProperty>();

    installProperty<EntityProperty>("entity_ref");
    installProperty<ModeProperty>();
    installProperty<LineProperty>("coords");
    installProperty<LineProperty>("points");
    installProperty<SolidProperty>();
    installProperty<StatusProperty>();
    installProperty<TransientProperty>();
    installProperty<Property<double>>("mass");
    installProperty<ServerBBoxProperty>();
    installProperty<AreaProperty>();
    installProperty<VisibilityProperty>();
    installProperty<VisibilityDistanceProperty>();
    installProperty<TerrainModProperty>();
    installProperty<TerrainProperty>();
    installProperty<TerrainPointsProperty>();
    installProperty<SuspendedProperty>();
    installProperty<TasksProperty>();
    installProperty<DomainProperty>();
    installProperty<PropelProperty>();
    /**
     * The "direction" property is set by the mind to indicate the direction the body should be moving.
     */
    installProperty<QuaternionProperty>("_direction");
    /**
     * The "_destination" property is set by the mind to indicate any destination the body should be moving to.
     * This is only to be used if the simulation easily can move the body there (it's close and nothing obscures it).
     */
    installProperty<Vector3Property>("_destination");


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
    installProperty<Property<MapType>>("_ready_at_attached")->m_flags |= prop_flag_persistence_ephem;

    /**
     * Keeps track of when singular entities are ready to be used again.
     * This could be used on a magic scroll for example.
     */
    installProperty<Property<double>>("ready_at")->m_flags |= prop_flag_persistence_ephem;

    installProperty<MindsProperty>();

    installProperty<AttachmentsProperty>();

    /**
     * Goals for the minds.
     */
    installProperty<Property<Atlas::Message::ListType>>("_goals");

    installProperty<AmountProperty>();

    installProperty<SimulationSpeedProperty>();

    /**
     * Handles data specific to the current mode.
     */
    installProperty<ModeDataProperty>();

    installProperty<ContainedVisibilityProperty>();

    installProperty<ModifyProperty>();
    installProperty<ModifySelfProperty>();
    installProperty<ModifiersProperty>();

    /**
     * Applies whenever an entity tries to move another entity.
     */
    installProperty<FilterProperty>("mover_constraint");
    /**
     * Applies whenever an entity is moved.
     */
    installProperty<FilterProperty>("move_constraint");
    /**
     * Applies whenever an entity has a contained child being moved.
     */
    installProperty<FilterProperty>("contain_constraint");
    /**
     * Applies whenever an entity has an entity being moved into it.
     */
    installProperty<FilterProperty>("destination_constraint");

    installProperty<AliasProperty>();

}

CorePropertyManager::~CorePropertyManager() = default;

int CorePropertyManager::installFactory(const std::string& type_name,
                                        const Root& type_desc,
                                        std::unique_ptr<PropertyKit> factory)
{
    if (m_inheritance.addChild(type_desc) == nullptr) {
        return -1;
    }

    PropertyManager::installFactory(type_name, std::move(factory));

    return 0;
}

std::unique_ptr<PropertyBase> CorePropertyManager::addProperty(const std::string& name) const
{
    assert(!name.empty());
    assert(name != "objtype");
    std::unique_ptr<PropertyBase> p;
    auto I = m_propertyFactories.find(name);
    if (I == m_propertyFactories.end()) {
        p = std::make_unique<SoftProperty>();
    } else {
        p = I->second->newProperty();
    }
    p->flags().addFlags(PropertyUtil::flagsForPropertyName(name));
    debug_print(name << " property found. ")
    return p;
}
