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
#include "null_stream.h"

#include "server/Connection.h"

#include "server/Account.h"
#include "server/CommClient.h"
#include "server/CommServer.h"
#include "rulesets/ExternalMind.h"
#include "rulesets/ExternalProperty.h"
#include "server/Lobby.h"
#include "server/Player.h"
#include "server/ServerRouting.h"

#include "rulesets/Character.h"

#include "common/compose.hpp"
#include "common/Inheritance.h"
#include "common/log.h"
#include "common/SystemTime.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cstdlib>
#include <cstdio>

#include <cassert>

using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Move;

class TestCommClient : public CommClient<null_stream> {
  public:
    TestCommClient(CommServer & cs) : CommClient<null_stream>(cs, "") { }
};

class Connectiontest : public Cyphesis::TestBase
{
  private:
    ServerRouting * m_server;
    CommServer * m_commServer;
    CommClient<null_stream> * m_tcc;
    Connection * m_connection;

    static bool Router_error_called;
    static bool Router_clientError_called;
  public:
    Connectiontest();

    void setup();
    void teardown();

    void test_addNewAccount();
    void test_CreateOperation_empty();
    void test_CreateOperation_root_arg();
    void test_CreateOperation_restricted();
    void test_CreateOperation_empty_arg();
    void test_CreateOperation_account_by_id();
    void test_CreateOperation_number_username();
    void test_CreateOperation_no_passed();
    void test_CreateOperation_empty_password();
    void test_CreateOperation_username();
    void test_CreateOperation();
    void test_foo();
    void test_disconnectAccount_empty();
    void test_disconnectAccount_unused_Character();
    void test_disconnectAccount_used_Character();
    void test_disconnectAccount_others_used_Character();
    void test_disconnectAccount_unlinked_Character();
    void test_disconnectAccount_non_Character();

    static void set_Router_error_called();
    static void set_Router_clientError_called();
};

bool Connectiontest::Router_error_called = false;
bool Connectiontest::Router_clientError_called = false;

void Connectiontest::set_Router_error_called()
{
    Router_error_called = true;
}

void Connectiontest::set_Router_clientError_called()
{
    Router_clientError_called = true;
}

Connectiontest::Connectiontest()
{
    ADD_TEST(Connectiontest::test_addNewAccount);
    ADD_TEST(Connectiontest::test_CreateOperation_empty);
    ADD_TEST(Connectiontest::test_CreateOperation_root_arg);
    ADD_TEST(Connectiontest::test_CreateOperation_restricted);
    ADD_TEST(Connectiontest::test_CreateOperation_empty_arg);
    ADD_TEST(Connectiontest::test_CreateOperation_account_by_id);
    ADD_TEST(Connectiontest::test_CreateOperation_number_username);
    ADD_TEST(Connectiontest::test_CreateOperation_no_passed);
    ADD_TEST(Connectiontest::test_CreateOperation_empty_password);
    ADD_TEST(Connectiontest::test_CreateOperation_username);
    ADD_TEST(Connectiontest::test_CreateOperation);
    ADD_TEST(Connectiontest::test_foo);
    ADD_TEST(Connectiontest::test_disconnectAccount_empty);
    ADD_TEST(Connectiontest::test_disconnectAccount_empty);
    ADD_TEST(Connectiontest::test_disconnectAccount_unused_Character);
    ADD_TEST(Connectiontest::test_disconnectAccount_used_Character);
    ADD_TEST(Connectiontest::test_disconnectAccount_others_used_Character);
    ADD_TEST(Connectiontest::test_disconnectAccount_unlinked_Character);
    ADD_TEST(Connectiontest::test_disconnectAccount_non_Character);
}

void Connectiontest::setup()
{
    Router_error_called = false;

    m_server = new ServerRouting(*(BaseWorld*)0, "noruleset", "unittesting",
                                 "1", 1, "2", 2);
                         
    m_commServer = new CommServer;

    m_tcc = new TestCommClient(*m_commServer);
    m_connection = new Connection(*m_tcc, *m_server, "addr", "3", 3);
}

void Connectiontest::teardown()
{
}

