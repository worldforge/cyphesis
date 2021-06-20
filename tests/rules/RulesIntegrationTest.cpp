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
#include "common/operations/Thought.h"
#include "rules/simulation/World.h"
#include "../NullEntityCreator.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <wfmath/atlasconv.h>
#include <rules/simulation/CorePropertyManager.h>

#include <memory>
#include <rules/simulation/ModifiersProperty.h>
#include <rules/simulation/WorldRouter.h>
#include <rules/simulation/MindsProperty.h>
#include <rules/BBoxProperty.h>
#include "rules/AtlasProperties.h"
#include "rules/PhysicalProperties.h"

using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Thought;
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
    long entityId;

    TestContext() :
            world(new World()),
            inheritance(factories),
            testWorld(world, entityCreator, [] { return std::chrono::steady_clock::now().time_since_epoch(); }),
            propertyManager(inheritance),
            entityId(1)
    {
    }

    ~TestContext()
    {
        testWorld.shutdown();
    }
};

OpVector resIgnored;

struct Tested : public Cyphesis::TestBaseWithContext<TestContext>
{
    Tested()
    {
        ADD_TEST(test_move_entity);
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

    void test_move_entity(TestContext& context)
    {
        auto thinkMoveFn = [&](const Ref<LocatedEntity>& destination, Anonymous ent) {

            Move move;
            move->setArgs1(ent);
            Thought thought;
            thought->setArgs1(move);
            OpVector res;
            destination->operation(thought, res);
            for (auto& op : res) {
                context.testWorld.operation(op, context.testWorld.getEntity(op->getFrom()));
            }

        };

        /**
         * Move within one domain
         *
         * All entities are placed at origo originally.
         * Hierarchy looks like this:
         * T1 has a physical domain
         *
         *              T1#
         *          T2       T3
         *
         */
        {
            Ref<Thing> t1 = new Thing(context.entityId++);
            t1->requirePropertyClassFixed<BBoxProperty>().data() = {{-512, -10, -512},
                                    {512,  10,  512}};
            t1->setAttrValue("domain", "physical");
            context.testWorld.addEntity(t1, context.world);
            Ref<Thing> t2 = new Thing(context.entityId++);
            t2->requirePropertyClassFixed<PositionProperty>().data() = {0, 0, 0};
            t2->requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
            t2->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            context.testWorld.addEntity(t2, t1);
            Ref<Thing> t3 = new Thing(context.entityId++);
            t3->requirePropertyClassFixed<PositionProperty>().data() = {0, 0, 0};
            t3->requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
            t3->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            context.testWorld.addEntity(t3, t1);

            //Moving t2 directly should succeed.
            OpVector res;
            {
                Move move;
                Anonymous ent;
                ent->setId(t2->getId());
                ent->setPosAsList({10, 0, 10});
                move->setArgs1(ent);
                t2->MoveOperation(move, res);
            }
            ASSERT_EQUAL(t2->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(10, 0, 10))

            //Moving t2 through t1 succeed.
            {
                Move move;
                Anonymous ent;
                ent->setId(t2->getId());
                ent->setPosAsList({20, 0, 20});
                ent->setAttr("orientation", WFMath::Quaternion(1, WFMath::numeric_constants<double>::pi() / 2.0).toAtlas());
                move->setArgs1(ent);
                t1->MoveOperation(move, res);
            }
            ASSERT_EQUAL(t2->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(20, 0, 20))
            ASSERT_EQUAL(t2->requirePropertyClassFixed<OrientationProperty>().data(), WFMath::Quaternion(1, WFMath::numeric_constants<double>::pi() / 2.0))

            //Moving t2 through t3 should fail.
            {
                Move move;
                Anonymous ent;
                ent->setId(t2->getId());
                ent->setPosAsList({30, 0, 30});
                move->setArgs1(ent);
                t3->MoveOperation(move, res);
            }
            ASSERT_EQUAL(t2->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(20, 0, 20))

            //Moving t2 directly to t3 should succeed.
            {
                Move move;
                Anonymous ent;
                ent->setId(t2->getId());
                ent->setLoc(t3->getId());
                move->setArgs1(ent);
                t2->MoveOperation(move, res);
            }
            ASSERT_EQUAL(t2->m_parent, t3.get())

            //Moving t2 directly to t1 should succeed even without pos and orientation.
            {
                Move move;
                Anonymous ent;
                ent->setId(t2->getId());
                ent->setLoc(t1->getId());
                move->setArgs1(ent);
                t2->MoveOperation(move, res);
            }
            ASSERT_EQUAL(t2->m_parent, t1.get())

        }

        /**
          * Move from one domain to another.
          *
          * All entities are placed at origo originally.
          * Hierarchy looks like this:
          * T1 has a physical domain.
          * T2 and T3 have container domains.
          *
          *              T1#
          *          T2*      T3*
          *          T4       T5
          */
        {
            Ref<Thing> t1 = new Thing(context.entityId++);
            t1->requirePropertyClassFixed<BBoxProperty>().data() = {{-512, -10, -512},
                                    {512,  10,  512}};
            t1->setAttrValue("domain", "physical");
            context.testWorld.addEntity(t1, context.world);
            Ref<Thing> t2 = new Thing(context.entityId++);
            t2->requirePropertyClassFixed<PositionProperty>().data() = {0, 0, 0};
            t2->requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
            t2->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            t2->setAttrValue("domain", "container");
            context.testWorld.addEntity(t2, t1);
            Ref<Thing> t3 = new Thing(context.entityId++);
            t3->requirePropertyClassFixed<PositionProperty>().data() = {0, 0, 0};
            t3->requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
            t3->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            t3->setAttrValue("domain", "container");
            context.testWorld.addEntity(t3, t1);

            Ref<Thing> t4 = new Thing(context.entityId++);
            t4->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            context.testWorld.addEntity(t4, t2);

            Ref<Thing> t5 = new Thing(context.entityId++);
            t5->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            context.testWorld.addEntity(t5, t3);


            //Moving t4 directly should succeed.
            OpVector res;
            {
                Move move;
                Anonymous ent;
                ent->setId(t4->getId());
                ent->setLoc(t3->getId());
                move->setArgs1(ent);
                t4->MoveOperation(move, res);
            }
            ASSERT_EQUAL(t4->m_parent, t3.get())

            //Moving t4 through t3 (its parent) should succeed.
            {
                Move move;
                Anonymous ent;
                ent->setId(t4->getId());
                ent->setLoc(t2->getId());
                move->setArgs1(ent);
                t3->MoveOperation(move, res);
            }
            ASSERT_EQUAL(t4->m_parent, t2.get())

            //Moving t4 through t3 (its destination) should succeed.
            {
                Move move;
                Anonymous ent;
                ent->setId(t4->getId());
                ent->setLoc(t3->getId());
                move->setArgs1(ent);
                t3->MoveOperation(move, res);
            }
            ASSERT_EQUAL(t4->m_parent, t3.get())


            //Moving t4 through t1 (root domain) should fail.
            {
                Move move;
                Anonymous ent;
                ent->setId(t4->getId());
                ent->setLoc(t2->getId());
                move->setArgs1(ent);
                t1->MoveOperation(move, res);
            }
            ASSERT_EQUAL(t4->m_parent, t3.get())

        }

        /**
          * Let an entity move other entities.
          *
          * All entities are placed at origo originally.
          * Hierarchy looks like this:
          * T1 has a physical domain
          * T2 has container domain.
          * T6 is an entity with a mind, inventory and reach = 2.0
          *
          *              T1#
          *          T2*      T3     T6**
          *          T4       T5
          */
        {
            Ref<Thing> t1 = new Thing(context.entityId++);
            t1->requirePropertyClassFixed<BBoxProperty>().data() = {{-512, -10, -512},
                                    {512,  10,  512}};
            t1->setAttrValue("domain", "physical");
            context.testWorld.addEntity(t1, context.world);
            Ref<Thing> t2 = new Thing(context.entityId++);
            t2->requirePropertyClassFixed<PositionProperty>().data() = {0, 0, 0};
            t2->requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
            t2->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            t2->setAttrValue("domain", "container");
            context.testWorld.addEntity(t2, t1);
            Ref<Thing> t3 = new Thing(context.entityId++);
            t3->requirePropertyClassFixed<PositionProperty>().data() = {0, 0, 0};
            t3->requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
            t3->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            context.testWorld.addEntity(t3, t1);

            Ref<Thing> t4 = new Thing(context.entityId++);
            t4->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            context.testWorld.addEntity(t4, t2);

            Ref<Thing> t5 = new Thing(context.entityId++);
            t5->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            context.testWorld.addEntity(t5, t3);
            Ref<Thing> t6 = new Thing(context.entityId++);
            t6->requirePropertyClassFixed<PositionProperty>().data() = {0, 0, 0};
            t6->requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
            t6->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            t6->setAttrValue("domain", "inventory");
            t6->setAttrValue("reach", 2.0);
            t6->setAttrValue(MindsProperty::property_name, {});
            context.testWorld.addEntity(t6, t1);

            OpVector res;

            //A Thought about a Move for moving t2 within t1 should work
            {

                Anonymous ent;
                ent->setId(t2->getId());
                ent->setPosAsList({1, 0, 0});
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t2->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(1, 0, 0))

            //A Thought about a Move for moving t2 to t6 should work
            {
                Anonymous ent;
                ent->setId(t2->getId());
                ent->setLoc(t6->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t2->m_parent, t6.get())

            //Add a listener which blocks all operations to t1
            struct : public OperationsListener
            {
                HandlerResult operation(LocatedEntity&, const Operation& op, OpVector& res) override
                {
                    if (op->getClassNo() == Atlas::Objects::Operation::MOVE_NO) {
                        return OPERATION_BLOCKED;
                    }
                    return OPERATION_IGNORED;
                }
            } moveBlockListener;
            t1->addListener(&moveBlockListener);


            //A Thought about a Move for moving t3 to t6 should not work if there's a blocker on t1 (the parent of t3)
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setLoc(t6->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t1.get())

            t1->removeListener(&moveBlockListener);
            t6->addListener(&moveBlockListener);

            //A Thought about a Move for moving t3 to t6 should not work if there's a blocker on t6
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setLoc(t6->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t1.get())

            t6->removeListener(&moveBlockListener);
            t3->addListener(&moveBlockListener);

            //A Thought about a Move for moving t3 to t6 should not work if there's a blocker on t3
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setLoc(t6->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t1.get())

            t3->removeListener(&moveBlockListener);

            //A Thought about a Move for moving t3 to t6 should work if there no blockers
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setLoc(t6->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t6.get())

            //A Thought about a Move for moving t3 to t1 should be blocked if the destination is too far away.
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setLoc(t1->getId());
                ent->setPosAsList({100.0, 0.0, 100.0});
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t6.get())

            //A Thought about a Move for moving t3 to t1 should be allowed if the destination is close.
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setLoc(t1->getId());
                ent->setPosAsList({1.0, 0.0, 1.0});
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t1.get())


            //Add a "mover" constaint to t6.

            t6->setAttrValue("mover_constraint", "false");
            //A Thought about a Move for moving t3 to t6 should not be allowed by the "mover_constraint"
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setLoc(t6->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t1.get())

            t6->setAttrValue("mover_constraint", std::string("entity.id = ") + t3->getId());

            //A Thought about a Move for moving t3 to t6 should be allowed if the "mover_constraint" matches.
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setLoc(t6->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t6.get())

            //clear "mover_constraint"
            t6->setAttrValue("mover_constraint", {});

            t3->setAttrValue("move_constraint", "false");
            //A Thought about a Move for moving t3 to t1 should not be allowed by the "move_constraint"
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setPosAsList({1.0, 0, 1.0});
                ent->setLoc(t1->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t6.get())

            t3->setAttrValue("move_constraint", std::string("entity.id = ") + t3->getId());
            //A Thought about a Move for moving t3 to t1 should be allowed if "move_constraint" matches
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setPosAsList({1.0, 0, 1.0});
                ent->setLoc(t1->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t1.get())

            //clear "move_constraint"
            t3->setAttrValue("move_constraint", {});

            t1->setAttrValue("contain_constraint", "false");
            //A Thought about a Move for moving t3 to t6 should not be allowed by the "contains_constraint" on t1
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setLoc(t6->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t1.get())

            t1->setAttrValue("contain_constraint", "true");
            //A Thought about a Move for moving t3 to t6 should not be allowed by the "contains_constraint" on t1
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setLoc(t6->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t6.get())

            //clear "contain_constraint"
            t1->setAttrValue("contain_constraint", {});

            t1->setAttrValue("destination_constraint", "false");
            //A Thought about a Move for moving t3 to t1 should not be allowed by the "destination_constraint" on t1
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setLoc(t1->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t6.get())

            t1->setAttrValue("destination_constraint", "true");
            //A Thought about a Move for moving t3 to t1 should be allowed is the "destination_constraint" matches
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setPosAsList({1.0, 0, 1.0});
                ent->setLoc(t1->getId());
                thinkMoveFn(t6, ent);
            }
            ASSERT_EQUAL(t3->m_parent, t1.get())
        }

