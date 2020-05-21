// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2006 Alistair Riddoch
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


#include "Inheritance.h"

#include "custom.h"

#include <Atlas/Objects/Generic.h>

#include "operations/CloseContainer.h"

void installCustomOperations(TypeStore & i)
{
    auto& atlas_factories = i.getFactories();

    i.addChild(atlasOpDefinition("connect", "set"));
    Atlas::Objects::Operation::CONNECT_NO = atlas_factories.addFactory("connect", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

    i.addChild(atlasOpDefinition("monitor", "set"));
    Atlas::Objects::Operation::MONITOR_NO = atlas_factories.addFactory("monitor", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

    i.addChild(atlasOpDefinition("setup", "root_operation"));
    Atlas::Objects::Operation::SETUP_NO = atlas_factories.addFactory("setup", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

    i.addChild(atlasOpDefinition("tick", "root_operation"));
    Atlas::Objects::Operation::TICK_NO = atlas_factories.addFactory("tick", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

    i.addChild(atlasOpDefinition("update", "tick"));
    Atlas::Objects::Operation::UPDATE_NO = atlas_factories.addFactory("update", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

    i.addChild(atlasOpDefinition("thought", "communicate"));
    Atlas::Objects::Operation::THOUGHT_NO = atlas_factories.addFactory("thought", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

    i.addChild(atlasOpDefinition("goal_info", "communicate"));
    Atlas::Objects::Operation::GOAL_INFO_NO = atlas_factories.addFactory("goal_info", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

    i.addChild(atlasOpDefinition("teleport", "action"));
    Atlas::Objects::Operation::TELEPORT_NO = atlas_factories.addFactory("teleport", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

    i.addChild(atlasOpDefinition("commune", "get"));
    Atlas::Objects::Operation::COMMUNE_NO = atlas_factories.addFactory("commune", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

    i.addChild(atlasOpDefinition("think", "set"));
    Atlas::Objects::Operation::THINK_NO = atlas_factories.addFactory("think", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

    //The relay operation is used when an operation needs to be sent from one router to another, and there's no natural way of doing it.
    i.addChild(atlasOpDefinition("relay", "root_operation"));
    Atlas::Objects::Operation::RELAY_NO = atlas_factories.addFactory("relay", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

    i.addChild(atlasOpDefinition("possess", "set"));
    Atlas::Objects::Operation::POSSESS_NO = atlas_factories.addFactory("possess", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

    i.addChild(atlasOpDefinition("close_container", "action"));
    Atlas::Objects::Operation::CLOSE_CONTAINER_NO = atlas_factories.addFactory("close_container", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);
}

void installCustomEntities(TypeStore & i)
{

    i.addChild(atlasClass("room", "admin_entity"));

    i.addChild(atlasClass("lobby", "admin_entity"));

    i.addChild(atlasClass("server", "admin_entity"));

    i.addChild(atlasClass("task", "admin_entity"));
}
