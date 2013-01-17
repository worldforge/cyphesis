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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"

#include "rulesets/Task.h"

#include "rulesets/Entity.h"
#include "rulesets/Script.h"

#include <Atlas/Objects/Generic.h>
#include <Atlas/Objects/RootEntity.h>

#include <iostream>

#include <cassert>

class Tasktest : public Cyphesis::TestBase
{
  private:
    LocatedEntity * chr;
    Task * m_task;

    static bool Script_operation_called;
    static bool Script_operation_ret;
  public:
    Tasktest();

    void setup();
    void teardown();

    void test_obsolete();
    void test_irrelevant();
    void test_operation();
    void test_sequence();
    void test_setScript();
    void test_operation_script();
    void test_initTask_script();
    void test_initTask_script_fail();

    static bool get_Script_operation_ret();
};

bool Tasktest::Script_operation_called = false;
bool Tasktest::Script_operation_ret = true;

bool Tasktest::get_Script_operation_ret()
{
    Script_operation_called = true;
    return Script_operation_ret;
}

Tasktest::Tasktest()
{
    ADD_TEST(Tasktest::test_obsolete);
    ADD_TEST(Tasktest::test_irrelevant);
    ADD_TEST(Tasktest::test_operation);
    ADD_TEST(Tasktest::test_sequence);
    ADD_TEST(Tasktest::test_setScript);
    ADD_TEST(Tasktest::test_operation_script);
    ADD_TEST(Tasktest::test_initTask_script);
    ADD_TEST(Tasktest::test_initTask_script_fail);
}

void Tasktest::setup()
{
    Script_operation_called = false;

    chr = new Entity("3", 3);

    m_task = new Task(*chr);
}

void Tasktest::teardown()
{
    delete m_task;

    delete chr;
}

void Tasktest::test_obsolete()
{
    ASSERT_EQUAL(m_task->m_obsolete, false);
    ASSERT_EQUAL(m_task->obsolete(), false);
}

void Tasktest::test_irrelevant()
{
    ASSERT_EQUAL(m_task->m_obsolete, false);
    ASSERT_EQUAL(m_task->obsolete(), false);
    m_task->irrelevant();
    ASSERT_EQUAL(m_task->m_obsolete, true);
    ASSERT_EQUAL(m_task->obsolete(), true);
}

void Tasktest::test_operation()
{
    Operation op;
    OpVector res;

    m_task->operation(op, res);

    ASSERT_EQUAL(Script_operation_called, false);
}

void Tasktest::test_sequence()
{
    m_task->nextTick(1.5);

    Atlas::Message::Element val;
    m_task->getAttr("foo", val);
    assert(val.isNone());
    m_task->setAttr("foo", 1);
    m_task->getAttr("foo", val);
    assert(val.isInt());

    assert(!m_task->obsolete());

    OpVector res;

    assert(res.empty());

    Atlas::Objects::Operation::Generic c;
    c->setParents(std::list<std::string>(1, "generic"));

    m_task->initTask(c, res);

    Operation op;

    m_task->operation(op, res);

    m_task->irrelevant();

    assert(m_task->obsolete());
}

void Tasktest::test_setScript()
{
    Script * s1 = new Script;
    Script * s2 = new Script;

    m_task->setScript(s1);

    ASSERT_EQUAL(m_task->m_script, s1);

    m_task->setScript(s2);

    ASSERT_EQUAL(m_task->m_script, s2);
}

void Tasktest::test_operation_script()
{
    Script * s1 = new Script;
    m_task->setScript(s1);

    Operation op;
    OpVector res;

    m_task->operation(op, res);

    ASSERT_EQUAL(Script_operation_called, true);
}

void Tasktest::test_initTask_script()
{
    Script_operation_ret = true;

    Script * s1 = new Script;
    m_task->setScript(s1);

    Operation op;
    OpVector res;

    m_task->initTask(op, res);

    ASSERT_EQUAL(m_task->obsolete(), false);
    ASSERT_EQUAL(res.size(), 1u);
}

void Tasktest::test_initTask_script_fail()
{
    Script_operation_ret = false;

    Script * s1 = new Script;
    m_task->setScript(s1);

    Operation op;
    OpVector res;

    m_task->initTask(op, res);

    ASSERT_EQUAL(m_task->obsolete(), true);
    ASSERT_TRUE(res.empty());
}