        /**
        * A standard case is not allowing a player entity to move too heavy things.
        *
        * All entities are placed at origo originally.
        * Hierarchy looks like this:
        * T1 has a physical domain
        * T2 is an entity with a mind, inventory, reach = 2.0 and a "mover_constraint" of "mass < 20".
        *
        *              T1#
        *          T2*      T3
        */
        {
            Ref<Thing> t1 = new Thing(context.entityId++);
            t1->requirePropertyClassFixed<BBoxProperty>().data() = {{-512, -10, -512},
                                    {512,  10,  512}};
            t1->setAttrValue("domain", "physical");
            context.testWorld.addEntity(t1, context.world);
            Ref<Thing> t2 = new Thing(context.entityId++);
            t2->requirePropertyClassFixed<PositionProperty>().data() = {0, 0, 0};
            t2->requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
            t2->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            t2->setAttrValue("domain", "inventory");
            t2->setAttrValue("reach", 2.0);
            t2->setAttrValue(MindsProperty::property_name, {});
            t2->setAttrValue("mover_constraint", "entity.mass = none or entity.mass < 20");
            context.testWorld.addEntity(t2, t1);
            Ref<Thing> t3 = new Thing(context.entityId++);
            t3->requirePropertyClassFixed<PositionProperty>().data() = {0, 0, 0};
            t3->requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
            t3->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                    {1,  1, 1}};
            context.testWorld.addEntity(t3, t1);


