// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#include "../TestBase.h"

#include "rules/simulation/Task.h"

#include "rules/simulation/Entity.h"
#include "rules/Script.h"

#include <Atlas/Objects/Generic.h>
#include <Atlas/Objects/RootEntity.h>

#include <iostream>

#include <cassert>
#include "pythonbase/PythonMalloc.h"
#include "pythonbase/Python_API.h"

struct TestTask : public Task {
    TestTask(UsageInstance usageInstance, Py::Object script): Task(usageInstance, script) {}
    bool getObsolete() const {
        return m_obsolete;
    }
};


class Tasktest : public Cyphesis::TestBase
{
  private:
    Ref<LocatedEntity> chr;
    UsageInstance usageInstance;
    Py::Dict script;
    Ref<TestTask> m_task;

    static bool Script_operation_called;
    static HandlerResult Script_operation_ret;
  public:
    Tasktest();

    void setup();
    void teardown();

    void test_obsolete();
    void test_irrelevant();
    void test_operation();
    void test_sequence();
    void test_setScript();
    void test_initTask_script();
    void test_initTask_script_fail();

    static HandlerResult get_Script_operation_ret();
};

bool Tasktest::Script_operation_called = false;
HandlerResult Tasktest::Script_operation_ret = OPERATION_IGNORED;

HandlerResult Tasktest::get_Script_operation_ret()
{
    Script_operation_called = true;
    return Script_operation_ret;
}

Tasktest::Tasktest()
{

    ADD_TEST(Tasktest::test_obsolete);
//    ADD_TEST(Tasktest::test_irrelevant);
//    ADD_TEST(Tasktest::test_operation);
//    ADD_TEST(Tasktest::test_sequence);
//    ADD_TEST(Tasktest::test_setScript);
//    ADD_TEST(Tasktest::test_initTask_script);
//    ADD_TEST(Tasktest::test_initTask_script_fail);
}

void Tasktest::setup()
{
    Script_operation_called = false;

    chr = new Entity(3);
    usageInstance = UsageInstance{};
    usageInstance.actor = chr;
    script = Py::Dict();

    m_task = new TestTask(usageInstance, script);
}

void Tasktest::teardown()
{
}

void Tasktest::test_obsolete()
{
    ASSERT_EQUAL(m_task->getObsolete(), false);
    ASSERT_EQUAL(m_task->obsolete(), false);
}

void Tasktest::test_irrelevant()
{
    ASSERT_EQUAL(m_task->getObsolete(), false);
    ASSERT_EQUAL(m_task->obsolete(), false);
    m_task->irrelevant();
    ASSERT_EQUAL(m_task->getObsolete(), true);
    ASSERT_EQUAL(m_task->obsolete(), true);
}

void Tasktest::test_operation()
{
//    Operation op;
//    OpVector res;
//
//    m_task->operation(op, res);
//
//    ASSERT_EQUAL(Script_operation_called, false);
}

void Tasktest::test_sequence()
{
//    m_task->nextTick(1.5);
//
//    Atlas::Message::Element val;
//    m_task->getAttr("foo", val);
//    assert(val.isNone());
//    m_task->setAttrValue("foo", 1);
//    m_task->getAttr("foo", val);
//    assert(val.isInt());
//
//    assert(!m_task->obsolete());
//
//    OpVector res;
//
//    assert(res.empty());
//
//    Atlas::Objects::Operation::Generic c;
//    c->setParent("generic");
//
//    m_task->initTask(c, res);
//
//    Operation op;
//
//    m_task->operation(op, res);
//
//    m_task->irrelevant();
//
//    assert(m_task->obsolete());
}

void Tasktest::test_setScript()
{
//    Script * s1 = new Script;
//    Script * s2 = new Script;
//
//    m_task->setScript(s1);
//
//    ASSERT_EQUAL(m_task->m_script, s1);
//
//    m_task->setScript(s2);
//
//    ASSERT_EQUAL(m_task->m_script, s2);
}

void Tasktest::test_initTask_script()
{
//    Script_operation_ret = OPERATION_BLOCKED;
//
//    Script * s1 = new Script;
//    m_task->setScript(s1);
//
//    Operation op;
//    OpVector res;
//
//    m_task->initTask("1", res);
//
//    ASSERT_EQUAL(m_task->obsolete(), false);
//    ASSERT_EQUAL(res.size(), 1u);
}

void Tasktest::test_initTask_script_fail()
{
//    Script_operation_ret = OPERATION_IGNORED;
//
//    Script * s1 = new Script;
//    m_task->setScript(s1);
//
//    Operation op;
//    OpVector res;
//
//    m_task->initTask("1", res);
//
//    ASSERT_TRUE(m_task->obsolete());
//    ASSERT_TRUE(res.empty());
}


int main()
{

    setupPythonMalloc();

    init_python_api({});
    auto result = Tasktest{}.run();
    shutdown_python_api();
    return result;
}

// stubs

#include "common/log.h"

#include "../stubs/common/stubcustom.h"
#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/rules/simulation/stubScriptUtils.h"
#include "../stubs/common/stubProperty.h"
//#include "../stubs/rules/simulation/stubActionsProperty.h"
#include "../stubs/rules/simulation/stubBaseWorld.h"

#define STUB_Script_operation
HandlerResult Script::operation(const std::string & opname,
                                const Atlas::Objects::Operation::RootOperation & op,
                                OpVector & res)
{
    return Tasktest::get_Script_operation_ret();
}
#include "../stubs/rules/stubScript.h"
#include "../stubs/common/stublog.h"
#include "../stubs/rules/simulation/stubActionsProperty.h"

