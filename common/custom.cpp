// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "inheritance.h"
#include "operations.h"

#include "Chop.h"
#include "Cut.h"
#include "Eat.h"
#include "Burn.h"
#include "Generic.h"
#include "Nourish.h"
#include "Setup.h"
#include "Tick.h"

#include <Atlas/Objects/Entity/AdminEntity.h>

void installCustomOperations()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(new Chop(Chop::Class()));
    i.opInstall("chop", OP_CHOP);
    i.addChild(new Cut(Cut::Class()));
    i.opInstall("cut", OP_CUT);
    i.addChild(new Eat(Eat::Class()));
    i.opInstall("eat", OP_EAT);
    i.addChild(new Burn(Burn::Class()));
    i.opInstall("burn", OP_BURN);
    i.addChild(new Nourish(Nourish::Class()));
    i.opInstall("nourish", OP_NOURISH);
    i.addChild(new Setup(Setup::Class()));
    i.opInstall("setup", OP_SETUP);
    i.addChild(new Tick(Tick::Class()));
    i.opInstall("tick", OP_TICK);


    // Custom ops used in scripts which do not need direct support in the
    // core

    i.opInstall("shoot", OP_OTHER);
    i.opInstall("extinguish", OP_OTHER);
}

using Atlas::Objects::Root;
using Atlas::Objects::Entity::AdminEntity;

void installCustomEntities()
{
    Inheritance & i = Inheritance::instance();

    Root * r = new AdminEntity(AdminEntity::Class());
    r->setId("room");
    r->setParents(Element::ListType(1,"admin_entity"));
    i.addChild(r);

    r = new AdminEntity(AdminEntity::Class());
    r->setId("lobby");
    r->setParents(Element::ListType(1,"room"));
    i.addChild(r);
}
