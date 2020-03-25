/*
 Copyright (C) 2020 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "../TestBaseWithContext.h"
#include "../DatabaseNull.h"
#include "../TestWorld.h"
#include "common/Monitors.h"
#include "common/Inheritance.h"
#include "rules/simulation/World.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <rules/simulation/CorePropertyManager.h>

#include <utility>

using Atlas::Objects::Operation::Set;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

struct TestContext
{
    Atlas::Objects::Factories factories;
    DatabaseNull database;
    Ref<World> world;
    Inheritance inheritance;
    TestWorld testWorld;
    CorePropertyManager propertyManager;

    TestContext() :
            world(new World()),
            inheritance(factories),
            testWorld(world),
            propertyManager(inheritance)
    {
    }

};

OpVector resIgnored;

struct Tested : public Cyphesis::TestBaseWithContext<TestContext>
{
    Tested()
    {
        ADD_TEST(test_setModify);
        ADD_TEST(test_setModifySelf);
        ADD_TEST(test_setProps);
    }

    static void sendSetOp(const Ref<Thing>& entity, const std::string& propertyName, Atlas::Message::Element value)
    {
        Set set;

        Anonymous arg1;
        arg1->setAttr(propertyName, std::move(value));
        set->setArgs1(arg1);

        entity->operation(set, resIgnored);
    }

    void test_setProps(const TestContext& context)
    {
        Ref<Thing> entity(new Thing("1", 1));

        ASSERT_FALSE(entity->getAttr("foo"));

        sendSetOp(entity, "foo", 1);

        ASSERT_TRUE(entity->getAttr("foo"));
        ASSERT_EQUAL(*entity->getAttr("foo"), 1);

        sendSetOp(entity, "foo!append", 2);
        ASSERT_EQUAL(*entity->getAttr("foo"), 3);

        sendSetOp(entity, "foo!prepend", 2);
        ASSERT_EQUAL(*entity->getAttr("foo"), 5);

        sendSetOp(entity, "foo!subtract", 3);
        ASSERT_EQUAL(*entity->getAttr("foo"), 2);
    }

    void test_setModifySelf(const TestContext& context)
    {
        Ref<Thing> entity(new Thing("1", 1));
        sendSetOp(entity, "modify_self", MapType{{"set_by_test",
                                                         MapType{
                                                                 {"constraint", "true = true"},
                                                                 {"modifiers",  MapType{
                                                                         {"foo", MapType{
                                                                                 {"append", 1.0}}},
                                                                         {"bar", MapType{
                                                                                 {"default", 1.0}}}
                                                                 }}
                                                         }}});

        ASSERT_TRUE(entity->getAttr("foo"));
        ASSERT_EQUAL(*entity->getAttr("foo"), 1.0);

        ASSERT_TRUE(entity->getAttr("bar"));
        ASSERT_EQUAL(*entity->getAttr("bar"), 1.0);

        sendSetOp(entity, "foo", 2.0);
        ASSERT_EQUAL(*entity->getAttr("foo"), 3.0);
    }

    void test_setModify(const TestContext& context)
    {
        Ref<Thing> entity(new Thing("1", 1));
        Ref<Thing> entityChild(new Thing("2", 2));
        entity->addChild(*entityChild);
        sendSetOp(entityChild, "modify", ListType{
                MapType{
                        {"constraint", "true = true"},
                        {"modifiers",  MapType{
                                {"foo", MapType{
                                        {"append", 1.0}}},
                                {"bar", MapType{
                                        {"default", 1.0}}}
                        }}
                }});

        ASSERT_TRUE(entity->getAttr("foo"));
        ASSERT_EQUAL(*entity->getAttr("foo"), 1.0);

        ASSERT_TRUE(entity->getAttr("bar"));
        ASSERT_EQUAL(*entity->getAttr("bar"), 1.0);

        sendSetOp(entity, "foo", 2.0);
        ASSERT_EQUAL(*entity->getAttr("foo"), 3.0);

        entityChild->destroy();
    }


};


int main()
{
    Monitors m;
    Tested t;

    return t.run();
}
