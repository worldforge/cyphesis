// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/RootOperation.h>
#include <Atlas/Objects/Entity/AdminEntity.h>
#include "Chop.h"
#include "Cut.h"
#include "Eat.h"
#include "Fire.h"
#include "Generic.h"
#include "Load.h"
#include "Nourish.h"
#include "Save.h"
#include "Setup.h"
#include "Tick.h"

#include "inheritance.h"
#include "operations.h"

void installCustomOperations()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(new Chop());
    i.addChild(new Cut());
    i.addChild(new Eat());
    i.addChild(new Fire());
    i.addChild(new Load());
    i.addChild(new Nourish());
    i.addChild(new Save());
    i.addChild(new Setup());
    i.addChild(new Tick());
}

using Atlas::Objects::Root;
using Atlas::Objects::Entity::AdminEntity;
using Atlas::Message::Object;

void installCustomEntities()
{
    Inheritance & i = Inheritance::instance();

    Root * r = new AdminEntity();
    r->SetId("room");
    r->SetParents(Object::ListType(1,"admin_entity"));
    i.addChild(r);

    r = new AdminEntity();
    r->SetId("lobby");
    r->SetParents(Object::ListType(1,"room"));
    i.addChild(r);
}
