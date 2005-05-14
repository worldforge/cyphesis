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
#include "Use.h"
#include "Wield.h"

#include "Generic.h"

#include <Atlas/Objects/Entity/AdminEntity.h>

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
using Atlas::Objects::Operation::Use;
using Atlas::Objects::Operation::Wield;

void installCustomOperations()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(new Add(Add::Class()));
    i.opInstall("add", OP_ADD, new OpFactory<Add>);

    i.addChild(new Burn(Burn::Class()));
    i.opInstall("burn", OP_BURN, new OpFactory<Burn>);

    i.addChild(new Chop(Chop::Class()));
    i.opInstall("chop", OP_CHOP, new OpFactory<Chop>);

    i.addChild(new Cut(Cut::Class()));
    i.opInstall("cut", OP_CUT, new OpFactory<Cut>);

    i.addChild(new Delve(Delve::Class()));
    i.opInstall("delve", OP_OTHER, new OpFactory<Delve>);

    i.addChild(new Dig(Dig::Class()));
    i.opInstall("dig", OP_OTHER, new OpFactory<Dig>);

    i.addChild(new Eat(Eat::Class()));
    i.opInstall("eat", OP_EAT, new OpFactory<Eat>);

    i.addChild(new Mow(Mow::Class()));
    i.opInstall("mow", OP_OTHER, new OpFactory<Mow>);

    i.addChild(new Nourish(Nourish::Class()));
    i.opInstall("nourish", OP_NOURISH, new OpFactory<Nourish>);

    i.addChild(new Setup(Setup::Class()));
    i.opInstall("setup", OP_SETUP, new OpFactory<Setup>);

    i.addChild(new Tick(Tick::Class()));
    i.opInstall("tick", OP_TICK, new OpFactory<Tick>);

    i.addChild(new Unseen(Unseen::Class()));
    i.opInstall("unseen", OP_UNSEEN, new OpFactory<Unseen>);

    i.addChild(new Use(Use::Class()));
    i.opInstall("use", OP_USE, new OpFactory<Use>);

    i.addChild(new Wield(Wield::Class()));
    i.opInstall("wield", OP_WIELD, new OpFactory<Wield>);

    // Custom ops used in scripts which do not need direct support in the
    // core

    i.opInstall("shoot", OP_OTHER, new GenericOpFactory("shoot"));
    i.opInstall("extinguish", OP_OTHER, new GenericOpFactory("extinguish"));
    i.opInstall("sow", OP_OTHER, new GenericOpFactory("sow"));
    i.opInstall("germinate", OP_OTHER, new GenericOpFactory("germinate"));
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
