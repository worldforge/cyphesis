// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "inheritance_impl.h"

#include "Add.h"
#include "Burn.h"
#include "Chop.h"
#include "Cut.h"
#include "Delve.h"
#include "Dig.h"
#include "Eat.h"
#include "Mow.h"
#include "Nourish.h"
#include "Setup.h"
#include "Tick.h"
#include "Unseen.h"

#include <Atlas/Objects/Entity.h>

using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::AdminEntity;
using Atlas::Objects::Operation::Add;
using Atlas::Objects::Operation::Burn;
using Atlas::Objects::Operation::Cut;
using Atlas::Objects::Operation::Chop;
using Atlas::Objects::Operation::Delve;
using Atlas::Objects::Operation::Dig;
using Atlas::Objects::Operation::Eat;
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
    i.opInstall("add", OP_ADD, new OpFactory<Add>);
    atlas_factories->addFactory("add", &Atlas::Objects::factory<Atlas::Objects::Operation::AddData>);

    i.addChild(atlasOpDefinition("burn", "action"));
    i.opInstall("burn", OP_BURN, new OpFactory<Burn>);
    atlas_factories->addFactory("burn", &Atlas::Objects::factory<Atlas::Objects::Operation::BurnData>);

    i.addChild(atlasOpDefinition("chop", "affect"));
    i.opInstall("chop", OP_CHOP, new OpFactory<Chop>);
    atlas_factories->addFactory("chop", &Atlas::Objects::factory<Atlas::Objects::Operation::ChopData>);

    i.addChild(atlasOpDefinition("cut", "action"));
    i.opInstall("cut", OP_CUT, new OpFactory<Cut>);
    atlas_factories->addFactory("cut", &Atlas::Objects::factory<Atlas::Objects::Operation::CutData>);

    i.addChild(atlasOpDefinition("delve", "affect"));
    i.opInstall("delve", OP_OTHER, new OpFactory<Delve>);
    atlas_factories->addFactory("delve", &Atlas::Objects::factory<Atlas::Objects::Operation::DelveData>);

    i.addChild(atlasOpDefinition("dig", "affect"));
    i.opInstall("dig", OP_OTHER, new OpFactory<Dig>);
    atlas_factories->addFactory("dig", &Atlas::Objects::factory<Atlas::Objects::Operation::DigData>);

    i.addChild(atlasOpDefinition("eat", "action"));
    i.opInstall("eat", OP_EAT, new OpFactory<Eat>);
    atlas_factories->addFactory("eat", &Atlas::Objects::factory<Atlas::Objects::Operation::EatData>);

    i.addChild(atlasOpDefinition("mow", "affect"));
    i.opInstall("mow", OP_OTHER, new OpFactory<Mow>);
    atlas_factories->addFactory("mow", &Atlas::Objects::factory<Atlas::Objects::Operation::MowData>);

    i.addChild(atlasOpDefinition("nourish", "affect"));
    i.opInstall("nourish", OP_NOURISH, new OpFactory<Nourish>);
    atlas_factories->addFactory("nourish", &Atlas::Objects::factory<Atlas::Objects::Operation::NourishData>);

    i.addChild(atlasOpDefinition("setup", "root_operation"));
    i.opInstall("setup", OP_SETUP, new OpFactory<Setup>);
    atlas_factories->addFactory("setup", &Atlas::Objects::factory<Atlas::Objects::Operation::SetupData>);

    i.addChild(atlasOpDefinition("tick", "root_operation"));
    i.opInstall("tick", OP_TICK, new OpFactory<Tick>);
    atlas_factories->addFactory("tick", &Atlas::Objects::factory<Atlas::Objects::Operation::TickData>);

    i.addChild(atlasOpDefinition("unseen", "perception"));
    i.opInstall("unseen", OP_UNSEEN, new OpFactory<Unseen>);
    atlas_factories->addFactory("unseen", &Atlas::Objects::factory<Atlas::Objects::Operation::UnseenData>);

    // Custom ops used in scripts which do not need direct support in the
    // core

    i.opInstall("shoot", OP_OTHER, new GenericOpFactory("shoot"));
    i.opInstall("extinguish", OP_OTHER, new GenericOpFactory("extinguish"));
    i.opInstall("sow", OP_OTHER, new GenericOpFactory("sow"));
    i.opInstall("germinate", OP_OTHER, new GenericOpFactory("germinate"));
    i.opInstall("wear", OP_OTHER, new GenericOpFactory("wear"));
    // i.opInstall("ignite", OP_OTHER, new GenericOpFactory("ignite"));
}

void installCustomEntities()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(atlasClass("room", "admin_entity"));

    i.addChild(atlasClass("lobby", "admin_entity"));

    i.addChild(atlasClass("server", "admin_entity"));
}
