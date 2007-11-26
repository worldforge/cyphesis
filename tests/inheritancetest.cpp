// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

// $Id: inheritancetest.cpp,v 1.19 2007-11-26 02:57:06 alriddoch Exp $

#include "common/inheritance.h"

#include "common/Eat.h"
#include "common/Burn.h"
#include "common/Nourish.h"
#include "common/Setup.h"
#include "common/Tick.h"

#include <Atlas/Objects/Operation.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Operation::RootOperation;

void descendTree(const Root & type, Inheritance & i, int & count)
{
    assert(type.isValid());
    ++count;
    if (!type->hasAttr("children")) {
        return;
    }

    Element childrenobj = type->getAttr("children");
    assert(childrenobj.isList());
    ListType & children = childrenobj.asList();

    
    ListType::const_iterator Iend = children.end();
    for (ListType::const_iterator I = children.begin(); I != Iend; ++I) {
        const Element & e = *I;
        assert(e.isString());
        const std::string & es = e.asString();
        assert(!es.empty());
        const Root & child = i.getClass(es);
        descendTree(child, i, count);
    }

}

int main()
{
    Inheritance & i = Inheritance::instance();

    assert(&i == &Inheritance::instance());

    assert(i.opEnumerate("login") == OP_LOGIN);
    assert(i.opEnumerate("logout") == OP_LOGOUT);
    assert(i.opEnumerate("action") == OP_ACTION);
    assert(i.opEnumerate("combine") == OP_COMBINE);
    assert(i.opEnumerate("create") == OP_CREATE);
    assert(i.opEnumerate("delete") == OP_DELETE);
    assert(i.opEnumerate("divide") == OP_DIVIDE);
    assert(i.opEnumerate("eat") == OP_EAT);
    assert(i.opEnumerate("burn") == OP_BURN);
    assert(i.opEnumerate("get") == OP_GET);
    assert(i.opEnumerate("imaginary") == OP_IMAGINARY);
    assert(i.opEnumerate("info") == OP_INFO);
    assert(i.opEnumerate("move") == OP_MOVE);
    assert(i.opEnumerate("nourish") == OP_NOURISH);
    assert(i.opEnumerate("set") == OP_SET);
    assert(i.opEnumerate("sight") == OP_SIGHT);
    assert(i.opEnumerate("sound") == OP_SOUND);
    assert(i.opEnumerate("talk") == OP_TALK);
    assert(i.opEnumerate("touch") == OP_TOUCH);
    assert(i.opEnumerate("tick") == OP_TICK);
    assert(i.opEnumerate("look") == OP_LOOK);
    assert(i.opEnumerate("setup") == OP_SETUP);
    assert(i.opEnumerate("appearance") == OP_APPEARANCE);
    assert(i.opEnumerate("disappearance") == OP_DISAPPEARANCE);
    assert(i.opEnumerate("error") == OP_ERROR);
    assert(i.opEnumerate("squigglymuff") == OP_INVALID);

    const int OP_SQUIGGLYMUFF = Atlas::Objects::Factories::instance()->addFactory("squigglymuff", &Atlas::Objects::generic_factory);
    
    i.opInstall("squigglymuff", OP_SQUIGGLYMUFF);

    // We haven't actually added the class yet
    assert(!i.hasClass("squigglymuff"));

    assert(i.opEnumerate("squigglymuff") != OP_INVALID);
    assert(i.opEnumerate("squigglymuff") == OP_SQUIGGLYMUFF);

    assert(i.opEnumerate(Atlas::Objects::Operation::Login()) == OP_LOGIN);
    Root login_obj = Atlas::Objects::Factories::instance()->createObject("login");
    RootOperation login_op = smart_dynamic_cast<RootOperation>(login_obj);
    assert(login_op.isValid());
    assert(i.opEnumerate(login_op) == OP_LOGIN);

    Root squigglymuff_obj = Atlas::Objects::Factories::instance()->createObject("squigglymuff");
    RootOperation squigglymuff_op = smart_dynamic_cast<RootOperation>(squigglymuff_obj);
    assert(squigglymuff_op.isValid());
    assert(i.opEnumerate(squigglymuff_op) == OP_SQUIGGLYMUFF);

    assert(i.hasClass("root"));
    assert(i.hasClass("root_entity"));
    assert(i.hasClass("root_operation"));
    assert(i.hasClass("login"));
    
    const Root & rt = i.getClass("root");

    // Make sure the type tree is coherent, and contains a decent
    // number of types.
    int count = 0;
    descendTree(rt, i, count);
    assert(count > 20);

    // Make sure inserting a type with unknown parents fails with null
    Root r;
    r->setId("squigglymuff");
    r->setParents(std::list<std::string>(1, "ludricous_test_parent"));
    assert(i.addChild(r) == 0);

    assert(!i.hasClass("squigglymuff"));

    r->setId("squigglymuff");
    r->setParents(std::list<std::string>(1, "root_operation"));
    assert(i.addChild(r) != 0);

    assert(i.hasClass("squigglymuff"));

    assert(i.isTypeOf("disappearance", "root_operation"));
    assert(i.isTypeOf("root_operation", "root_operation"));
    assert(!i.isTypeOf("root_operation", "talk"));

    // Make sure it clears out okay
    i.flush();
    assert(i.opEnumerate("login") == OP_INVALID);
    assert(i.opEnumerate("squigglymuff") == OP_INVALID);
    assert(i.opEnumerate(Atlas::Objects::Operation::Login()) == OP_LOGIN);
    assert(i.opEnumerate(login_op) == OP_LOGIN);
    assert(i.opEnumerate(squigglymuff_op) == OP_SQUIGGLYMUFF);

    // Make sure the type for root can no longer be retrieved
    const Root & non_root = i.getClass("root");
    assert(i.getAllObjects().empty());
    assert(!non_root.isValid());

    // Make sure installing a child of root still fails.
    r->setId("squigglymuff");
    r->setParents(std::list<std::string>(1, "root"));
    assert(i.addChild(r) == 0);

    assert(!i.hasClass("root"));
    assert(!i.hasClass("root_entity"));
    assert(!i.hasClass("root_operation"));
    assert(!i.hasClass("login"));
    assert(!i.hasClass("squigglymuff"));
    
}
