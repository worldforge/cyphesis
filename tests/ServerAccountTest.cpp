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

#include "TestBase.h"
#include "TestWorld.h"

#include "server/ServerAccount.h"

#include "server/Connection.h"
#include "server/ServerRouting.h"
#include "server/PossessionAuthenticator.h"

#include "rulesets/Character.h"

#include "common/CommSocket.h"
#include "common/compose.hpp"

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::RootOperation;

using String::compose;

class ServerAccounttest : public Cyphesis::TestBase
{
  protected:
    long m_id_counter;

    ServerRouting * m_server;
    Connection * m_connection;
    ServerAccount * m_account;

    static Ref<Entity> TestWorld_addNewEntity_ret_value;
  public:
    ServerAccounttest();

    void setup();
    void teardown();

    void test_getType();
    void test_characterError();
    void test_createObject_not_obj();
    void test_createObject_non_entity();
    void test_createObject_failed();
    void test_createObject_success();
    void test_createObject_success_refo();
    void test_createObject_no_possess();
    void test_createObject_possess_non_string();
    void test_createObject_success_possess();
    void test_addNewEntity_failed();
    void test_addNewEntity_success();
    void test_addNewEntity_unconnected();

    static Ref<Entity> get_TestWorld_addNewEntity_ret_value();
};

Ref<Entity> ServerAccounttest::TestWorld_addNewEntity_ret_value;

Ref<Entity> ServerAccounttest::get_TestWorld_addNewEntity_ret_value()
{
    return TestWorld_addNewEntity_ret_value;
}

ServerAccounttest::ServerAccounttest() : m_id_counter(0L),
                                         m_server(0),
                                         m_connection(0),
                                         m_account(0)
{
    ADD_TEST(ServerAccounttest::test_getType);
    ADD_TEST(ServerAccounttest::test_characterError);
    ADD_TEST(ServerAccounttest::test_createObject_not_obj);
    ADD_TEST(ServerAccounttest::test_createObject_non_entity);
    ADD_TEST(ServerAccounttest::test_createObject_failed);
    ADD_TEST(ServerAccounttest::test_createObject_success);
    ADD_TEST(ServerAccounttest::test_createObject_success_refo);
    ADD_TEST(ServerAccounttest::test_createObject_no_possess);
    ADD_TEST(ServerAccounttest::test_createObject_possess_non_string);
    ADD_TEST(ServerAccounttest::test_createObject_success_possess);
    ADD_TEST(ServerAccounttest::test_addNewEntity_failed);
    ADD_TEST(ServerAccounttest::test_addNewEntity_success);
    ADD_TEST(ServerAccounttest::test_addNewEntity_unconnected);
}

void ServerAccounttest::setup()
{

    Ref<Entity> gw = new Entity(compose("%1", m_id_counter),
                             m_id_counter++);
    TestWorld::extension.addNewEntityFn = [&, gw](const std::string &,
                        const Atlas::Objects::Entity::RootEntity &)
    {
        auto ne = ServerAccounttest::get_TestWorld_addNewEntity_ret_value();
        if (ne) {
            ne->m_location.m_loc = gw;
            ne->m_location.m_pos = Point3D(0,0,0);
            assert(ne->m_location.isValid());
        }
        return ne;
    };
    m_server = new ServerRouting(*new TestWorld(*gw),
                                 "5529d7a4-0158-4dc1-b4a5-b5f260cac635",
                                 "bad621d4-616d-4faf-b9e6-471d12b139a9",
                                 compose("%1", m_id_counter), m_id_counter++,
                                 compose("%1", m_id_counter), m_id_counter++);
    m_connection = new Connection(*(CommSocket*)0, *m_server,
                                  "8d18a4e8-f14f-4a46-997e-ada120d5438f",
                                  compose("%1", m_id_counter), m_id_counter++);
    m_account = new ServerAccount(m_connection,
                                  "6c9f3236-5de7-4ba4-8b7a-b0222df0af38",
                                  "fa1a03a2-a745-4033-85cb-bb694e921e62",
                                  compose("%1", m_id_counter), m_id_counter++);

    PossessionAuthenticator::init();
}

