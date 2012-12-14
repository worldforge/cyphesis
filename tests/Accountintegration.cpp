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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"

#include "server/Account.h"
#include "server/CommServer.h"
#include "server/EntityBuilder.h"
#include "server/Ruleset.h"
#include "server/ServerRouting.h"
#include "server/Connection.h"
#include "server/WorldRouter.h"

#include "rulesets/Character.h"
#include "rulesets/Domain.h"
#include "rulesets/Entity.h"
#include "rulesets/ExternalMind.h"

#include "common/CommSocket.h"
#include "common/Inheritance.h"
#include "common/SystemTime.h"

#include "TestWorld.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <cassert>

using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Imaginary;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Talk;
using Atlas::Objects::Operation::Move;

class TestCommSocket : public CommSocket {
  public:
    explicit TestCommSocket(CommServer & cs) : CommSocket(cs) { }
    virtual ~TestCommSocket() { }

    int getFd() const { return 0; }
    bool isOpen() const { return true; }
    bool eof() { return false; }
    int read() { return 0; }
    void dispatch() { }
    void disconnect() { }
    int flush() { return 0; }
};

class TestAccount : public Account {
  public:
    TestAccount(Connection * conn, const std::string & username,
                                   const std::string & passwd,
                                   const std::string & id, long intId) :
        Account(conn, username, passwd, id, intId) {
    }

    virtual int characterError(const Operation & op,
                               const Atlas::Objects::Root & ent,
                               OpVector & res) const {
        return 0;
    }
};

class Accountintegration : public Cyphesis::TestBase
{
    SystemTime * m_time;
    WorldRouter * m_world;

    ServerRouting * m_server;

    CommServer * m_commServer;

    CommSocket * m_tc;
    Connection * m_c;
    TestAccount * m_ac;

  public:
    Accountintegration();

    void setup();
    void teardown();

    void test_all();
    void test_all1();
    void test_all2();
    void test_all3();
    void test_all4();
    void test_all5();
    void test_all6();
    void test_all7();
    void test_SetOperation();
    void test_TalkOperation();
    void test_LogoutOperation();
    void test_connectCharacter_entity();
    void test_connectCharacter_character();
};

Accountintegration::Accountintegration()
{
    (void)new Domain;

    ADD_TEST(Accountintegration::test_all);
    ADD_TEST(Accountintegration::test_all1);
    ADD_TEST(Accountintegration::test_all2);
    ADD_TEST(Accountintegration::test_all3);
    ADD_TEST(Accountintegration::test_all4);
    ADD_TEST(Accountintegration::test_all5);
    ADD_TEST(Accountintegration::test_all6);
    ADD_TEST(Accountintegration::test_all7);
    ADD_TEST(Accountintegration::test_SetOperation);
    ADD_TEST(Accountintegration::test_TalkOperation);
    ADD_TEST(Accountintegration::test_LogoutOperation);
    ADD_TEST(Accountintegration::test_connectCharacter_entity);
    ADD_TEST(Accountintegration::test_connectCharacter_character);
}

void Accountintegration::setup()
{
    m_time = new SystemTime;
    m_world = new WorldRouter(*m_time);

    m_server = new ServerRouting(*m_world, "noruleset", "unittesting",
                         "1", 1, "2", 2);

    m_commServer = new CommServer;

    m_tc = new TestCommSocket(*m_commServer);
    m_c = new Connection(*m_tc, *m_server, "addr", "3", 3);
    m_ac = new TestAccount(m_c, "user", "password", "4", 4);
}

void Accountintegration::teardown()
{
    delete m_ac;
    delete m_world;
    EntityBuilder::del();
    Ruleset::del();
    Inheritance::clear();
}

void Accountintegration::test_all()
{
    Anonymous new_char;
    Entity * chr = m_ac->addNewCharacter("thing", new_char,
                                         RootEntity());
    assert(chr != 0);

    std::cout << "Test 1" << std::endl << std::flush;
}

void Accountintegration::test_all1()
{
    m_ac->getType();
}

void Accountintegration::test_all2()
{
    MapType emap;
    m_ac->addToMessage(emap);
}

