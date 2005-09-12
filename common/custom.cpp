// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "inheritance.h"

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
    i.opInstall("add", OP_ADD);
    Atlas::Objects::Operation::ADD_NO = atlas_factories->addFactory("add", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("burn", "action"));
    i.opInstall("burn", OP_BURN);
    Atlas::Objects::Operation::BURN_NO = atlas_factories->addFactory("burn", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("chop", "affect"));
    i.opInstall("chop", OP_CHOP);
    Atlas::Objects::Operation::CHOP_NO = atlas_factories->addFactory("chop", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("cut", "action"));
    i.opInstall("cut", OP_CUT);
    Atlas::Objects::Operation::CUT_NO = atlas_factories->addFactory("cut", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("delve", "affect"));
    i.opInstall("delve", OP_OTHER);
    Atlas::Objects::Operation::DELVE_NO = atlas_factories->addFactory("delve", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("dig", "affect"));
    i.opInstall("dig", OP_OTHER);
    Atlas::Objects::Operation::DIG_NO = atlas_factories->addFactory("dig", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("eat", "action"));
    i.opInstall("eat", OP_EAT);
    Atlas::Objects::Operation::EAT_NO = atlas_factories->addFactory("eat", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("mow", "affect"));
    i.opInstall("mow", OP_OTHER);
    Atlas::Objects::Operation::MOW_NO = atlas_factories->addFactory("mow", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("nourish", "affect"));
    i.opInstall("nourish", OP_NOURISH);
    Atlas::Objects::Operation::NOURISH_NO = atlas_factories->addFactory("nourish", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("setup", "root_operation"));
    i.opInstall("setup", OP_SETUP);
    Atlas::Objects::Operation::SETUP_NO = atlas_factories->addFactory("setup", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("tick", "root_operation"));
    i.opInstall("tick", OP_TICK);
    Atlas::Objects::Operation::TICK_NO = atlas_factories->addFactory("tick", &Atlas::Objects::generic_factory);

    i.addChild(atlasOpDefinition("unseen", "perception"));
    i.opInstall("unseen", OP_UNSEEN);
    Atlas::Objects::Operation::UNSEEN_NO = atlas_factories->addFactory("unseen", &Atlas::Objects::generic_factory);

    // Custom ops used in scripts which do not need direct support in the
    // core

    i.opInstall("shoot", OP_OTHER);
    atlas_factories->addFactory("shoot", &Atlas::Objects::generic_factory);

    i.opInstall("extinguish", OP_OTHER);
    atlas_factories->addFactory("extinguish", &Atlas::Objects::generic_factory);

    i.opInstall("sow", OP_OTHER);
    atlas_factories->addFactory("sow", &Atlas::Objects::generic_factory);

    i.opInstall("germinate", OP_OTHER);
    atlas_factories->addFactory("germinate", &Atlas::Objects::generic_factory);

    i.opInstall("wear", OP_OTHER);
    atlas_factories->addFactory("wear", &Atlas::Objects::generic_factory);

    i.opInstall("ignite", OP_OTHER);
    atlas_factories->addFactory("ignite", &Atlas::Objects::generic_factory);

}

void installCustomEntities()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(atlasClass("room", "admin_entity"));

    i.addChild(atlasClass("lobby", "admin_entity"));

    i.addChild(atlasClass("server", "admin_entity"));
}
