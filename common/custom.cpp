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

// $Id$

#include "Inheritance.h"

#include "custom.h"

#include <Atlas/Objects/Generic.h>

void installCustomOperations()
{
    Inheritance & i = Inheritance::instance();
    Atlas::Objects::Factories * atlas_factories = Atlas::Objects::Factories::instance();

    i.addChild(atlasOpDefinition("add", "set"));
    Atlas::Objects::Operation::ADD_NO = atlas_factories->addFactory("add", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("burn", "affect"));
    Atlas::Objects::Operation::BURN_NO = atlas_factories->addFactory("burn", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("connect", "set"));
    Atlas::Objects::Operation::CONNECT_NO = atlas_factories->addFactory("connect", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("eat", "action"));
    Atlas::Objects::Operation::EAT_NO = atlas_factories->addFactory("eat", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("monitor", "set"));
    Atlas::Objects::Operation::MONITOR_NO = atlas_factories->addFactory("monitor", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("nourish", "affect"));
    Atlas::Objects::Operation::NOURISH_NO = atlas_factories->addFactory("nourish", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("setup", "root_operation"));
    Atlas::Objects::Operation::SETUP_NO = atlas_factories->addFactory("setup", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("tick", "root_operation"));
    Atlas::Objects::Operation::TICK_NO = atlas_factories->addFactory("tick", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("unseen", "perception"));
    Atlas::Objects::Operation::UNSEEN_NO = atlas_factories->addFactory("unseen", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("attack", "action"));
    Atlas::Objects::Operation::ATTACK_NO = atlas_factories->addFactory("attack", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("pickup", "action"));
    Atlas::Objects::Operation::PICKUP_NO = atlas_factories->addFactory("pickup", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("drop", "action"));
    Atlas::Objects::Operation::DROP_NO = atlas_factories->addFactory("drop", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("update", "tick"));
    Atlas::Objects::Operation::UPDATE_NO = atlas_factories->addFactory("update", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("actuate", "action"));
    Atlas::Objects::Operation::ACTUATE_NO = atlas_factories->addFactory("actuate", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("thought", "communicate"));
    Atlas::Objects::Operation::THOUGHT_NO = atlas_factories->addFactory("thought", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("goal_info", "communicate"));
    Atlas::Objects::Operation::GOAL_INFO_NO = atlas_factories->addFactory("goal_info", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("teleport", "action"));
    Atlas::Objects::Operation::TELEPORT_NO = atlas_factories->addFactory("teleport", &Atlas::Objects::generic_factory);
}

void installCustomEntities()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(atlasClass("room", "admin_entity"));

    i.addChild(atlasClass("lobby", "admin_entity"));

    i.addChild(atlasClass("server", "admin_entity"));

    i.addChild(atlasClass("task", "admin_entity"));
}
