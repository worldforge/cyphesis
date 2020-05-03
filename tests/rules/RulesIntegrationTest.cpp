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
#include "../NullEntityCreator.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <rules/simulation/CorePropertyManager.h>

#include <memory>
#include <rules/simulation/ModifiersProperty.h>
#include <rules/simulation/WorldRouter.h>

using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

struct TestContext
{
    Atlas::Objects::Factories factories;
    DatabaseNull database;
    Ref<World> world;
    Inheritance inheritance;
    WorldRouter testWorld;
    NullEntityCreator entityCreator;
    CorePropertyManager propertyManager;

    TestContext() :
            world(new World()),
            inheritance(factories),
            testWorld(world, entityCreator),
            propertyManager(inheritance)
    {
    }

};

OpVector resIgnored;

struct Tested : public Cyphesis::TestBaseWithContext<TestContext>
{
    Tested()
    {
        ADD_TEST(test_delete_entity);
        ADD_TEST(test_modifyWithAttachConstraints);
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


    void test_delete_entity(TestContext& context)
    {
        WFMath::AxisBox<3> bbox(WFMath::Point<3>(-10, -10, -10), WFMath::Point<3>(10, 10, 10));


        /**
         * Handle the case where there's a physical domain and an Inventory domain.
         *
         * All entities are placed at origo
         * Hierarchy looks like this:
         * T1 has a physical domain
         * T2 has a container domain
         * T3, T4, T5 has no domain
         *
         *              T1#
         *         T2*       T3
         *         T4        T5
         */
        {
            Ref<Thing> t1 = new Thing("1", 1);
            t1->setAttrValue("domain", "physical");
            Ref<Thing> t2 = new Thing("2", 2);
            t2->m_location.m_pos = {10, 0, 20};
            t2->m_location.setBBox(bbox);
            t2->m_location.m_orientation = WFMath::Quaternion(1, 2.0);
            t2->setAttrValue("domain", "container");
            Ref<Thing> t3 = new Thing("3", 3);
            t3->m_location.m_pos =  {20, 0, 20};
            t3->m_location.setBBox(bbox);
            Ref<Thing> t4 = new Thing("4", 4);
            t4->m_location.m_pos = WFMath::Point<3>::ZERO();
            t4->m_location.setBBox(bbox);
            t4->m_location.m_pos =  {30, 0, 20};
            Ref<Thing> t5 = new Thing("5", 5);
            t5->m_location.m_pos = WFMath::Point<3>::ZERO();
            t5->m_location.setBBox(bbox);
            t5->m_location.m_pos =  {40, 0, 20};

            t1->addChild(*t2);
            t1->addChild(*t3);
            t2->addChild(*t4);
            t3->addChild(*t5);

            OpVector res;
            t2->DeleteOperation({},res);
            ASSERT_TRUE(t2->isDestroyed())
            ASSERT_EQUAL(t1->getId(), t4->m_location.m_parent->getId())
            ASSERT_EQUAL(t4->m_location.m_pos, WFMath::Point<3>(10, 0, 20))
            ASSERT_EQUAL(t4->m_location.m_orientation, WFMath::Quaternion(1, 2.0))

            t3->DeleteOperation({},res);
            ASSERT_TRUE(t3->isDestroyed())
            ASSERT_EQUAL(t1->getId(), t5->m_location.m_parent->getId())
            ASSERT_EQUAL(t5->m_location.m_pos, WFMath::Point<3>(20, 0, 20))

        }
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

        //Removing the "bar" modifier should remove the "bar" property.
        sendSetOp(entity, "modify_self", MapType{{"set_by_test",
                                                         MapType{
                                                                 {"constraint", "true = true"},
                                                                 {"modifiers",  MapType{
                                                                         {"foo", MapType{
                                                                                 {"append", 1.0}}}
                                                                 }}
                                                         }}});

        ASSERT_EQUAL(*entity->getAttr("bar"), Atlas::Message::Element());

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

        //Trying to alter "_modifiers" should not be possible.
        sendSetOp(entity, ModifiersProperty::property_name, ListType{});

        ASSERT_TRUE(entity->getAttr("bar"));
        ASSERT_EQUAL(*entity->getAttr("bar"), 1.0);

        //Removing the "bar" modifier should remove the "bar" property.
        sendSetOp(entityChild, "modify", ListType{
                MapType{
                        {"constraint", "true = true"},
                        {"modifiers",  MapType{
                                {"foo", MapType{
                                        {"append", 1.0}}}
                        }}
                }});

        ASSERT_EQUAL(*entity->getAttr("bar"), Atlas::Message::Element());


        entityChild->destroy();

    }

    void test_modifyWithAttachConstraints(TestContext& context)
    {
        Ref<Thing> entity(new Thing("1", 1));
        context.testWorld.addEntity(entity, context.world);
        sendSetOp(entity, "attachments", MapType{{"hand_primary", "contains(actor.contains, child = tool)"}});

        Ref<Thing> entityChild(new Thing("2", 2));
        context.testWorld.addEntity(entityChild, entity);
        sendSetOp(entityChild, "modify", ListType{
                MapType{
                        {"constraint",          "entity.mode_data.mode = 'planted'"},
                        {"modifiers",           MapType{
                                {"foo", MapType{
                                        {"append", 1.0}}},
                                {"bar", MapType{
                                        {"default", 1.0}}}
                        }},
                        {"observed_properties", ListType{"attached_hand_primary"}}
                }});

        ASSERT_FALSE(entity->getAttr("foo"))

        {
            Wield wield;
            Anonymous arg1;
            arg1->setAttr("attachment", "hand_primary");
            arg1->setId(entityChild->getId());
            wield->setArgs1(arg1);
            entity->operation(wield, resIgnored);
        }

        ASSERT_TRUE(entity->getAttr("foo"))
        ASSERT_EQUAL(*entity->getAttr("foo"), 1.0)


        entityChild->destroy();
    }

};


int main()
{
    Monitors m;
    Tested t;

    return t.run();
}
