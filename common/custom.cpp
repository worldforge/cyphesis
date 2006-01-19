// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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

#include "inheritance.h"

#include "Add.h"
#include "Attack.h"
#include "Burn.h"
#include "Chop.h"
#include "Cut.h"
#include "Delve.h"
#include "Dig.h"
#include "Eat.h"
#include "Monitor.h"
#include "Mow.h"
#include "Nourish.h"
#include "Setup.h"
#include "Tick.h"
#include "Unseen.h"

#include <Atlas/Objects/Entity.h>

using Atlas::Objects::Root;
using Atlas::Objects::Entity::AdminEntity;
using Atlas::Objects::Operation::Add;
using Atlas::Objects::Operation::Burn;
using Atlas::Objects::Operation::Cut;
using Atlas::Objects::Operation::Chop;
using Atlas::Objects::Operation::Delve;
using Atlas::Objects::Operation::Dig;
using Atlas::Objects::Operation::Eat;
using Atlas::Objects::Operation::Monitor;
using Atlas::Objects::Operation::Mow;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Unseen;

void installCustomOperations()
{
    Inheritance & i = Inheritance::instance();
    Atlas::Objects::Factories * atlas_factories = Atlas::Objects::Factories::instance();

    i.addChild(atlasOpDefinition("add", "set"));
    Atlas::Objects::Operation::ADD_NO = atlas_factories->addFactory("add", &Atlas::Objects::generic_factory);
    i.opInstall("add", OP_ADD);

    i.addChild(atlasOpDefinition("burn", "action"));
    Atlas::Objects::Operation::BURN_NO = atlas_factories->addFactory("burn", &Atlas::Objects::generic_factory);
    i.opInstall("burn", OP_BURN);

    i.addChild(atlasOpDefinition("chop", "affect"));
    Atlas::Objects::Operation::CHOP_NO = atlas_factories->addFactory("chop", &Atlas::Objects::generic_factory);
    i.opInstall("chop", OP_CHOP);

    i.addChild(atlasOpDefinition("cut", "action"));
    Atlas::Objects::Operation::CUT_NO = atlas_factories->addFactory("cut", &Atlas::Objects::generic_factory);
    i.opInstall("cut", OP_CUT);

    i.addChild(atlasOpDefinition("delve", "affect"));
    Atlas::Objects::Operation::DELVE_NO = atlas_factories->addFactory("delve", &Atlas::Objects::generic_factory);
    i.opInstall("delve", OP_DELVE);

    i.addChild(atlasOpDefinition("dig", "affect"));
    Atlas::Objects::Operation::DIG_NO = atlas_factories->addFactory("dig", &Atlas::Objects::generic_factory);
    i.opInstall("dig", OP_DIG);

    i.addChild(atlasOpDefinition("eat", "action"));
    Atlas::Objects::Operation::EAT_NO = atlas_factories->addFactory("eat", &Atlas::Objects::generic_factory);
    i.opInstall("eat", OP_EAT);

    i.addChild(atlasOpDefinition("monitor", "affect"));
    Atlas::Objects::Operation::MONITOR_NO = atlas_factories->addFactory("monitor", &Atlas::Objects::generic_factory);
    i.opInstall("monitor", OP_MONITOR);

    i.addChild(atlasOpDefinition("mow", "affect"));
    Atlas::Objects::Operation::MOW_NO = atlas_factories->addFactory("mow", &Atlas::Objects::generic_factory);
    i.opInstall("mow", OP_MOW);

    i.addChild(atlasOpDefinition("nourish", "affect"));
    Atlas::Objects::Operation::NOURISH_NO = atlas_factories->addFactory("nourish", &Atlas::Objects::generic_factory);
    i.opInstall("nourish", OP_NOURISH);

    i.addChild(atlasOpDefinition("setup", "root_operation"));
    Atlas::Objects::Operation::SETUP_NO = atlas_factories->addFactory("setup", &Atlas::Objects::generic_factory);
    i.opInstall("setup", OP_SETUP);

    i.addChild(atlasOpDefinition("tick", "root_operation"));
    Atlas::Objects::Operation::TICK_NO = atlas_factories->addFactory("tick", &Atlas::Objects::generic_factory);
    i.opInstall("tick", OP_TICK);

    i.addChild(atlasOpDefinition("unseen", "perception"));
    Atlas::Objects::Operation::UNSEEN_NO = atlas_factories->addFactory("unseen", &Atlas::Objects::generic_factory);
    i.opInstall("unseen", OP_UNSEEN);

    i.addChild(atlasOpDefinition("attack", "action"));
    Atlas::Objects::Operation::ATTACK_NO = atlas_factories->addFactory("attack", &Atlas::Objects::generic_factory);
    i.opInstall("attack", OP_ATTACK);

}

void installCustomEntities()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(atlasClass("room", "admin_entity"));

    i.addChild(atlasClass("lobby", "admin_entity"));

    i.addChild(atlasClass("server", "admin_entity"));
}
