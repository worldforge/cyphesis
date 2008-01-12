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

// $Id: custom.cpp,v 1.49 2008-01-12 22:41:11 alriddoch Exp $

#include "inheritance.h"

#include "Add.h"
#include "Attack.h"
#include "Burn.h"
#include "Drop.h"
#include "Eat.h"
#include "Monitor.h"
#include "Nourish.h"
#include "Pickup.h"
#include "Setup.h"
#include "Tick.h"
#include "Unseen.h"
#include "Update.h"

#include <Atlas/Objects/Entity.h>

void installCustomOperations()
{
    Inheritance & i = Inheritance::instance();
    Atlas::Objects::Factories * atlas_factories = Atlas::Objects::Factories::instance();

    i.addChild(atlasOpDefinition("add", "set"));
    Atlas::Objects::Operation::ADD_NO = atlas_factories->addFactory("add", &Atlas::Objects::generic_factory);
    i.opInstall("add", Atlas::Objects::Operation::ADD_NO);

    i.addChild(atlasOpDefinition("burn", "affect"));
    Atlas::Objects::Operation::BURN_NO = atlas_factories->addFactory("burn", &Atlas::Objects::generic_factory);
    i.opInstall("burn", Atlas::Objects::Operation::BURN_NO);

    i.addChild(atlasOpDefinition("eat", "action"));
    Atlas::Objects::Operation::EAT_NO = atlas_factories->addFactory("eat", &Atlas::Objects::generic_factory);
    i.opInstall("eat", Atlas::Objects::Operation::EAT_NO);

    i.addChild(atlasOpDefinition("monitor", "affect"));
    Atlas::Objects::Operation::MONITOR_NO = atlas_factories->addFactory("monitor", &Atlas::Objects::generic_factory);
    i.opInstall("monitor", Atlas::Objects::Operation::MONITOR_NO);

    i.addChild(atlasOpDefinition("nourish", "affect"));
    Atlas::Objects::Operation::NOURISH_NO = atlas_factories->addFactory("nourish", &Atlas::Objects::generic_factory);
    i.opInstall("nourish", Atlas::Objects::Operation::NOURISH_NO);

    i.addChild(atlasOpDefinition("setup", "root_operation"));
    Atlas::Objects::Operation::SETUP_NO = atlas_factories->addFactory("setup", &Atlas::Objects::generic_factory);
    i.opInstall("setup", Atlas::Objects::Operation::SETUP_NO);

    i.addChild(atlasOpDefinition("tick", "root_operation"));
    Atlas::Objects::Operation::TICK_NO = atlas_factories->addFactory("tick", &Atlas::Objects::generic_factory);
    i.opInstall("tick", Atlas::Objects::Operation::TICK_NO);

    i.addChild(atlasOpDefinition("unseen", "perception"));
    Atlas::Objects::Operation::UNSEEN_NO = atlas_factories->addFactory("unseen", &Atlas::Objects::generic_factory);
    i.opInstall("unseen", Atlas::Objects::Operation::UNSEEN_NO);

    i.addChild(atlasOpDefinition("attack", "action"));
    Atlas::Objects::Operation::ATTACK_NO = atlas_factories->addFactory("attack", &Atlas::Objects::generic_factory);
    i.opInstall("attack", Atlas::Objects::Operation::ATTACK_NO);

    i.addChild(atlasOpDefinition("pickup", "action"));
    Atlas::Objects::Operation::PICKUP_NO = atlas_factories->addFactory("pickup", &Atlas::Objects::generic_factory);
    i.opInstall("pickup", Atlas::Objects::Operation::PICKUP_NO);

    i.addChild(atlasOpDefinition("drop", "action"));
    Atlas::Objects::Operation::DROP_NO = atlas_factories->addFactory("drop", &Atlas::Objects::generic_factory);
    i.opInstall("drop", Atlas::Objects::Operation::DROP_NO);

    i.addChild(atlasOpDefinition("update", "tick"));
    Atlas::Objects::Operation::UPDATE_NO = atlas_factories->addFactory("update", &Atlas::Objects::generic_factory);
    i.opInstall("update", Atlas::Objects::Operation::UPDATE_NO);

}

void installCustomEntities()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(atlasClass("room", "admin_entity"));

    i.addChild(atlasClass("lobby", "admin_entity"));

    i.addChild(atlasClass("server", "admin_entity"));

    i.addChild(atlasClass("task", "admin_entity"));
}