void Accountintegration::test_all3()
{
    RootEntity ent;
    m_ac->addToEntity(ent);
}

void Accountintegration::test_all4()
{
    Create op;
    OpVector res;
    m_ac->operation(op, res);
    op->setArgs1(Root());
    m_ac->operation(op, res);
    Anonymous op_arg;
    op->setArgs1(op_arg);
    m_ac->operation(op, res);
    op_arg->setParents(std::list<std::string>());
    m_ac->operation(op, res);
    op_arg->setParents(std::list<std::string>(1, "game_entity"));
    m_ac->operation(op, res);
    op_arg->setName("Bob");
    m_ac->operation(op, res);
}

void Accountintegration::test_all5()
{
    Get op;
    OpVector res;
    m_ac->operation(op, res);
    op->setArgs1(Root());
    m_ac->operation(op, res);
    Anonymous op_arg;
    op->setArgs1(op_arg);
    m_ac->operation(op, res);
    op_arg->setParents(std::list<std::string>());
    m_ac->operation(op, res);
}

void Accountintegration::test_all6()
{
    Imaginary op;
    OpVector res;
    m_ac->operation(op, res);
    op->setArgs1(Root());
    m_ac->operation(op, res);
    op->setSerialno(1);
    m_ac->operation(op, res);
    Anonymous op_arg;
    op->setArgs1(op_arg);
    m_ac->operation(op, res);
    op_arg->setLoc("2");
    m_ac->operation(op, res);
}

void Accountintegration::test_all7()
{
    Anonymous new_char;
    Entity * chr = m_ac->addNewCharacter("thing", new_char,
                                           RootEntity());

    Look op;
    OpVector res;
    m_ac->operation(op, res);
    op->setArgs1(Root());
    m_ac->operation(op, res);
    Anonymous op_arg;
    op->setArgs1(op_arg);
    m_ac->operation(op, res);
    op_arg->setId("1");
    m_ac->operation(op, res);
    op_arg->setId(chr->getId());
    m_ac->operation(op, res);
}

void Accountintegration::test_SetOperation()
{
    Anonymous new_char;
    Entity * chr = m_ac->addNewCharacter("thing", new_char,
                                         RootEntity());
    BBox newBox(WFMath::Point<3>(-0.5, -0.5, 0.0),
                WFMath::Point<3>(-0.5, -0.5, 2.0));
    chr->m_location.setBBox(newBox);

    Anonymous op_arg;

    op_arg->setId(chr->getId());
    op_arg->setAttr("guise", "foo");
    op_arg->setAttr("height", 3.0);
    op_arg->setAttr("tasks", ListType());

    Set op;
    op->setArgs1(op_arg);

    OpVector res;
    m_ac->operation(op, res);

    // FIXME Ensure character has been modified
}

void Accountintegration::test_TalkOperation()
{
    Anonymous op_arg;
    op_arg->setParents(std::list<std::string>());
    op_arg->setLoc("1");

    Talk op;
    op->setSerialno(1);
    op->setArgs1(op_arg);

    OpVector res;
    m_ac->operation(op, res);

    // FIXME Sound op should have been sent to the lobby
}

void Accountintegration::test_LogoutOperation()
{
    Logout op;
    op->setSerialno(1);

    Anonymous op_arg;
    op_arg->setParents(std::list<std::string>());
    op->setArgs1(op_arg);

    OpVector res;
    m_ac->operation(op, res);

    // FIXME Account should have been removed from Lobby, and also from
    // Connection
}

void Accountintegration::test_connectCharacter_entity()
{
    Entity *e = new Entity("7", 7);

    int ret = m_ac->connectCharacter(e);
    ASSERT_NOT_EQUAL(ret, 0);
}

void Accountintegration::test_connectCharacter_character()
{
    Character * e = new Character("8", 8);

    int ret = m_ac->connectCharacter(e);
    ASSERT_EQUAL(ret, 0);
    ASSERT_NOT_NULL(e->m_externalMind);
    ASSERT_TRUE(e->m_externalMind->isLinkedTo(m_c));
}

int main()
{
    database_flag = false;

    Accountintegration t;

    return t.run();
}

void TestWorld::message(const Operation & op, Entity & ent)
{
}

Entity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}
