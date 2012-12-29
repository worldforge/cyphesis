// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "server/Lobby.h"

#include "server/CommClient.h"
#include "server/Connection.h"
#include "server/Account.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

class TestAccount : public Account
{
  public:
    TestAccount() : Account(0, "bob", "foo", "2", 2) { }

    virtual int characterError(const Operation &,
                               const Atlas::Objects::Root & ent,
                               OpVector & res) const
    {
        return false;
    }
};

class Lobbytest : public Cyphesis::TestBase
{
  private:
    Lobby * m_lobby;
  public:
    Lobbytest();

    void setup();
    void teardown();

    void test_constructor();
    void test_addAccount();
    void test_delAccount();
    void test_delAccount_empty();
    void test_operation();
    void test_addAccount_connected();
    void test_operation_connected();
    void test_operation_connected_other();
    void test_operation_account();
    void test_addToMessage();
    void test_addToEntity();
};

Lobbytest::Lobbytest()
{
    ADD_TEST(Lobbytest::test_constructor);
    ADD_TEST(Lobbytest::test_addAccount);
    ADD_TEST(Lobbytest::test_delAccount);
    ADD_TEST(Lobbytest::test_delAccount_empty);
    ADD_TEST(Lobbytest::test_operation);
    ADD_TEST(Lobbytest::test_addAccount_connected);
    ADD_TEST(Lobbytest::test_operation_connected);
    ADD_TEST(Lobbytest::test_operation_connected_other);
    ADD_TEST(Lobbytest::test_operation_account);
    ADD_TEST(Lobbytest::test_addToMessage);
    ADD_TEST(Lobbytest::test_addToEntity);
}

void Lobbytest::setup()
{
    m_lobby = new Lobby(*(ServerRouting*)0, "1", 1);
}

void Lobbytest::teardown()
{
    delete m_lobby;
}

void Lobbytest::test_constructor()
{
}

void Lobbytest::test_addAccount()
{
    assert(m_lobby->getAccounts().size() == 0);

    m_lobby->addAccount(new TestAccount());

    assert(m_lobby->getAccounts().size() == 1);
}

void Lobbytest::test_delAccount()
{
    assert(m_lobby->getAccounts().size() == 0);

    Account * tac = new TestAccount();

    m_lobby->addAccount(tac);

    assert(m_lobby->getAccounts().size() == 1);

    m_lobby->delAccount(tac);

    assert(m_lobby->getAccounts().size() == 0);
}

void Lobbytest::test_delAccount_empty()
{
    assert(m_lobby->getAccounts().size() == 0);

    m_lobby->delAccount(new TestAccount());

    assert(m_lobby->getAccounts().size() == 0);
}

void Lobbytest::test_operation()
{
    Atlas::Objects::Operation::RootOperation op;
    OpVector res;
    m_lobby->operation(op, res);
}

void Lobbytest::test_addAccount_connected()
{
    Account * tac = new TestAccount();

    tac->m_connection = new Connection(*(CommSocket*)0,
                                       *(ServerRouting*)0,
                                       "foo", "3", 3);

    m_lobby->addAccount(tac);

    Atlas::Objects::Operation::RootOperation op;
    OpVector res;
    m_lobby->operation(op, res);
}

void Lobbytest::test_operation_connected()
{
    Account * tac = new TestAccount();

    tac->m_connection = new Connection(*(CommSocket*)0,
                                       *(ServerRouting*)0,
                                       "foo", "3", 3);

    m_lobby->addAccount(tac);

    Atlas::Objects::Operation::RootOperation op;
    OpVector res;
    op->setTo("3");
    m_lobby->operation(op, res);
}

void Lobbytest::test_operation_connected_other()
{
    Account * tac = new TestAccount();

    tac->m_connection = new Connection(*(CommSocket*)0,
                                       *(ServerRouting*)0,
                                       "foo", "3", 3);

    m_lobby->addAccount(tac);

    Atlas::Objects::Operation::RootOperation op;
    OpVector res;
    op->setTo("2");
    m_lobby->operation(op, res);
}

void Lobbytest::test_operation_account()
{
    Account * tac = new TestAccount();

    m_lobby->addAccount(tac);

    Atlas::Objects::Operation::RootOperation op;
    OpVector res;
    op->setTo("2");
    m_lobby->operation(op, res);
}

void Lobbytest::test_addToMessage()
{
    m_lobby->addAccount(new TestAccount());

    Atlas::Message::MapType e;
    m_lobby->addToMessage(e);
}

void Lobbytest::test_addToEntity()
{
    m_lobby->addAccount(new TestAccount());

    Atlas::Objects::Entity::RootEntity e;
    m_lobby->addToEntity(e);
}


int main()
{
    Lobbytest t;

    return t.run();
}

// Stub functions

#include "common/log.h"

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

void Account::store() const
{
}

void Account::addToMessage(Atlas::Message::MapType &) const
{
}

void Account::addToEntity(const Atlas::Objects::Entity::RootEntity &) const
{
}


void Account::externalOperation(const Operation & op, Link &)
{
}

void Account::operation(const Operation &, OpVector &)
{
}

void Account::createObject(const std::string & type_str,
                           const Atlas::Objects::Root & arg,
                           const Operation & op,
                           OpVector & res)
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
}

void log(LogLevel lvl, const std::string & msg)
{
}
