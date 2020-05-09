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
#include "../stubs/common/stubcustom.h"
#include "../NullEntityCreator.h"
#include "../TestWorldRouter.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <rules/simulation/CorePropertyManager.h>

#include <memory>
#include <utility>
#include <rules/simulation/PhysicalDomain.h>
#include <rules/simulation/VoidDomain.h>
#include <rules/simulation/WorldRouter.h>
#include <common/operations/Tick.h>
#include <wfmath/atlasconv.h>
#include <rules/simulation/AdminProperty.h>
#include <rules/simulation/ContainerAccessProperty.h>

using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

struct TestContext
{
    Atlas::Objects::Factories factories;
    DatabaseNull database;
    NullEntityCreator entityCreator;
    Ref<World> world;
    Inheritance inheritance;
    TestWorldRouter testWorld;
    CorePropertyManager propertyManager;

    TestContext()
            : world(new World()), inheritance(factories), testWorld(world, entityCreator), propertyManager(inheritance)
    {
    }

    ~TestContext()
    {
        world->destroy();
    }
};

namespace {
    std::vector<OpQueEntry<LocatedEntity>> collectQueue(std::priority_queue<OpQueEntry<LocatedEntity>, std::vector<OpQueEntry<LocatedEntity>>, std::greater<OpQueEntry<LocatedEntity>>>& queue)
    {
        std::vector<OpQueEntry<LocatedEntity>> list;
        list.reserve(queue.size());
        while (!queue.empty()) {
            list.emplace_back(queue.top());
            queue.pop();
        }
        return list;
    }
}

OpVector resIgnored;

struct Tested : public Cyphesis::TestBaseWithContext<TestContext>
{
    Tested()
    {
        ADD_TEST(test_handleContainers);
        ADD_TEST(test_sendAppearDisappearWithPrivateAndProtected);
        ADD_TEST(test_sendAppearDisappear);
    }

    static void sendSetOp(const Ref<Thing>& entity, const std::string& propertyName, Atlas::Message::Element value)
    {
        Set set;

        Anonymous arg1;
        arg1->setAttr(propertyName, std::move(value));
        set->setArgs1(arg1);

        entity->operation(set, resIgnored);
    }

    void test_handleContainers(TestContext& context)
    {

        auto moveFn = [](const Ref<Thing>& thing, WFMath::Point<3> pos) -> OpVector {
            OpVector res;
            Atlas::Objects::Operation::Move move;

            Anonymous arg1;
            arg1->setId(thing->getId());
            Atlas::Message::Element posElement = pos.toAtlas();
            arg1->setPosAsList(posElement.List());
            move->setArgs1(arg1);
            thing->MoveOperation(move, res);
            return res;
        };

        /**
         * Check that entities only are allowed to look into containers if they are allowed by the __container_access
         *
         * All entities are placed at origo
         * Hierarchy looks like this:
         * T1 has a physical domain
         * T2 and T5 has a container domain
         *
         *              T1#
         *         T2*       T3
         *      T4   T5*
         *           T6
         */
        {
            auto& opsHandler = context.testWorld.getOperationsHandler();
            auto& queue = opsHandler.getQueue();

            WFMath::AxisBox<3> bbox(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));
            Ref<Thing> t1 = new Thing(1);
            t1->setAttrValue("domain", "physical");
            t1->m_location.m_pos = WFMath::Point<3>::ZERO();
            context.testWorld.addEntity(t1, context.world);
            Ref<Thing> t2 = new Thing(2);
            t2->m_location.m_pos = WFMath::Point<3>::ZERO();
            t2->m_location.setBBox(bbox);
            t2->setAttrValue("domain", "container");
            context.testWorld.addEntity(t2, t1);
            Ref<Thing> t3 = new Thing(3);
            t3->m_location.m_pos = WFMath::Point<3>::ZERO();
            t3->m_location.setBBox(bbox);
            t3->setAttrValue("perception_sight", 1);
            context.testWorld.addEntity(t3, t1);
            Ref<Thing> t4 = new Thing(4);
            t4->m_location.m_pos = WFMath::Point<3>::ZERO();
            t4->m_location.setBBox(bbox);
            context.testWorld.addEntity(t4, t2);
            Ref<Thing> t5 = new Thing(5);
            t5->m_location.m_pos = WFMath::Point<3>::ZERO();
            t5->m_location.setBBox(bbox);
            t5->setAttrValue("domain", "container");
            context.testWorld.addEntity(t5, t2);
            Ref<Thing> t6 = new Thing(7);
            t6->m_location.m_pos = WFMath::Point<3>::ZERO();
            t6->m_location.setBBox(bbox);
            context.testWorld.addEntity(t6, t5);

            opsHandler.clearQueues();

            ASSERT_TRUE(t2->isVisibleForOtherEntity(t3.get()))
            ASSERT_TRUE(t3->canReach(EntityLocation{t2}))
            ASSERT_FALSE(t3->canReach(EntityLocation{t4}))
            ASSERT_FALSE(t3->canReach(EntityLocation{t5}))
            ASSERT_FALSE(t3->canReach(EntityLocation{t6}))
            //Add t3 as container observer to t2, which allows it to view its content (t4 and t5)
            t2->setAttrValue(ContainerAccessProperty::property_name, Atlas::Message::ListType{t3->getId()});
            ASSERT_TRUE(t3->canReach(EntityLocation{t2}))
            ASSERT_TRUE(t3->canReach(EntityLocation{t4}))
            ASSERT_TRUE(t3->canReach(EntityLocation{t5}))
            ASSERT_FALSE(t3->canReach(EntityLocation{t6}))
            ASSERT_EQUAL(1, queue.size())
            ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, queue.top()->getClassNo())
            ASSERT_EQUAL(t3->getId(), queue.top()->getTo())
            ASSERT_EQUAL(2, queue.top()->getArgs().size())

