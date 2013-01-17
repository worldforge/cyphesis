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
#include "TestWorld.h"

#include "server/ServerAccount.h"

#include "server/Connection.h"
#include "server/ServerRouting.h"
#include "server/TeleportAuthenticator.h"

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

    static Entity * TestWorld_addNewEntity_ret_value;
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

    static Entity * get_TestWorld_addNewEntity_ret_value();
};

Entity * ServerAccounttest::TestWorld_addNewEntity_ret_value;

Entity * ServerAccounttest::get_TestWorld_addNewEntity_ret_value()
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
    Entity * gw = new Entity(compose("%1", m_id_counter),
                             m_id_counter++);
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

    TeleportAuthenticator::init();
}

void ServerAccounttest::teardown()
{
    TeleportAuthenticator::del();

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

    delete TestWorld_addNewEntity_ret_value;
    TestWorld_addNewEntity_ret_value = 0;
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

    delete TestWorld_addNewEntity_ret_value;
    TestWorld_addNewEntity_ret_value = 0;
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

    delete TestWorld_addNewEntity_ret_value;
    TestWorld_addNewEntity_ret_value = 0;
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

    delete TestWorld_addNewEntity_ret_value;
    TestWorld_addNewEntity_ret_value = 0;
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

    delete TestWorld_addNewEntity_ret_value;
    TestWorld_addNewEntity_ret_value = 0;
}

void ServerAccounttest::test_addNewEntity_failed()
{
    TestWorld_addNewEntity_ret_value = 0;

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

    delete TestWorld_addNewEntity_ret_value;
    TestWorld_addNewEntity_ret_value = 0;
}

void ServerAccounttest::test_addNewEntity_unconnected()
{
    m_account->m_connection = 0;

    std::string type_str("unimportant_string");
    RootEntity arg;

    LocatedEntity * e = m_account->addNewEntity(type_str, arg, arg);

    ASSERT_NULL(e);
}

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    Entity * ne = ServerAccounttest::get_TestWorld_addNewEntity_ret_value();
    if (ne != 0) {
        ne->m_location.m_loc = &m_gameWorld;
        ne->m_location.m_pos = Point3D(0,0,0);
        assert(ne->m_location.isValid());
    }
    return ne;
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

Account::Account(Connection * conn,
                 const std::string & uname,
                 const std::string & passwd,
                 const std::string & id,
                 long intId) :
         ConnectableRouter(id, intId, conn),
         m_username(uname), m_password(passwd)
{
}

Account::~Account()
{
}

LocatedEntity * Account::addNewCharacter(const std::string & typestr,
                                  const RootEntity & ent,
                                  const Root & arg)
{
    return 0;
}

int Account::connectCharacter(LocatedEntity *chr)
{
    return 0;
}

const char * Account::getType() const
{
    return "account";
}

void Account::store() const
{
}

void Account::createObject(const std::string & type_str,
                           const Root & arg,
                           const Operation & op,
                           OpVector & res)
{
}

void Account::addToMessage(MapType & omap) const
{
}

void Account::addToEntity(const RootEntity & ent) const
{
}

void Account::externalOperation(const Operation & op, Link &)
{
}

void Account::operation(const Operation & op, OpVector & res)
{
}

void Account::LogoutOperation(const Operation &, OpVector &)
{
}

void Account::CreateOperation(const Operation &, OpVector &)
{
}

void Account::SetOperation(const Operation &, OpVector &)
{
}

void Account::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Account::TalkOperation(const Operation &, OpVector &)
{
}

void Account::LookOperation(const Operation &, OpVector &)
{
}

void Account::GetOperation(const Operation &, OpVector &)
{
}

void Account::OtherOperation(const Operation &, OpVector &)
{
}

Connection::Connection(CommSocket & client,
                       ServerRouting & svr,
                       const std::string & addr,
                       const std::string & id, long iid) :
            Link(client, id, iid), m_obsolete(false),
                                                m_server(svr)
{
}

Account * Connection::newAccount(const std::string & type,
                                 const std::string & username,
                                 const std::string & passwd,
                                 const std::string & id, long intId)
{
    return 0;
}

int Connection::verifyCredentials(const Account &,
                                  const Atlas::Objects::Root &) const
{
    return 0;
}

Connection::~Connection()
{
}

void Connection::externalOperation(const Operation & op, Link &)
{
}

void Connection::operation(const Operation &, OpVector &)
{
}

void Connection::LoginOperation(const Operation &, OpVector &)
{
}

void Connection::LogoutOperation(const Operation &, OpVector &)
{
}

void Connection::CreateOperation(const Operation &, OpVector &)
{
}

void Connection::GetOperation(const Operation &, OpVector &)
{
}

void Connection::addEntity(LocatedEntity * ent)
{
}

ConnectableRouter::ConnectableRouter(const std::string & id,
                                 long iid,
                                 Connection *c) :
                 Router(id, iid),
                 m_connection(c)
{
}