void Connectiontest::test_addNewAccount()
{
    Account * ac = m_connection->addNewAccount("player", "bob", "foo");

    ASSERT_NOT_NULL(ac);

    m_connection->disconnectObject(m_connection->m_objects.find(ac->getIntId()),
                                   "test_event");

    ASSERT_EQUAL(m_connection->m_objects.size(), 0u);
}

void Connectiontest::test_CreateOperation_empty()
{
    Create op;
    OpVector res;
    m_connection->operation(op, res);
    ASSERT_EQUAL(m_connection->m_objects.size(), 0u);
    ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_root_arg()
{
    Create op;
    OpVector res;
    op->setArgs1(Root());
    m_connection->operation(op, res);
    ASSERT_EQUAL(m_connection->m_objects.size(), 0u);
    ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_restricted()
{
    Create op;
    OpVector res;
    restricted_flag = true;
    m_connection->operation(op, res);
    ASSERT_EQUAL(m_connection->m_objects.size(), 0u);
    ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_empty_arg()
{
    Create op;
    OpVector res;
    restricted_flag = false;
    Anonymous op_arg;
    op->setArgs1(op_arg);
    m_connection->operation(op, res);
    ASSERT_EQUAL(m_connection->m_objects.size(), 0u);
    ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_account_by_id()
{
    Create op;
    OpVector res;
    Anonymous op_arg;
    op->setArgs1(op_arg);
    op_arg->setId("jim");
    // Legacy op
    m_connection->operation(op, res);
    ASSERT_EQUAL(m_connection->m_objects.size(), 0u);
    ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_number_username()
{
    Create op;
    OpVector res;
    Anonymous op_arg;
    op->setArgs1(op_arg);
    op_arg->setAttr("username", 1);
    // Malformed username
    m_connection->operation(op, res);
    ASSERT_EQUAL(m_connection->m_objects.size(), 0u);
    ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_no_passed()
{
    Create op;
    OpVector res;
    Anonymous op_arg;
    op->setArgs1(op_arg);
    op_arg->setAttr("username", "jim");
    // username, no password
    m_connection->operation(op, res);
    ASSERT_EQUAL(m_connection->m_objects.size(), 0u);
    ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_empty_password()
{
    Create op;
    OpVector res;
    Anonymous op_arg;
    op->setArgs1(op_arg);
    op_arg->setAttr("username", "jim");
    op_arg->setAttr("password", "");
    // zero length password
    m_connection->operation(op, res);
    ASSERT_EQUAL(m_connection->m_objects.size(), 0u);
    ASSERT_TRUE(Router_clientError_called);
}

void Connectiontest::test_CreateOperation_username()
{
    Create op;
    OpVector res;
    Anonymous op_arg;
    op->setArgs1(op_arg);
    op_arg->setAttr("username", "");
    op_arg->setAttr("password", "foo");
    // zero length username
    m_connection->operation(op, res);
    ASSERT_EQUAL(m_connection->m_objects.size(), 0u);
    ASSERT_TRUE(Router_clientError_called);
}

void Connectiontest::test_CreateOperation()
{
    Create op;
    OpVector res;
    Anonymous op_arg;
    op->setArgs1(op_arg);
    op_arg->setAttr("username", "jim");
    op_arg->setAttr("password", "foo");
    // valid username and password
    m_connection->operation(op, res);
    ASSERT_EQUAL(m_connection->m_objects.size(), 1u);
}

void Connectiontest::test_foo()
{
    {
        Login op;
        OpVector res;
        m_connection->operation(op, res);
        op->setArgs1(Root());
        m_connection->operation(op, res);
        Anonymous op_arg;
        op->setArgs1(op_arg);
        m_connection->operation(op, res);
        op_arg->setId("bob");
        m_connection->operation(op, res);
        op_arg->setAttr("username", 1);
        m_connection->operation(op, res);
        op_arg->setAttr("username", "");
        m_connection->operation(op, res);
        op_arg->setAttr("username", "bob");
        m_connection->operation(op, res);
        op_arg->setAttr("password", "foo");
        m_connection->operation(op, res);
        m_connection->operation(op, res);
    }

    {
        Get op;
        OpVector res;
        m_connection->operation(op, res);
        Root op_arg;
        op->setArgs1(op_arg);
        m_connection->operation(op, res);
        op_arg->setId("1");
        m_connection->operation(op, res);
        op_arg->setId("game_entity");
        m_connection->operation(op, res);
    }

    {
        Logout op;
        OpVector res;
        m_connection->operation(op, res);
        op->setSerialno(24);
        m_connection->operation(op, res);
        Root op_arg;
        op->setArgs1(op_arg);
        m_connection->operation(op, res);
        op_arg->setId("-1");
        m_connection->operation(op, res);
        op_arg->setId("23");
        m_connection->operation(op, res);
        // How to determine the real ID?
        const RouterMap rm = m_connection->m_objects;
        RouterMap::const_iterator I = rm.begin();
        for (;I != rm.end(); ++I) {
            std::string object_id = String::compose("%1", I->first);
            std::cout << "ID: " << object_id << std::endl;
            op_arg->setId(object_id);
            m_connection->operation(op, res);
        }
    }

}

void Connectiontest::test_disconnectAccount_empty()
{
    // setup
    Account * ac = new Player(m_connection,
                              "jim",
                              "1e0ce8e9-304b-470c-83c4-feab11f9a2e4",
                              "4", 4);

    ac->m_connection = m_connection;
    m_connection->m_objects[ac->getIntId()] = ac;

    RouterMap::iterator I = m_connection-> m_objects.find(ac->getIntId());
    assert(I != m_connection->m_objects.end());

    m_connection->disconnectAccount(ac, I, "test_disconnect_account");

    ASSERT_TRUE(m_connection-> m_objects.find(ac->getIntId()) == 
                m_connection->m_objects.end());
}

void Connectiontest::test_disconnectAccount_unused_Character()
{
    // setup
    Account * ac = new Player(m_connection,
                              "jim",
                              "1e0ce8e9-304b-470c-83c4-feab11f9a2e4",
                              "4", 4);

    ac->m_connection = m_connection;
    m_connection->m_objects[ac->getIntId()] = ac;

    RouterMap::iterator I = m_connection-> m_objects.find(ac->getIntId());
    assert(I != m_connection->m_objects.end());

    Character * avatar = new Character("5", 5);
    m_connection->m_objects[avatar->getIntId()] = avatar;
    ac->addCharacter(avatar);

    m_connection->disconnectAccount(ac, I, "test_disconnect_account");

    ASSERT_TRUE(m_connection-> m_objects.find(ac->getIntId()) == 
                m_connection->m_objects.end());

    ASSERT_TRUE(m_connection-> m_objects.find(avatar->getIntId()) == 
                m_connection->m_objects.end());
}

void Connectiontest::test_disconnectAccount_used_Character()
{
    // setup
    Account * ac = new Player(m_connection,
                              "jim",
                              "1e0ce8e9-304b-470c-83c4-feab11f9a2e4",
                              "4", 4);

    ac->m_connection = m_connection;
    m_connection->m_objects[ac->getIntId()] = ac;

    RouterMap::iterator I = m_connection-> m_objects.find(ac->getIntId());
    assert(I != m_connection->m_objects.end());

    Character * avatar = new Character("5", 5);
    avatar->m_externalMind = new ExternalMind(*avatar);
    avatar->m_externalMind->linkUp(m_connection);
    m_connection->m_objects[avatar->getIntId()] = avatar;
    ac->addCharacter(avatar);

    m_connection->disconnectAccount(ac, I, "test_disconnect_account");

    ASSERT_TRUE(m_connection-> m_objects.find(ac->getIntId()) == 
                m_connection->m_objects.end());

    // The Character was in use, so it stays connected
    ASSERT_TRUE(m_connection-> m_objects.find(avatar->getIntId()) != 
                m_connection->m_objects.end());
}

void Connectiontest::test_disconnectAccount_others_used_Character()
{
    // setup
    Account * ac = new Player(m_connection,
                              "jim",
                              "1e0ce8e9-304b-470c-83c4-feab11f9a2e4",
                              "4", 4);

    ac->m_connection = m_connection;
    m_connection->m_objects[ac->getIntId()] = ac;

    RouterMap::iterator I = m_connection-> m_objects.find(ac->getIntId());
    assert(I != m_connection->m_objects.end());

    CommClient<null_stream> * otcc = new TestCommClient(*m_commServer);
    Connection * other_con = new Connection(*otcc, *m_server, "addr", "6", 6);

    Character * avatar = new Character("5", 5);
    avatar->m_externalMind = new ExternalMind(*avatar);
    avatar->m_externalMind->linkUp(other_con);
    m_connection->m_objects[avatar->getIntId()] = avatar;
    ac->addCharacter(avatar);

    m_connection->disconnectAccount(ac, I, "test_disconnect_account");

    ASSERT_TRUE(m_connection-> m_objects.find(ac->getIntId()) == 
                m_connection->m_objects.end());

    // The Character was in use by another connection, so it is removed
    // from this one.
    ASSERT_TRUE(m_connection-> m_objects.find(avatar->getIntId()) == 
                m_connection->m_objects.end());
}

void Connectiontest::test_disconnectAccount_unlinked_Character()
{
    // setup
    Account * ac = new Player(m_connection,
                              "jim",
                              "1e0ce8e9-304b-470c-83c4-feab11f9a2e4",
                              "4", 4);

    ac->m_connection = m_connection;
    m_connection->m_objects[ac->getIntId()] = ac;

    RouterMap::iterator I = m_connection-> m_objects.find(ac->getIntId());
    assert(I != m_connection->m_objects.end());

    Character * avatar = new Character("5", 5);
    avatar->m_externalMind = new ExternalMind(*avatar);
    m_connection->m_objects[avatar->getIntId()] = avatar;
    ac->addCharacter(avatar);

    m_connection->disconnectAccount(ac, I, "test_disconnect_account");

    ASSERT_TRUE(m_connection-> m_objects.find(ac->getIntId()) == 
                m_connection->m_objects.end());

    ASSERT_TRUE(m_connection-> m_objects.find(avatar->getIntId()) == 
                m_connection->m_objects.end());
}

void Connectiontest::test_disconnectAccount_non_Character()
{
    // setup
    Account * ac = new Player(m_connection,
                              "jim",
                              "1e0ce8e9-304b-470c-83c4-feab11f9a2e4",
                              "4", 4);

    ac->m_connection = m_connection;
    m_connection->m_objects[ac->getIntId()] = ac;

    RouterMap::iterator I = m_connection-> m_objects.find(ac->getIntId());
    assert(I != m_connection->m_objects.end());

    Entity * avatar = new Thing("5", 5);
    m_connection->m_objects[avatar->getIntId()] = avatar;
    ac->addCharacter(avatar);

    m_connection->disconnectAccount(ac, I, "test_disconnect_account");

    ASSERT_TRUE(m_connection-> m_objects.find(ac->getIntId()) == 
                m_connection->m_objects.end());

    ASSERT_TRUE(m_connection-> m_objects.find(avatar->getIntId()) == 
                m_connection->m_objects.end());
}

int main()
{
    Connectiontest t;

    return t.run();
}

// Stubs

#include "common/BaseWorld.h"

bool restricted_flag;

namespace Atlas { namespace Objects { namespace Operation {
int UPDATE_NO = -1;
} } }


CommServer::CommServer() : m_congested(false)
{
}

CommServer::~CommServer()
{
}

Idle::Idle(CommServer & svr) : m_idleManager(svr)
{
}

Idle::~Idle()
{
}

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
}

Player::Player(Connection * conn,
               const std::string & username,
               const std::string & passwd,
               const std::string & id,
               long intId) :
        Account(conn, username, passwd, id, intId)
{
}

Player::~Player() { }

const char * Player::getType() const
{
    return "player";
}

void Player::addToMessage(MapType & omap) const
{
}

void Player::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

int Player::characterError(const Operation & op,
                           const Root & ent, OpVector & res) const
{
    return 0;
}

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

const char * Account::getType() const
{
    return "testaccount";
}

// Simplified stub version to allow us to test Connection::disconnectObject
void Account::addCharacter(LocatedEntity * chr)
{
    m_charactersDict[chr->getIntId()] = chr;
}

void Account::store() const
{
}

void Account::addToMessage(Atlas::Message::MapType &) const
{
}

void Account::addToEntity(const Atlas::Objects::Entity::RootEntity &) const
{
}

void Account::createObject(const std::string & type_str,
                           const Root & arg,
                           const Operation & op,
                           OpVector & res)
{
}

void Account::externalOperation(const Operation &, Link &)
{
}

void Account::operation(const Operation &, OpVector &)
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

Lobby::Lobby(ServerRouting & s, const std::string & id, long intId) :
       Router(id, intId),
       m_server(s)
{
}

Lobby::~Lobby()
{
}

void Lobby::delAccount(Account * ac)
{
}

void Lobby::addToMessage(MapType & omap) const
{
}

void Lobby::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Lobby::addAccount(Account * ac)
{
}

void Lobby::externalOperation(const Operation &, Link &)
{
}

void Lobby::operation(const Operation & op, OpVector & res)
{
}

ExternalMind::ExternalMind(Entity & e) : Router(e.getId(), e.getIntId()),
                                         m_external(0), m_entity(e)
{
}

ExternalMind::~ExternalMind()
{
}

void ExternalMind::externalOperation(const Operation &, Link &)
{
}

void ExternalMind::operation(const Operation & op, OpVector & res)
{
}

ExternalProperty::ExternalProperty(ExternalMind * & data) : m_data(data)
{
}

int ExternalProperty::get(Atlas::Message::Element & val) const
{
    return 0;
}

void ExternalProperty::set(const Atlas::Message::Element & val)
{
}

void ExternalProperty::add(const std::string & s,
                         Atlas::Message::MapType & map) const
{
}

void ExternalProperty::add(const std::string & s,
                         const Atlas::Objects::Entity::RootEntity & ent) const
{
}

ExternalProperty * ExternalProperty::copy() const
{
    return 0;
}

const std::string & ExternalMind::connectionId()
{
    assert(m_external != 0);
    return m_external->getId();
}

void ExternalMind::linkUp(Link * c)
{
    m_external = c;
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

int Character::linkExternal(Link * link)
{
    return 0;
}

int Character::unlinkExternal(Link * link)
{
    return 0;
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

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return m_properties[name] = prop;
}

PropertyBase * Entity::modProperty(const std::string & name)
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

void Link::sendError(const Operation & op,
                     const std::string &,
                     const std::string &) const
{
}

void Link::disconnect()
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

void Router::error(const Operation & op,
                   const std::string & errstring,
                   OpVector & res,
                   const std::string & to) const
{
    Connectiontest::set_Router_error_called();
}

void Router::clientError(const Operation & op,
                         const std::string & errstring,
                         OpVector & res,
                         const std::string & to) const
{
    Connectiontest::set_Router_clientError_called();
}

Location::Location() : m_loc(0)
{
}

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(LocatedEntity *)
{
}

void PropertyBase::apply(LocatedEntity *)
{
}

void PropertyBase::add(const std::string & s,
                       Atlas::Message::MapType & ent) const
{
}

void PropertyBase::add(const std::string & s,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
}

HandlerResult PropertyBase::operation(LocatedEntity *,
                                      const Operation &,
                                      OpVector &)
{
    return OPERATION_IGNORED;
}

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
{
}

BaseWorld::~BaseWorld()
{
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    return 0;
}

LocatedEntity * BaseWorld::getEntity(long id) const
{
    return 0;
}

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance() : noClass(0)
{
}

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
    }
    return *m_instance;
}

const TypeNode * Inheritance::getType(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second;
}

const Root & Inheritance::getClass(const std::string & parent)
{
    return noClass;
}

void log(LogLevel lvl, const std::string & msg)
{
}
void hash_password(const std::string & pwd, const std::string & salt,
                   std::string & hash )
{
}
void logEvent(LogEvent lev, const std::string & msg)
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

static long idGenerator = 0;

long newId(std::string & id)
{
    static char buf[32];
    long new_id = ++idGenerator;
    sprintf(buf, "%ld", new_id);
    id = buf;
    assert(!id.empty());
    return new_id;
}

void addToEntity(const Vector3D & v, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = v[0];
    vd[1] = v[1];
    vd[2] = v[2];
}

int check_password(const std::string & pwd, const std::string & hash)
{
    return 0;
}

#include <common/Shaker.h>

Shaker::Shaker()
{
}
std::string Shaker::generateSalt(size_t length)
{
    return "";
}
