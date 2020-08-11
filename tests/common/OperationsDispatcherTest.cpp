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

#include "common/OperationsDispatcher_impl.h"
#include "common/Monitors.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

#include <memory>
#include <wfmath/atlasconv.h>
#include <modules/ReferenceCounted.h>
#include <common/operations/Update.h>

using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

struct TestContext
{
};

struct TestEntity : ReferenceCounted
{
    std::string describeEntity() const
    {
        return "";
    }

    std::string getId() const
    {
        return "1";
    }
};


struct Tested : public Cyphesis::TestBaseWithContext<TestContext>
{
    Tested()
    {
        ADD_TEST(test_dispatchInOrder)

    }

    void test_dispatchInOrder(TestContext& context)
    {

        std::chrono::milliseconds time(0);
        auto processorFn = [](const Operation&, Ref<TestEntity>) {};
        auto timeProviderFn = [&time]() -> std::chrono::steady_clock::duration { return time; };

        OperationsDispatcher<TestEntity> dispatcher(processorFn, timeProviderFn);
        auto& queue = dispatcher.getQueue();

        Ref<TestEntity> entity(new TestEntity);

        {
            Operation op;
            op->setSeconds(1.0);
            op->setRefno(1);
            dispatcher.addOperationToQueue(op, entity);
        }
        {
            Operation op;
            op->setSeconds(2.0);
            op->setRefno(2);
            dispatcher.addOperationToQueue(op, entity);
        }
        {
            Operation op;
            op->setSeconds(3.0);
            op->setRefno(3);
            dispatcher.addOperationToQueue(op, entity);
        }

        ASSERT_EQUAL(queue.top().op->getRefno(), 1)
        queue.pop();
        ASSERT_EQUAL(queue.top().op->getRefno(), 2)
        queue.pop();
        ASSERT_EQUAL(queue.top().op->getRefno(), 3)
        queue.pop();

        //Now try with same seconds set.
        {
            Operation op;
            op->setSeconds(1.0);
            op->setRefno(1);
            dispatcher.addOperationToQueue(op, entity);
        }
        {
            Operation op;
            op->setSeconds(1.0);
            op->setRefno(2);
            dispatcher.addOperationToQueue(op, entity);
        }
        {
            Operation op;
            op->setSeconds(1.0);
            op->setRefno(3);
            dispatcher.addOperationToQueue(op, entity);
        }

        ASSERT_EQUAL(queue.top().op->getRefno(), 1)
        queue.pop();
        ASSERT_EQUAL(queue.top().op->getRefno(), 2)
        queue.pop();
        ASSERT_EQUAL(queue.top().op->getRefno(), 3)
        queue.pop();


        //Now try with same seconds set.
        {
            Atlas::Objects::Operation::Update op;
            op->setSeconds(1.0);
            dispatcher.addOperationToQueue(op, entity);
        }
        {
            Atlas::Objects::Operation::Appearance op;
            op->setSeconds(1.0);
            dispatcher.addOperationToQueue(op, entity);
        }
        {
            Atlas::Objects::Operation::Update op;
            op->setSeconds(1.0);
            dispatcher.addOperationToQueue(op, entity);
        }

        ASSERT_EQUAL(queue.top().op->getClassNo(), Atlas::Objects::Operation::UPDATE_NO)
        queue.pop();
        ASSERT_EQUAL(queue.top().op->getClassNo(), Atlas::Objects::Operation::APPEARANCE_NO)
        queue.pop();
        ASSERT_EQUAL(queue.top().op->getClassNo(), Atlas::Objects::Operation::UPDATE_NO)
        queue.pop();

        {
            Operation op;
            op->setSeconds(2.0);
            op->setRefno(2);
            dispatcher.addOperationToQueue(op, entity);
        }
        {
            Operation op;
            op->setSeconds(3.0);
            op->setRefno(3);
            dispatcher.addOperationToQueue(op, entity);
        }
        {
            Operation op;
            op->setSeconds(1.0);
            op->setRefno(1);
            dispatcher.addOperationToQueue(op, entity);
        }
        {
            Operation op;
            op->setRefno(4);
            op->setSeconds(0);
            dispatcher.addOperationToQueue(op, entity);
        }
        {
            Operation op;
            op->setRefno(5);
            op->setSeconds(0);
            dispatcher.addOperationToQueue(op, entity);
        }

        ASSERT_EQUAL(queue.top().op->getRefno(), 4)
        queue.pop();
        ASSERT_EQUAL(queue.top().op->getRefno(), 5)
        queue.pop();
        ASSERT_EQUAL(queue.top().op->getRefno(), 1)
        queue.pop();
        ASSERT_EQUAL(queue.top().op->getRefno(), 2)
        queue.pop();
        ASSERT_EQUAL(queue.top().op->getRefno(), 3)
        queue.pop();

    }

};

int main()
{
    Tested t;
    Monitors m;

    return t.run();
}