int main()
{
    Tasktest t;
    return t.run();
}

// stubs

#include "common/log.h"

namespace Atlas { namespace Objects { namespace Operation {
int TICK_NO = -1;
} } }

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId), m_motion(0)
{
}

Entity::~Entity()
{
}

void Entity::destroy()
{
    destroyed.emit();
}

void Entity::ActuateOperation(const Operation &, OpVector &)
{
}

void Entity::AppearanceOperation(const Operation &, OpVector &)
{
}

void Entity::AttackOperation(const Operation &, OpVector &)
{
}

void Entity::CombineOperation(const Operation &, OpVector &)
{
}

void Entity::CreateOperation(const Operation &, OpVector &)
{
}

void Entity::DeleteOperation(const Operation &, OpVector &)
{
}

void Entity::DisappearanceOperation(const Operation &, OpVector &)
{
}

void Entity::DivideOperation(const Operation &, OpVector &)
{
}

void Entity::EatOperation(const Operation &, OpVector &)
{
}

void Entity::GetOperation(const Operation &, OpVector &)
{
}

void Entity::InfoOperation(const Operation &, OpVector &)
{
}

void Entity::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Entity::LookOperation(const Operation &, OpVector &)
{
}

void Entity::MoveOperation(const Operation &, OpVector &)
{
}

void Entity::NourishOperation(const Operation &, OpVector &)
{
}

void Entity::SetOperation(const Operation &, OpVector &)
{
}

void Entity::SightOperation(const Operation &, OpVector &)
{
}

void Entity::SoundOperation(const Operation &, OpVector &)
{
}

void Entity::TalkOperation(const Operation &, OpVector &)
{
}

void Entity::TickOperation(const Operation &, OpVector &)
{
}

void Entity::TouchOperation(const Operation &, OpVector &)
{
}

void Entity::UpdateOperation(const Operation &, OpVector &)
{
}

void Entity::UseOperation(const Operation &, OpVector &)
{
}

void Entity::WieldOperation(const Operation &, OpVector &)
{
}

void Entity::externalOperation(const Operation & op, Link &)
{
}

void Entity::operation(const Operation & op, OpVector & res)
{
}

void Entity::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Entity::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

PropertyBase * Entity::setAttr(const std::string & name,
                               const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return 0;
}

void Entity::installHandler(int class_no, Handler handler)
{
}

void Entity::installDelegate(int class_no, const std::string & delegate)
{
}

Domain * Entity::getMovementDomain()
{
    return 0;
}

void Entity::sendWorld(const Operation & op)
{
}

void Entity::onContainered()
{
}

void Entity::onUpdated()
{
}

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

int LocatedEntity::getAttr(const std::string & name,
                           Atlas::Message::Element & attr) const
{
    return -1;
}

int LocatedEntity::getAttrType(const std::string & name,
                               Atlas::Message::Element & attr,
                               int type) const
{
    return -1;
}

PropertyBase * LocatedEntity::setAttr(const std::string & name,
                                      const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * LocatedEntity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * LocatedEntity::setProperty(const std::string & name,
                                          PropertyBase * prop)
{
    return 0;
}

void LocatedEntity::installHandler(int, Handler)
{
}

void LocatedEntity::installDelegate(int, const std::string &)
{
}

void LocatedEntity::destroy()
{
}

Domain * LocatedEntity::getMovementDomain()
{
    return 0;
}

void LocatedEntity::sendWorld(const Operation & op)
{
}

void LocatedEntity::onContainered()
{
}

void LocatedEntity::onUpdated()
{
}

Script::Script()
{
}

Script::~Script()
{
}

bool Script::operation(const std::string & opname,
                       const Atlas::Objects::Operation::RootOperation & op,
                       OpVector & res)
{
   return Tasktest::get_Script_operation_ret();
}

void Script::hook(const std::string & function, LocatedEntity * entity)
{
}

Router::Router(const std::string & id, long intId) : m_id(id),
                                                             m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

Location::Location() : m_loc(0)
{
}

void log(LogLevel lvl, const std::string & msg)
{
}