            //Add t3 as container observer to t5, which allows it to view its content (t6)
            opsHandler.clearQueues();
            t5->setAttrValue(ContainerAccessProperty::property_name, Atlas::Message::ListType{t3->getId()});
            ASSERT_TRUE(t3->canReach(EntityLocation{t6}))
            ASSERT_EQUAL(1, queue.size())
            ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, queue.top()->getClassNo())
            ASSERT_EQUAL(t3->getId(), queue.top()->getTo())
            ASSERT_EQUAL(1, queue.top()->getArgs().size())
            //Remove t3 as observer...
            opsHandler.clearQueues();
            t5->setAttrValue(ContainerAccessProperty::property_name, Atlas::Message::ListType{});
            ASSERT_FALSE(t3->canReach(EntityLocation{t6}))
            ASSERT_EQUAL(1, queue.size())
            ASSERT_EQUAL(Atlas::Objects::Operation::DISAPPEARANCE_NO, queue.top()->getClassNo())
            ASSERT_EQUAL(t3->getId(), queue.top()->getTo())
            ASSERT_EQUAL(1, queue.top()->getArgs().size())
            //...and add it back.
            t5->setAttrValue(ContainerAccessProperty::property_name, Atlas::Message::ListType{t3->getId()});
            ASSERT_TRUE(t3->canReach(EntityLocation{t6}))
            //Remove t3 as observer from t2, which should sever the connection to t5
            opsHandler.clearQueues();
            t2->setAttrValue(ContainerAccessProperty::property_name, Atlas::Message::ListType{});
            //Should still be able to reach t2, even if we can't reach into it.
            ASSERT_TRUE(t3->canReach(EntityLocation{t2}))
            ASSERT_TRUE(t2->isVisibleForOtherEntity(t3.get()));
            ASSERT_FALSE(t3->canReach(EntityLocation{t5}))
            ASSERT_FALSE(t5->isVisibleForOtherEntity(t3.get()));
            ASSERT_FALSE(t3->canReach(EntityLocation{t6}))
            ASSERT_FALSE(t6->isVisibleForOtherEntity(t3.get()));
            ASSERT_EQUAL(1, queue.size()) //Should really be 2, one from t2 and one from t5
            ASSERT_EQUAL(Atlas::Objects::Operation::DISAPPEARANCE_NO, queue.top()->getClassNo())
            ASSERT_EQUAL(t3->getId(), queue.top()->getTo())
            ASSERT_EQUAL(2, queue.top()->getArgs().size()) //From both t4 and t5

            //Add it back as observer
            t2->setAttrValue(ContainerAccessProperty::property_name, Atlas::Message::ListType{t3->getId()});
            ASSERT_TRUE(t3->canReach(EntityLocation{t5}))

            opsHandler.clearQueues();
            //Now move t3 away far enough that it can't reach t2 anymore.
            moveFn(t3, {510, 0, 500});
            ASSERT_FALSE(t3->canReach(EntityLocation{t2}))
            ASSERT_FALSE(t3->canReach(EntityLocation{t5}))

            ASSERT_EQUAL(2, queue.size()) //Should really be 3, one Sight, and one Appearance from t2 and one Appearance from t5
            queue.pop();
            ASSERT_EQUAL(Atlas::Objects::Operation::DISAPPEARANCE_NO, queue.top()->getClassNo())
            ASSERT_EQUAL(t3->getId(), queue.top()->getTo())
            ASSERT_EQUAL(2, queue.top()->getArgs().size()) //From both t4 and t5

            //Move t3 closer again
            moveFn(t3, {0, 0, 0});
            ASSERT_TRUE(t3->canReach(EntityLocation{t2}))
            //t5 should not be reachable since we severed the "reach" connection when moving away
            ASSERT_FALSE(t3->canReach(EntityLocation{t5}))
            t2->setAttrValue(ContainerAccessProperty::property_name, Atlas::Message::ListType{t3->getId()});
            ASSERT_TRUE(t3->canReach(EntityLocation{t5}))

            //And then once again move it away
            moveFn(t3, {510, 0, 500});

            ASSERT_FALSE(t3->canReach(EntityLocation{t2}))
            ASSERT_FALSE(t3->canReach(EntityLocation{t5}))


            //            //When we add t3 back as an observer to t2 the link to t6 should have been severed, and it should no longer be reachable
