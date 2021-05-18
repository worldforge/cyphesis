// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "common/AtlasStreamClient.h"
#include "common/ClientTask.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>
Atlas::Objects::Factories atlasFactories;
class TestAtlasStreamClient : public AtlasStreamClient {
  public:

    explicit TestAtlasStreamClient(boost::asio::io_context& io_context):
        AtlasStreamClient(io_context, atlasFactories) {}

    void test_objectArrived(const Atlas::Objects::Root & op) {
        objectArrived(op);
    }

    void test_operation(const Operation & op) {
        operation(op);
    }

    void test_output(const Atlas::Message::Element & item, int depth) {
        output(item, depth);
    }

    std::shared_ptr<ClientTask> test_currentTask() { return m_currentTask; }
};

class TestClientTask : public ClientTask {
  public:
    virtual void setup(const std::string & arg, OpVector & res) {
        res.push_back(Operation());
    }
    /// \brief Handle an operation from the server
    virtual void operation(const Operation &, OpVector & res) {
        res.push_back(Operation());
    }

    void make_complete() { m_complete = true; }
};

int main()
{
    boost::asio::io_context io_context;
    {
        AtlasStreamClient * asc = new AtlasStreamClient{io_context, atlasFactories};

        delete asc;
    }

    TestAtlasStreamClient asc{io_context};

    {
        Atlas::Objects::Root obj;
        asc.test_objectArrived(obj);
        obj->setParent("");
        asc.test_objectArrived(obj);
        obj->setParent("foo");
        asc.test_objectArrived(obj);
        obj->setObjtype("foo");
        asc.test_objectArrived(obj);
    }

    Operation op;
    asc.test_objectArrived(op);
    asc.test_operation(op);

    auto tct = std::make_shared<TestClientTask>();
    // Test starting a task
    asc.runTask(tct, "foo");
    assert(asc.test_currentTask() == tct);
    // Try and start it again will busy, as one is running
    asc.runTask(std::make_shared<TestClientTask>(), "foo");
    assert(asc.test_currentTask() == tct);
    
    asc.endTask();
    assert(asc.test_currentTask() == 0);
    asc.endTask();
    assert(asc.test_currentTask() == 0);

    tct = std::make_unique<TestClientTask>();
    asc.runTask(tct, "foo");
    // Pass in an operation while a task is running.
    asc.test_operation(op);
    assert(asc.test_currentTask() == tct);
    tct->make_complete();
    asc.test_operation(op);
    assert(asc.test_currentTask() == 0);

    tct = std::make_unique<TestClientTask>();
    asc.runTask(tct, "foo");
    assert(asc.test_currentTask() == tct);
    // Pass in an operation while a task is running.
    asc.test_operation(op);
    assert(asc.test_currentTask() == tct);

    {
        Atlas::Objects::Operation::Info op;

        asc.test_operation(op);
        Atlas::Objects::Entity::Anonymous arg;
        op->setArgs1(arg);
        asc.test_operation(op);
        op->setRefno(23);
        asc.test_operation(op);
        op->setRefno(asc.newSerialNo());
        asc.test_operation(op);
        op->setFrom("1");
        asc.test_operation(op);
    }

    {
        Atlas::Objects::Operation::Error op;

        asc.test_operation(op);
        Atlas::Objects::Entity::Anonymous arg;
        op->setArgs1(arg);
        asc.test_operation(op);
        arg->setAttr("message", 1);
        asc.test_operation(op);
        arg->setAttr("message", "Real message");
        asc.test_operation(op);
    }

    {
        Atlas::Objects::Operation::Appearance op;

        asc.test_operation(op);
    }

    {
        Atlas::Objects::Operation::Disappearance op;

        asc.test_operation(op);
    }

    {
        Atlas::Objects::Operation::Sight op;

        asc.test_operation(op);
    }

    {
        Atlas::Objects::Operation::Sound op;

        asc.test_operation(op);
    }
    assert(asc.test_currentTask() == tct);

    // Verify these bail out cleanly when unconnected
    asc.poll(std::chrono::seconds::zero());
    asc.login("foo", "bar");
    asc.create("player", "foo", "bar");

    int ret = asc.connect("localhost", 2323);
    assert(ret != 0);
    ret = asc.connectLocal("/sys/thereisnofilehere");
    assert(ret != 0);

}

// stubs

#include "common/debug.h"
#include "common/system.h"
#include "../stubs/common/stublog.h"
using Atlas::Message::Element;

ClientTask::ClientTask() : m_complete(false)
{
}

ClientTask::~ClientTask()
{
}
