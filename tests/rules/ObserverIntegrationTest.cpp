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

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <rules/simulation/CorePropertyManager.h>

#include <memory>
#include <utility>
#include <rules/simulation/ModifiersProperty.h>
#include <rules/simulation/PhysicalDomain.h>
#include <rules/simulation/VoidDomain.h>
#include <rules/simulation/WorldRouter.h>

using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

struct TestWorldRouter : public WorldRouter
{
    TestWorldExtension m_extension;

    explicit TestWorldRouter(Ref<LocatedEntity> gw, EntityCreator& entityCreator) : WorldRouter(std::move(gw), entityCreator)
    {
    }

    ~TestWorldRouter() override = default;

    Ref<LocatedEntity> addNewEntity(const std::string& id,
                                    const Atlas::Objects::Entity::RootEntity& op) override
    {
        if (m_extension.addNewEntityFn) {
            return m_extension.addNewEntityFn(id, op);
        } else {
            return WorldRouter::addNewEntity(id, op);
        }
    }


    void message(Operation op, LocatedEntity& ent) override
    {
        if (m_extension.messageFn) {
            m_extension.messageFn(op, ent);
        } else {
            WorldRouter::message(op, ent);
        }
    }


};

struct NullEntityCreator : public EntityCreator
{
    Ref<LocatedEntity> newEntity(const std::string& id,
                                 long intId,
                                 const std::string& type,
                                 const Atlas::Objects::Entity::RootEntity& attrs) const override
    {
        return {};
    }
};

struct TestContext
{
    Atlas::Objects::Factories factories;
    DatabaseNull database;
    NullEntityCreator entityCreator;
    Ref<World> world;
    Inheritance inheritance;
    TestWorldRouter testWorld;
    CorePropertyManager propertyManager;

    TestContext() :
            world(new World()),
            inheritance(factories),
            testWorld(world, entityCreator),
            propertyManager(inheritance)
    {
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

        //Clear ops queue
        opsHandler.clearQueues();

        //Make an observer, which we'll add to the physical domain
        Ref<Thing> observer(new Thing(counter++));
        observer->setAttrValue("mode", "fixed");
        observer->setAttrValue("perception_sight", 1);
        observer->m_location.m_pos = {0, 0, 0};
        context.testWorld.addEntity(observer, domainPhysical);

        auto ops = collectQueue(queue);
        //We now expect to get an Appearence op sent to the observer about the domain entity, as well as an Appearance sent to the observer itself.
        ASSERT_EQUAL(2, ops.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, ops[0]->getClassNo())
        ASSERT_EQUAL(domainPhysical->getId(), ops[0]->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), ops[0]->getTo())

        ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, ops[1]->getClassNo())
        ASSERT_EQUAL(observer->getId(), ops[1]->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), ops[1]->getTo())

        opsHandler.clearQueues();

        //Make another observer, which we'll add to the void domain
        Ref<Thing> observer_void(new Thing(counter++));
        observer->setAttrValue("mode", "fixed");
        observer->setAttrValue("perception_sight", 1);
        observer->m_location.m_pos = {0, 0, 0};
        context.testWorld.addEntity(observer_void, domainVoid);

        //Clear ops queue
        opsHandler.clearQueues();

        //Create something we can look at
        Ref<Thing> object1(new Thing(counter++));
        object1->m_location.setBBox({{-1, -1, -1},
                                     {1,  1,  1}});
        object1->setAttrValue("mode", "fixed");
        object1->m_location.m_pos = {10, 0, 10};
        context.testWorld.addEntity(object1, domainPhysical);

        ops = collectQueue(queue);

        //We now expect to get an Appearance op sent to the observer (but not the observer in the void)
        ASSERT_EQUAL(1, ops.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, ops.front()->getClassNo())
        ASSERT_EQUAL(object1->getId(), ops.front()->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), ops.front()->getTo())

        opsHandler.clearQueues();

        OpVector res;
        //Move object1 to the void domain
        {
            Atlas::Objects::Operation::Move move;

            Anonymous arg1;
            arg1->setId(object1->getId());
            arg1->setLoc(domainVoid->getId());
            move->setArgs1(arg1);
            object1->MoveOperation(move, res);
        }
        ops = collectQueue(queue);

        //We now expect to get a Disappearance op sent to the observer (but nothing sent to the observer in the void). The first op should be a Sight op, the second an Update op.
        ASSERT_EQUAL(0, ops.size())
        ASSERT_EQUAL(3, res.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::SIGHT_NO, res[0]->getClassNo())
        ASSERT_EQUAL(Atlas::Objects::Operation::UPDATE_NO, res[1]->getClassNo())
        ASSERT_EQUAL(Atlas::Objects::Operation::DISAPPEARANCE_NO, res[2]->getClassNo())
        ASSERT_EQUAL(observer->getId(), res[0]->getTo())
        ASSERT_EQUAL(object1->getId(), res[2]->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), res[2]->getTo())

        res.clear();

        //Move object1 back to the physical domain
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

        //We now expect to get an Appearance op sent to the observer (but nothing sent to the observer in the void). The first op should be an Update op
        ASSERT_EQUAL(0, ops.size())
        ASSERT_EQUAL(2, res.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::UPDATE_NO, res[0]->getClassNo())
        ASSERT_EQUAL(Atlas::Objects::Operation::APPEARANCE_NO, res[1]->getClassNo())
        ASSERT_EQUAL(object1->getId(), res[1]->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), res[1]->getTo())

        res.clear();

        //Delete object1
        {
            Atlas::Objects::Operation::Delete deleteOp;
            object1->DeleteOperation(deleteOp, res);
        }
        ops = collectQueue(queue);

        //A Sight is is sent to the deleted entity, but it's sent directly without being put on the queue.
        ASSERT_EQUAL(0, ops.size())
        ASSERT_EQUAL(2, res.size())
        ASSERT_EQUAL(Atlas::Objects::Operation::SIGHT_NO, res[0]->getClassNo())
        ASSERT_EQUAL(Atlas::Objects::Operation::DISAPPEARANCE_NO, res[1]->getClassNo())
        ASSERT_EQUAL(observer->getId(), res[0]->getTo())
        ASSERT_EQUAL(object1->getId(), res[1]->getArgs().front()->getId())
        ASSERT_EQUAL(observer->getId(), res[1]->getTo())
    }

};


int main()
{
    Monitors m;
    Tested t;

    return t.run();
}