//            t2->setAttrValue(ContainerAccessProperty::property_name, Atlas::Message::ListType{t3->getId()});
//            ASSERT_TRUE(t3->canReach(EntityLocation{t5}))
//            ASSERT_FALSE(t3->canReach(EntityLocation{t6}))


        }
    }


    void test_sendAppearDisappearWithPrivateAndProtected(TestContext& context)
    {
        auto& opsHandler = context.testWorld.getOperationsHandler();
        auto& queue = opsHandler.getQueue();

        long counter = 1;
        Ref<Thing> domainPhysical(new Thing(counter++));
        context.testWorld.addEntity(domainPhysical, context.world);
        domainPhysical->m_location.setBBox({{-512, -512, -512},
                                            {512,  512,  512}});
        domainPhysical->setDomain(std::make_unique<PhysicalDomain>(*domainPhysical));

        // Clear ops queue
        opsHandler.clearQueues();

        // Make an observer, which we'll add to the physical domain
        Ref<Thing> observer(new Thing(counter++));
        observer->setAttrValue("mode", "fixed");
        observer->setAttrValue("perception_sight", 1);
        observer->m_location.m_pos = {0, 0, 0};
        context.testWorld.addEntity(observer, domainPhysical);

        // Make an admin observer, which we'll add to the physical domain
        Ref<Thing> observerAdmin(new Thing(counter++));
        observerAdmin->setAttrValue("mode", "fixed");
        observerAdmin->setAttrValue("perception_sight", 1);
        observerAdmin->setAttrValue(AdminProperty::property_name, 1);
        observerAdmin->m_location.m_pos = {0, 0, 0};
        context.testWorld.addEntity(observerAdmin, domainPhysical);

        auto ops = collectQueue(queue);
        opsHandler.clearQueues();
        OpVector res;

        // Create a private entity, which should only be seen by observerAdmin
        Ref<Thing> objectPrivate1(new Thing(counter++));
        objectPrivate1->m_location.setBBox({{-1, -1, -1},
                                            {1,  1,  1}});
        objectPrivate1->setAttrValue("mode", "fixed");
        objectPrivate1->setAttrValue("visibility", "private");
        objectPrivate1->m_location.m_pos = {0, 0, 0};
        context.testWorld.addEntity(objectPrivate1, domainPhysical);

        ops = collectQueue(queue);

        // We now expect to get an Appearance op sent to the admin observer (but not the regular observer)
        ASSERT_EQUAL(1, ops.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, ops.front()->getClassNo())
        ASSERT_EQUAL(objectPrivate1->getId(), ops.front()->getArgs().front()->getId())
        ASSERT_EQUAL(observerAdmin->getId(), ops.front()->getTo())

        opsHandler.clearQueues();

        // Move objectPrivate1 a bit; only admin observer should see it
        {
            Atlas::Objects::Operation::Move move;

            Anonymous arg1;
            arg1->setId(objectPrivate1->getId());
            arg1->setPos({510, 0, 500});
            move->setArgs1(arg1);
            objectPrivate1->MoveOperation(move, res);
        }
        ASSERT_EQUAL(1, res.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::SIGHT_NO, res.front()->getClassNo())
        ASSERT_EQUAL(Atlas::Objects::Operation::MOVE_NO, res.front()->getArgs().front()->getClassNo())
        ASSERT_EQUAL(objectPrivate1->getId(), Atlas::Objects::smart_dynamic_cast<Operation>(res.front()->getArgs().front())->getArgs().front()->getId())
        ASSERT_EQUAL(observerAdmin->getId(), res.front()->getTo())
    }

    void test_sendAppearDisappear(TestContext& context)
    {
        auto& opsHandler = context.testWorld.getOperationsHandler();
        auto& queue = opsHandler.getQueue();

        long counter = 1;
        Ref<Thing> domainPhysical(new Thing(counter++));
        context.testWorld.addEntity(domainPhysical, context.world);
        domainPhysical->m_location.setBBox({{-512, -512, -512},
                                            {512,  512,  512}});
        domainPhysical->setDomain(std::make_unique<PhysicalDomain>(*domainPhysical));

        Ref<Thing> domainVoid(new Thing(counter++));
        context.testWorld.addEntity(domainVoid, context.world);
        domainVoid->setDomain(std::make_unique<VoidDomain>(*domainVoid));

        auto domainTickFn = [&]() -> OpVector {
            // We must send a Tick op to make the domain handle appear and disappear
            OpVector res;
            Atlas::Objects::Operation::Tick tick;
            Atlas::Objects::Entity::Anonymous arg1;
            arg1->setName("domain");
            tick->setArgs1(arg1);
            tick->setAttr("lastTick", 0.0f);
            tick->setSeconds(2.0f);  // This should trigger a visibility.
            domainPhysical->getDomain()->operation(domainPhysical.get(), tick, res);
            return res;
        };

        // Clear ops queue
        opsHandler.clearQueues();

        // Make an observer, which we'll add to the physical domain
        Ref<Thing> observer(new Thing(counter++));
        observer->setAttrValue("mode", "fixed");
        observer->setAttrValue("perception_sight", 1);
        observer->m_location.m_pos = {0, 0, 0};
        context.testWorld.addEntity(observer, domainPhysical);

        auto ops = collectQueue(queue);
        // We now expect to get an Appearence op sent to the observer about the domain entity, as well as an Appearance sent to the observer itself.
        ASSERT_EQUAL(2, ops.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, ops[0]->getClassNo())
        ASSERT_EQUAL(domainPhysical->getId(), ops[0]->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), ops[0]->getTo())

        ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, ops[1]->getClassNo())
        ASSERT_EQUAL(observer->getId(), ops[1]->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), ops[1]->getTo())

        opsHandler.clearQueues();

        // Make another observer, which we'll add to the void domain
        Ref<Thing> observer_void(new Thing(counter++));
        observer->setAttrValue("mode", "fixed");
        observer->setAttrValue("perception_sight", 1);
        observer->m_location.m_pos = {0, 0, 0};
        context.testWorld.addEntity(observer_void, domainVoid);

        // Clear ops queue
        opsHandler.clearQueues();

        // Create something we can look at
        Ref<Thing> object1(new Thing(counter++));
        object1->m_location.setBBox({{-1, -1, -1},
                                     {1,  1,  1}});
        object1->setAttrValue("mode", "fixed");
        object1->m_location.m_pos = {10, 0, 10};
        context.testWorld.addEntity(object1, domainPhysical);

        ops = collectQueue(queue);

        // We now expect to get an Appearance op sent to the observer (but not the observer in the void)
        ASSERT_EQUAL(1, ops.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, ops.front()->getClassNo())
        ASSERT_EQUAL(object1->getId(), ops.front()->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), ops.front()->getTo())

        opsHandler.clearQueues();

        OpVector res;
        // Move object1 to the void domain
        {
            Atlas::Objects::Operation::Move move;

            Anonymous arg1;
            arg1->setId(object1->getId());
            arg1->setLoc(domainVoid->getId());
            move->setArgs1(arg1);
            object1->MoveOperation(move, res);
        }
        ops = collectQueue(queue);

        // We now expect to get a Disappearance op sent to the observer (but nothing sent to the observer in the void). The first op should be a Sight op, the second an Update op.
        ASSERT_EQUAL(0, ops.size())
        ASSERT_EQUAL(3, res.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::SIGHT_NO, res[0]->getClassNo())
        ASSERT_EQUAL(Atlas::Objects::Operation::UPDATE_NO, res[1]->getClassNo())
        ASSERT_EQUAL(Atlas::Objects::Operation::DISAPPEARANCE_NO, res[2]->getClassNo())
        ASSERT_EQUAL(observer->getId(), res[0]->getTo())
        ASSERT_EQUAL(object1->getId(), res[2]->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), res[2]->getTo())

        res.clear();

        // Move object1 back to the physical domain
        {
            Atlas::Objects::Operation::Move move;

            Anonymous arg1;
            arg1->setId(object1->getId());
            arg1->setLoc(domainPhysical->getId());
            arg1->setPosAsList({0, 0, 0});
            move->setArgs1(arg1);
            object1->MoveOperation(move, res);
        }
        ops = collectQueue(queue);

        // We now expect to get an Appearance op sent to the observer (but nothing sent to the observer in the void). The first op should be an Update op
        ASSERT_EQUAL(0, ops.size())
        ASSERT_EQUAL(2, res.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::UPDATE_NO, res[0]->getClassNo())
        ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, res[1]->getClassNo())
        ASSERT_EQUAL(object1->getId(), res[1]->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), res[1]->getTo())

        res.clear();

        // Delete object1
        {
            Atlas::Objects::Operation::Delete deleteOp;
            object1->DeleteOperation(deleteOp, res);
        }
        ops = collectQueue(queue);

        // A Sight is sent to the deleted entity, but it's sent directly without being put on the queue.
        ASSERT_EQUAL(0, ops.size())
        ASSERT_EQUAL(2, res.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::SIGHT_NO, res[0]->getClassNo())
        ASSERT_EQUAL(Atlas::Objects::Operation::DISAPPEARANCE_NO, res[1]->getClassNo())
        ASSERT_EQUAL(observer->getId(), res[0]->getTo())
        ASSERT_EQUAL(object1->getId(), res[1]->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), res[1]->getTo())

        // Create a new object, which we'll then move away beyond visible distance
        Ref<Thing> object2(new Thing(counter++));
        object2->m_location.setBBox({{-0.1, -0.1, -0.1},
                                     {0.1,  0.1,  0.1}});
        object2->setAttrValue("mode", "fixed");
        object2->m_location.m_pos = {10, 0, 10};
        context.testWorld.addEntity(object2, domainPhysical);

        ops = collectQueue(queue);

        // We now expect to get an Appearance op sent to the observer (but not the observer in the void)
        ASSERT_EQUAL(1, ops.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, ops.front()->getClassNo())
        ASSERT_EQUAL(object2->getId(), ops.front()->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), ops.front()->getTo())

        opsHandler.clearQueues();
        res.clear();
        domainTickFn();
        // Move object2 away a great distance, outside of visible range
        {
            Atlas::Objects::Operation::Move move;

            Anonymous arg1;
            arg1->setId(object2->getId());
            arg1->setPos({500, 0, 500});
            move->setArgs1(arg1);
            object2->MoveOperation(move, res);
        }
        ops = collectQueue(queue);
        // We now expect to get a Disappearance op sent to the observer. The first op should be a Sight op, the second an Update op.
        // The ops contains a Sight of a Set, which contains the new positional attributes as the entity moved within the PhysicalDomain.
        ASSERT_EQUAL(1, ops.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::SIGHT_NO, ops[0]->getClassNo())
        ASSERT_EQUAL(Atlas::Objects::Operation::SET_NO, ops[0]->getArgs().front()->getClassNo())
        ASSERT_EQUAL(observer->getId(), ops[0]->getTo())
        ASSERT_EQUAL(1, res.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::SIGHT_NO, res[0]->getClassNo())
        ASSERT_EQUAL(observer->getId(), res[0]->getTo())

        // We must send a Tick op to make the domain handle appear and disappear
        res = domainTickFn();
        ASSERT_EQUAL(2, res.size())  // Second op is a Tick
        ASSERT_EQUAL(Atlas::Objects::Operation::DISAPPEARANCE_NO, res[0]->getClassNo())
        ASSERT_EQUAL(observer->getId(), res[0]->getTo())
        ASSERT_EQUAL(object2->getId(), res[0]->getArgs().front()->getId())

        opsHandler.clearQueues();
        res.clear();
        // Move object2 a bit; nothing should see it
        {
            Atlas::Objects::Operation::Move move;

            Anonymous arg1;
            arg1->setId(object2->getId());
            arg1->setPos({510, 0, 500});
            move->setArgs1(arg1);
            object2->MoveOperation(move, res);
        }
        ops = collectQueue(queue);
        ASSERT_TRUE(res.empty())
        ASSERT_TRUE(ops.empty())
        res = domainTickFn();
        ASSERT_EQUAL(1, res.size())  // Should only contain Tick

        opsHandler.clearQueues();
        // Make object2 very much larger, so it should appear
        {
            object2->setAttrValue("bbox", WFMath::AxisBox<3>{{-500, -500, -500},
                                                             {500,  500,  500}}.toAtlas());
        }
        ops = collectQueue(queue);
        ASSERT_TRUE(ops.empty())
        res = domainTickFn();
        ASSERT_EQUAL(2, res.size())  // Second op is a Tick

        ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, res[0]->getClassNo())
        ASSERT_EQUAL(observer->getId(), res[0]->getTo())
        ASSERT_EQUAL(object2->getId(), res[0]->getArgs().front()->getId())

        opsHandler.clearQueues();
        // Make object2 smaller again, so it should disappear
        {
            object2->setAttrValue("bbox", WFMath::AxisBox<3>{{-1, -1, -1},
                                                             {1,  1,  1}}.toAtlas());
        }
        res = domainTickFn();
        ASSERT_EQUAL(2, res.size())  // Second op is a Tick

        ASSERT_EQUAL(Atlas::Objects::Operation::DISAPPEARANCE_NO, res[0]->getClassNo())
        ASSERT_EQUAL(observer->getId(), res[0]->getTo())
        ASSERT_EQUAL(object2->getId(), res[0]->getArgs().front()->getId())


        opsHandler.clearQueues();
        res.clear();
        // Move object2 close to the observer so it appears
        {
            Atlas::Objects::Operation::Move move;

            Anonymous arg1;
            arg1->setId(object2->getId());
            arg1->setPos({0, 0, 0});
            move->setArgs1(arg1);
            object2->MoveOperation(move, res);
        }
        ops = collectQueue(queue);
        ASSERT_TRUE(res.empty())
        ASSERT_TRUE(ops.empty())
        res = domainTickFn();
        ASSERT_EQUAL(2, res.size())  // Second op is a Tick

        ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, res[0]->getClassNo())
        ASSERT_EQUAL(observer->getId(), res[0]->getTo())
        ASSERT_EQUAL(object2->getId(), res[0]->getArgs().front()->getId())
    }
};

int main()
{
    Monitors m;
    Tested t;

    return t.run();
}