ConnectableRouter::~ConnectableRouter()
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

TeleportAuthenticator * TeleportAuthenticator::m_instance = NULL;

int TeleportAuthenticator::addTeleport(const std::string &entity_id,
                                        const std::string &possess_key)
{
    return 0;
}

int TeleportAuthenticator::removeTeleport(const std::string &entity_id)
{
    return 0;
}

LocatedEntity *TeleportAuthenticator::authenticateTeleport(const std::string &entity_id,
                                            const std::string &possess_key)
{
    return 0;
}

Persistence * Persistence::m_instance = NULL;

Persistence::Persistence() : m_db(*(Database*)0)
{
}

Persistence * Persistence::instance()
{
    if (m_instance == NULL) {
        m_instance = new Persistence();
    }
    return m_instance;
}

void Persistence::putAccount(const Account & ac)
{
}

Character::Character(const std::string & id, long intId) :
           Thing(id, intId),
               m_movement(*(Movement*)0),
               m_mind(0), m_externalMind(0)
{
}

Character::~Character()
{
}

void Character::operation(const Operation & op, OpVector &)
{
}

void Character::externalOperation(const Operation & op, Link &)
{
}


void Character::ImaginaryOperation(const Operation & op, OpVector &)
{
}

void Character::InfoOperation(const Operation & op, OpVector &)
{
}

void Character::TickOperation(const Operation & op, OpVector &)
{
}

void Character::TalkOperation(const Operation & op, OpVector &)
{
}

void Character::NourishOperation(const Operation & op, OpVector &)
{
}

void Character::UseOperation(const Operation & op, OpVector &)
{
}

void Character::WieldOperation(const Operation & op, OpVector &)
{
}

void Character::AttackOperation(const Operation & op, OpVector &)
{
}

void Character::ActuateOperation(const Operation & op, OpVector &)
{
}

void Character::mindActuateOperation(const Operation &, OpVector &)
{
}

void Character::mindAttackOperation(const Operation &, OpVector &)
{
}

void Character::mindCombineOperation(const Operation &, OpVector &)
{
}

void Character::mindCreateOperation(const Operation &, OpVector &)
{
}

void Character::mindDeleteOperation(const Operation &, OpVector &)
{
}

void Character::mindDivideOperation(const Operation &, OpVector &)
{
}

void Character::mindEatOperation(const Operation &, OpVector &)
{
}

void Character::mindGoalInfoOperation(const Operation &, OpVector &)
{
}

void Character::mindImaginaryOperation(const Operation &, OpVector &)
{
}

void Character::mindLookOperation(const Operation &, OpVector &)
{
}

void Character::mindMoveOperation(const Operation &, OpVector &)
{
}

void Character::mindSetOperation(const Operation &, OpVector &)
{
}

void Character::mindSetupOperation(const Operation &, OpVector &)
{
}

void Character::mindTalkOperation(const Operation &, OpVector &)
{
}

void Character::mindThoughtOperation(const Operation &, OpVector &)
{
}

void Character::mindTickOperation(const Operation &, OpVector &)
{
}

void Character::mindTouchOperation(const Operation &, OpVector &)
{
}

void Character::mindUpdateOperation(const Operation &, OpVector &)
{
}

void Character::mindUseOperation(const Operation &, OpVector &)
{
}

void Character::mindWieldOperation(const Operation &, OpVector &)
{
}


void Character::mindOtherOperation(const Operation &, OpVector &)
{
}

void Character::sendMind(const Operation & op, OpVector & res)
{
}

Thing::Thing(const std::string & id, long intId) :
       Entity(id, intId)
{
}

Thing::~Thing()
{
}

void Thing::DeleteOperation(const Operation & op, OpVector & res)
{
}

void Thing::MoveOperation(const Operation & op, OpVector & res)
{
}

void Thing::SetOperation(const Operation & op, OpVector & res)
{
}

void Thing::LookOperation(const Operation & op, OpVector & res)
{
}

void Thing::CreateOperation(const Operation & op, OpVector & res)
{
}

void Thing::UpdateOperation(const Operation & op, OpVector & res)
{
}

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

void Entity::callOperation(const Operation & op, OpVector & res)
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

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
}

void LocatedEntity::merge(const MapType & ent)
{
}

Link::Link(CommSocket & socket, const std::string & id, long iid) :
            Router(id, iid), m_encoder(0), m_commSocket(socket)
{
}

Link::~Link()
{
}

void Link::send(const Operation & op) const
{
}

void Link::disconnect()
{
}

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
    delete &m_gameWorld;
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    return 0;
}

LocatedEntity * BaseWorld::getEntity(long id) const
{
    return 0;
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

void Router::error(const Operation & op,
                   const std::string & errstring,
                   OpVector & res,
                   const std::string & to) const
{
    res.push_back(Atlas::Objects::Operation::Error());
}

Location::Location() : m_loc(0)
{
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

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

