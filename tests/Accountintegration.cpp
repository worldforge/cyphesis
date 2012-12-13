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

#include "null_stream.h"
#include "TestBase.h"

#include "server/Account.h"
#include "server/CommServer.h"
#include "server/ServerRouting.h"
#include "server/CommClient.h"
#include "server/Connection.h"
#include "server/WorldRouter.h"

#include "rulesets/Character.h"
#include "rulesets/Domain.h"
#include "rulesets/Entity.h"

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

class TestCommClient : public CommClient<null_stream> {
  public:
    TestCommClient(CommServer & cs) : CommClient<null_stream>(cs, "") { }
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

    TestCommClient * m_tc;
    Connection * m_c;
    TestAccount * m_ac;

  public:
    Accountintegration();

    void setup();
    void teardown();

    void test_all();
};

Accountintegration::Accountintegration()
{
    (void)new Domain;

    ADD_TEST(Accountintegration::test_all);
}

void Accountintegration::setup()
{
    m_time = new SystemTime;
    m_world = new WorldRouter(*m_time);

    m_server = new ServerRouting(*m_world, "noruleset", "unittesting",
                         "1", 1, "2", 2);

    m_commServer = new CommServer;

    m_tc = new TestCommClient(*m_commServer);
    m_c = new Connection(*m_tc, *m_server, "addr", "3", 3);
    m_ac = new TestAccount(m_c, "user", "password", "4", 4);
}

void Accountintegration::teardown()
{
    delete m_ac;
}

void Accountintegration::test_all()
{
    {
        Anonymous new_char;
        Entity * chr = m_ac->addNewCharacter("thing", new_char,
                                             RootEntity());
        assert(chr != 0);
    }

    m_ac->getType();

    {
        MapType emap;
        m_ac->addToMessage(emap);
    }

    {
        RootEntity ent;
        m_ac->addToEntity(ent);
    }

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

    {
        Anonymous new_char;
        Entity * chr = m_ac->addNewCharacter("thing", new_char,
                                               RootEntity());

        Set op;
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
        op_arg->setAttr("guise", "foo");
        m_ac->operation(op, res);
        op_arg->setAttr("height", 3.0);
        m_ac->operation(op, res);
        BBox newBox(WFMath::Point<3>(-0.5, -0.5, 0.0),
                    WFMath::Point<3>(-0.5, -0.5, 2.0));
        chr->m_location.setBBox(newBox);
        op_arg->setAttr("height", 3.0);
        m_ac->operation(op, res);
        op_arg->setAttr("tasks", ListType());
        m_ac->operation(op, res);
    }

    {
        Talk op;
        OpVector res;
        m_ac->operation(op, res);
        op->setArgs1(Root());
        m_ac->operation(op, res);
        Anonymous op_arg;
        op->setArgs1(op_arg);
        m_ac->operation(op, res);
        op_arg->setParents(std::list<std::string>());
        m_ac->operation(op, res);
        op->setSerialno(1);
        m_ac->operation(op, res);
        op_arg->setLoc("1");
        m_ac->operation(op, res);
    }

    {
        Logout op;
        OpVector res;
        m_ac->operation(op, res);
        op->setSerialno(1);
        m_ac->operation(op, res);
        op->setArgs1(Root());
        m_ac->operation(op, res);
        Anonymous op_arg;
        op->setArgs1(op_arg);
        m_ac->operation(op, res);
        op_arg->setParents(std::list<std::string>());
        m_ac->operation(op, res);
    }

    {
        // Move has no meaning
        Move op;
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

    {
        Entity e("7", 7);

        int ret = m_ac->connectCharacter(&e);
        assert(ret == -1);
    }

    {
        Character e("8", 8);

        int ret = m_ac->connectCharacter(&e);
        assert(ret == 0);
    }

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
