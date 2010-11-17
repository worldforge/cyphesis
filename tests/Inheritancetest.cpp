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

// $Id$

#include "common/Inheritance.h"

#include "common/log.h"
#include "common/OperationRouter.h"
#include "common/TypeNode.h"

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

    assert(i.opEnumerate("login") == Atlas::Objects::Operation::LOGIN_NO);
    assert(i.opEnumerate("logout") == Atlas::Objects::Operation::LOGOUT_NO);
    assert(i.opEnumerate("action") == Atlas::Objects::Operation::ACTION_NO);
    assert(i.opEnumerate("combine") == Atlas::Objects::Operation::COMBINE_NO);
    assert(i.opEnumerate("create") == Atlas::Objects::Operation::CREATE_NO);
    assert(i.opEnumerate("delete") == Atlas::Objects::Operation::DELETE_NO);
    assert(i.opEnumerate("divide") == Atlas::Objects::Operation::DIVIDE_NO);
    assert(i.opEnumerate("eat") == Atlas::Objects::Operation::EAT_NO);
    assert(i.opEnumerate("burn") == Atlas::Objects::Operation::BURN_NO);
    assert(i.opEnumerate("get") == Atlas::Objects::Operation::GET_NO);
    assert(i.opEnumerate("imaginary") == Atlas::Objects::Operation::IMAGINARY_NO);
    assert(i.opEnumerate("info") == Atlas::Objects::Operation::INFO_NO);
    assert(i.opEnumerate("move") == Atlas::Objects::Operation::MOVE_NO);
    assert(i.opEnumerate("nourish") == Atlas::Objects::Operation::NOURISH_NO);
    assert(i.opEnumerate("set") == Atlas::Objects::Operation::SET_NO);
    assert(i.opEnumerate("sight") == Atlas::Objects::Operation::SIGHT_NO);
    assert(i.opEnumerate("sound") == Atlas::Objects::Operation::SOUND_NO);
    assert(i.opEnumerate("talk") == Atlas::Objects::Operation::TALK_NO);
    assert(i.opEnumerate("touch") == Atlas::Objects::Operation::TOUCH_NO);
    assert(i.opEnumerate("tick") == Atlas::Objects::Operation::TICK_NO);
    assert(i.opEnumerate("look") == Atlas::Objects::Operation::LOOK_NO);
    assert(i.opEnumerate("setup") == Atlas::Objects::Operation::SETUP_NO);
    assert(i.opEnumerate("appearance") == Atlas::Objects::Operation::APPEARANCE_NO);
    assert(i.opEnumerate("disappearance") == Atlas::Objects::Operation::DISAPPEARANCE_NO);
    assert(i.opEnumerate("error") == Atlas::Objects::Operation::ERROR_NO);
    assert(i.opEnumerate("squigglymuff") == OP_INVALID);

    const int SQUIGGLYMUFF_NO = Atlas::Objects::Factories::instance()->addFactory("squigglymuff", &Atlas::Objects::generic_factory);
    
    i.opInstall("squigglymuff", SQUIGGLYMUFF_NO);

    // We haven't actually added the class yet
    assert(!i.hasClass("squigglymuff"));

    assert(i.opEnumerate("squigglymuff") != OP_INVALID);
    assert(i.opEnumerate("squigglymuff") == SQUIGGLYMUFF_NO);

    assert(i.opEnumerate(Atlas::Objects::Operation::Login()) == Atlas::Objects::Operation::LOGIN_NO);
    Root login_obj = Atlas::Objects::Factories::instance()->createObject("login");
    RootOperation login_op = smart_dynamic_cast<RootOperation>(login_obj);
    assert(login_op.isValid());
    assert(i.opEnumerate(login_op) == Atlas::Objects::Operation::LOGIN_NO);

    Root squigglymuff_obj = Atlas::Objects::Factories::instance()->createObject("squigglymuff");
    RootOperation squigglymuff_op = smart_dynamic_cast<RootOperation>(squigglymuff_obj);
    assert(squigglymuff_op.isValid());
    assert(i.opEnumerate(squigglymuff_op) == SQUIGGLYMUFF_NO);

    assert(i.hasClass("root"));
    assert(i.hasClass("root_entity"));
    assert(i.hasClass("root_operation"));
    assert(i.hasClass("login"));
    
    const TypeNode * rt_node = i.getType("root");
    assert(rt_node != 0);

    const Root & rt = i.getClass("root");

    // Make sure the type tree is coherent, and contains a decent
    // number of types.
    int count = 0;
    descendTree(rt, i, count);
    assert(count > 20);

    {
        Root r;

        r->setId("squigglymuff");
        r->setParents(std::list<std::string>(1, "ludricous_test_parent"));
        
        int ret = i.updateClass("squigglymuff", r);
        assert(ret == -1);
    }

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

    {
        Root r;

        r->setId("squigglymuff");
        r->setParents(std::list<std::string>(1, "root_operation"));
        
        int ret = i.updateClass("squigglymuff", r);
        assert(ret == 0);
    }

    // Make sure adding a duplicate fails. 
    r = Root();
    r->setId("squigglymuff");
    r->setParents(std::list<std::string>(1, "root_operation"));
    assert(i.addChild(r) == 0);

    assert(!i.isTypeOf("ludricous_test_parent", "root_operation"));
    assert(i.isTypeOf("disappearance", "root_operation"));
    assert(i.isTypeOf("root_operation", "root_operation"));
    assert(!i.isTypeOf("root_operation", "talk"));

    // Make sure it clears out okay
    i.flush();
    assert(i.opEnumerate("login") == OP_INVALID);
    assert(i.opEnumerate("squigglymuff") == OP_INVALID);
    assert(i.opEnumerate(Atlas::Objects::Operation::Login()) == Atlas::Objects::Operation::LOGIN_NO);
    assert(i.opEnumerate(login_op) == Atlas::Objects::Operation::LOGIN_NO);
    assert(i.opEnumerate(squigglymuff_op) == SQUIGGLYMUFF_NO);

    // Make sure the type for root can no longer be retrieved
    const TypeNode * no_root_node = i.getType("root");
    assert(no_root_node == 0);
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
    
    // Clean up and delete it
    Inheritance::clear();
}

// stubs

namespace Atlas { namespace Objects { namespace Operation {

int ACTUATE_NO = -1;
int ADD_NO = -1;
int ATTACK_NO = -1;
int BURN_NO = -1;
int CONNECT_NO = -1;
int DROP_NO = -1;
int MONITOR_NO = -1;
int EAT_NO = -1;
int UNSEEN_NO = -1;
int UPDATE_NO = -1;
int NOURISH_NO = -1;
int PICKUP_NO = -1;
int SETUP_NO = -1;
int TICK_NO = -1;
int THOUGHT_NO = -1;
int GOAL_INFO_NO = -1;
int TELEPORT_NO = -1;

} } }

TypeNode::TypeNode() : m_parent(0)
{
}

TypeNode::~TypeNode()
{
}

void log(LogLevel lvl, const std::string & msg)
{
}