            OpVector res;

            //A Thought about a Move for moving t3 within t1 should work
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setPosAsList({1, 0, 0});
                thinkMoveFn(t2, ent);
            }
            ASSERT_EQUAL(t3->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(1, 0, 0))

            t3->setAttrValue("mass", 30.0);

            //A Thought about a Move for moving t3 within t1 should fail as t3 is too heavy
            {
                Anonymous ent;
                ent->setId(t3->getId());
                ent->setPosAsList({-1, 0, 0});
                thinkMoveFn(t2, ent);
            }
            ASSERT_EQUAL(t3->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(1, 0, 0))

        }
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
            Ref<Thing> t1 = new Thing(context.entityId++);
            t1->requirePropertyClassFixed<BBoxProperty>().data() = {{-128, -128, -128},
                                    {128,  128,  128}};
            t1->setAttrValue("domain", "physical");
            context.testWorld.addEntity(t1, context.world);
            Ref<Thing> t2 = new Thing(context.entityId++);
            t2->requirePropertyClassFixed<PositionProperty>().data() = {10, 0, 20};
            t2->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            t2->requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion(1, 2.0);
            t2->setAttrValue("domain", "container");
            context.testWorld.addEntity(t2, t1);
            Ref<Thing> t3 = new Thing(context.entityId++);
            t3->requirePropertyClassFixed<PositionProperty>().data() = {20, 0, 20};
            t3->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            context.testWorld.addEntity(t3, t1);
            Ref<Thing> t4 = new Thing(context.entityId++);
            t4->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t4->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            t4->requirePropertyClassFixed<PositionProperty>().data() = {30, 0, 20};
            context.testWorld.addEntity(t4, t2);
            Ref<Thing> t5 = new Thing(context.entityId++);
            t5->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t5->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            t5->requirePropertyClassFixed<PositionProperty>().data() = {40, 0, 20};
            context.testWorld.addEntity(t5, t3);

            OpVector res;
            t2->DeleteOperation({}, res);
            ASSERT_TRUE(t2->isDestroyed())
            ASSERT_EQUAL(t1->getId(), t4->m_parent->getId())
            ASSERT_EQUAL(t4->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(10, 0, 20))
            ASSERT_EQUAL(t4->requirePropertyClassFixed<OrientationProperty>().data(), WFMath::Quaternion(1, 2.0))

            t3->DeleteOperation({}, res);
            ASSERT_TRUE(t3->isDestroyed())
            ASSERT_EQUAL(t1->getId(), t5->m_parent->getId())
            ASSERT_EQUAL(t5->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(20, 0, 20))

        }
    }

    void test_setProps(TestContext& context)
    {
        Ref<Thing> entity(new Thing(context.entityId++));

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

    void test_setModifySelf(TestContext& context)
    {
        Ref<Thing> entity(new Thing(context.entityId++));
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

    void test_setModify(TestContext& context)
    {
        Ref<Thing> entity(new Thing(context.entityId++));
        Ref<Thing> entityChild(new Thing(context.entityId++));
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
        Ref<Thing> entity(new Thing(context.entityId++));
        context.testWorld.addEntity(entity, context.world);
        sendSetOp(entity, "attachments", MapType{{"hand_primary", "contains(actor.contains, child = tool)"}});

        Ref<Thing> entityChild(new Thing(context.entityId++));
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
