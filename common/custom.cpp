// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "inheritance_impl.h"

#include "Burn.h"
#include "Chop.h"
#include "Cut.h"
#include "Eat.h"
#include "Nourish.h"
#include "Setup.h"
#include "Tick.h"
#include "Use.h"
#include "Wield.h"

#include "Generic.h"

#include <Atlas/Objects/Entity/AdminEntity.h>

using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::AdminEntity;
using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Eat;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Cut;
using Atlas::Objects::Operation::Chop;
using Atlas::Objects::Operation::Burn;
using Atlas::Objects::Operation::Use;
using Atlas::Objects::Operation::Wield;

void installCustomOperations()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(new Chop(Chop::Class()));
    i.opInstall("chop", OP_CHOP, new OpFactory<Chop>);
    i.addChild(new Cut(Cut::Class()));
    i.opInstall("cut", OP_CUT, new OpFactory<Cut>);
    i.addChild(new Eat(Eat::Class()));
    i.opInstall("eat", OP_EAT, new OpFactory<Eat>);
    i.addChild(new Burn(Burn::Class()));
    i.opInstall("burn", OP_BURN, new OpFactory<Burn>);
    i.addChild(new Nourish(Nourish::Class()));
    i.opInstall("nourish", OP_NOURISH, new OpFactory<Nourish>);
    i.addChild(new Setup(Setup::Class()));
    i.opInstall("setup", OP_SETUP, new OpFactory<Setup>);
    i.addChild(new Tick(Tick::Class()));
    i.opInstall("tick", OP_TICK, new OpFactory<Tick>);
    i.addChild(new Use(Use::Class()));
    i.opInstall("use", OP_USE, new OpFactory<Use>);
    i.addChild(new Wield(Wield::Class()));
    i.opInstall("wield", OP_WIELD, new OpFactory<Wield>);

    // Custom ops used in scripts which do not need direct support in the
    // core

    i.opInstall("shoot", OP_OTHER, new GenericOpFactory("shoot"));
    i.opInstall("extinguish", OP_OTHER, new GenericOpFactory("extinguish"));
}

void installCustomEntities()
{
    Inheritance & i = Inheritance::instance();

    Root * r = new AdminEntity(AdminEntity::Class());
    r->setId("room");
    r->setParents(ListType(1,"admin_entity"));
    i.addChild(r);

    r = new AdminEntity(AdminEntity::Class());
    r->setId("lobby");
    r->setParents(ListType(1,"room"));
    i.addChild(r);
}