void ServerAccounttest::teardown()
{
    PossessionAuthenticator::del();

    delete m_server;
    delete m_account;
    delete m_connection;
}

void ServerAccounttest::test_getType()
{
    const char * type = m_account->getType();

    ASSERT_EQUAL(std::string("server"), type);
}

void ServerAccounttest::test_characterError()
{
    RootOperation op;
    Root arg;
    OpVector res;

    int result = m_account->characterError(op, arg, res);

    ASSERT_EQUAL(result, -1);
}

void ServerAccounttest::test_createObject_not_obj()
{
    std::string type_str("unimportant_string");
    Root arg;
    RootOperation op;
    OpVector res;

    arg->setObjtype("foo");

    m_account->createObject(type_str, arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void ServerAccounttest::test_createObject_non_entity()
{
    std::string type_str("unimportant_string");
    Root arg;
    RootOperation op;
    OpVector res;

    m_account->createObject(type_str, arg, op, res);

    ASSERT_TRUE(res.empty());
}

void ServerAccounttest::test_createObject_failed()
{
    TestWorld_addNewEntity_ret_value = 0;

    std::string type_str("unimportant_string");
    RootEntity arg;
    RootOperation op;
    OpVector res;

    m_account->createObject(type_str, arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void ServerAccounttest::test_createObject_success()
{
    long cid = m_id_counter++;
    TestWorld_addNewEntity_ret_value = new Character(compose("%1", cid), cid);

    std::string type_str("unimportant_string");
    RootEntity arg;
    RootOperation op;
    OpVector res;

    m_account->createObject(type_str, arg, op, res);


    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::INFO_NO);

    TestWorld_addNewEntity_ret_value = nullptr;
}

void ServerAccounttest::test_createObject_success_refo()
{
    long cid = m_id_counter++;
    TestWorld_addNewEntity_ret_value = new Character(compose("%1", cid), cid);

    std::string type_str("unimportant_string");
    RootEntity arg;
    RootOperation op;
    op->setSerialno(44295);
    OpVector res;

    m_account->createObject(type_str, arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::INFO_NO);
    ASSERT_TRUE(!reply->isDefaultRefno());
    ASSERT_EQUAL(reply->getRefno(), op->getSerialno());

    TestWorld_addNewEntity_ret_value = nullptr;
}

void ServerAccounttest::test_createObject_no_possess()
{
    long cid = m_id_counter++;
    TestWorld_addNewEntity_ret_value = new Character(compose("%1", cid), cid);

    std::string type_str("unimportant_string");
    RootEntity arg;
    RootEntity arg2;
    RootOperation op;
    op->setArgs1(arg);
    op->modifyArgs().push_back(arg2);
    OpVector res;

    m_account->createObject(type_str, arg, op, res);

    // FIXME No error to the client!
    ASSERT_TRUE(res.empty());

    TestWorld_addNewEntity_ret_value = nullptr;
}

void ServerAccounttest::test_createObject_possess_non_string()
{
    long cid = m_id_counter++;
    TestWorld_addNewEntity_ret_value = new Character(compose("%1", cid), cid);

    std::string type_str("unimportant_string");
    RootEntity arg;
    RootEntity arg2;
    arg2->setAttr("possess_key", 23);
    RootOperation op;
    op->setArgs1(arg);
    op->modifyArgs().push_back(arg2);
    OpVector res;

    m_account->createObject(type_str, arg, op, res);

    // FIXME No error to the client!
    ASSERT_TRUE(res.empty());

    TestWorld_addNewEntity_ret_value = nullptr;
}

void ServerAccounttest::test_createObject_success_possess()
{
    long cid = m_id_counter++;
    TestWorld_addNewEntity_ret_value = new Character(compose("%1", cid), cid);

    std::string type_str("unimportant_string");
    RootEntity arg;
    RootEntity arg2;
    arg2->setAttr("possess_key", "unimportant_string");
    RootOperation op;
    op->setArgs1(arg);
    op->modifyArgs().push_back(arg2);
    OpVector res;

    m_account->createObject(type_str, arg, op, res);


    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::INFO_NO);

    TestWorld_addNewEntity_ret_value = nullptr;
}

void ServerAccounttest::test_addNewEntity_failed()
{
    TestWorld_addNewEntity_ret_value = nullptr;

    std::string type_str("unimportant_string");
    RootEntity arg;

    LocatedEntity * e = m_account->addNewEntity(type_str, arg, arg);

    ASSERT_NULL(e);
}

void ServerAccounttest::test_addNewEntity_success()
{
    long cid = m_id_counter++;
    Entity * c = new Character(compose("%1", cid), cid);
    TestWorld_addNewEntity_ret_value = c;

    std::string type_str("unimportant_string");
    RootEntity arg;

    LocatedEntity * e = m_account->addNewEntity(type_str, arg, arg);

    ASSERT_EQUAL(c, e);

    TestWorld_addNewEntity_ret_value = nullptr;
}

void ServerAccounttest::test_addNewEntity_unconnected()
{
    m_account->m_connection = 0;

    std::string type_str("unimportant_string");
    RootEntity arg;

    LocatedEntity * e = m_account->addNewEntity(type_str, arg, arg);

    ASSERT_NULL(e);
}


int main()
{
    ServerAccounttest t;

    return t.run();
}

// stubs

#include "server/Connection.h"
#include "server/Persistence.h"

#include "common/globals.h"
#include "common/id.h"
#include "common/log.h"

#include <cstdlib>

#include "stubs/server/stubAccount.h"
#include "stubs/server/stubConnection.h"


ConnectableRouter::ConnectableRouter(const std::string & id,
                                 long iid,
                                 Connection *c) :
                 Router(id, iid),
                 m_connection(c)
{
}

ServerRouting::ServerRouting(BaseWorld & wrld,
                             const std::string & ruleset,
                             const std::string & name,
                             const std::string & id, long intId,
                             const std::string & lId, long lIntId) :
        Router(id, intId),
        m_svrRuleset(ruleset), m_svrName(name),
        m_numClients(0), m_world(wrld), m_lobby(*(Lobby*)0)
{
}

ServerRouting::~ServerRouting()
{
    delete &m_world;
}

void ServerRouting::addToMessage(Atlas::Message::MapType & omap) const
{
}

void ServerRouting::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

Account * ServerRouting::getAccountByName(const std::string & username)
{
    return 0;
}

void ServerRouting::addAccount(Account * a)
{
}

void ServerRouting::externalOperation(const Operation & op, Link &)
{
}

void ServerRouting::operation(const Operation &, OpVector &)
{
}

#include "stubs/server/stubTeleportAuthenticator.h"
#include "stubs/server/stubPersistence.h"
#include "stubs/rulesets/stubThing.h"
#include "stubs/rulesets/stubCharacter.h"
#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/common/stubLink.h"
#include "stubs/rulesets/stubBaseWorld.h"



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

void Router::error(const Operation & op,
                   const std::string & errstring,
                   OpVector & res,
                   const std::string & to) const
{
    res.push_back(Atlas::Objects::Operation::Error());
}
#include "stubs/rulesets/stubLocation.h"


void logEvent(LogEvent lev, const std::string & msg)
{
}

void log(LogLevel lvl, const std::string & msg)
{
}

bool database_flag = false;

#include <common/Shaker.h>

Shaker::Shaker()
{
}
std::string Shaker::generateSalt(size_t length)
{
    return "";
}

