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

#include "server/SystemAccount.h"

#include "server/Connection.h"
#include "server/ServerRouting.h"

#include "rulesets/Entity.h"

#include "common/CommSocket.h"
#include "common/compose.hpp"

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

class SystemAccounttest : public Cyphesis::TestBase
{
  protected:
    long m_id_counter;

    ServerRouting * m_server;
    Connection * m_connection;
    Account * m_account;
    TestWorld* m_world;
  public:
    SystemAccounttest();

    void setup();
    void teardown();

    void test_getType();
    void test_store();
};

SystemAccounttest::SystemAccounttest() : m_id_counter(0L),
                                         m_server(0),
                                         m_connection(0),
                                         m_account(0)
{
    ADD_TEST(SystemAccounttest::test_getType);
    ADD_TEST(SystemAccounttest::test_store);
}

void SystemAccounttest::setup()
{
    Entity * gw = new Entity(compose("%1", m_id_counter),
                             m_id_counter++);
    m_world = new TestWorld(*gw);
    m_server = new ServerRouting(*m_world,
                                 "5529d7a4-0158-4dc1-b4a5-b5f260cac635",
                                 "bad621d4-616d-4faf-b9e6-471d12b139a9",
                                 compose("%1", m_id_counter), m_id_counter++,
                                 compose("%1", m_id_counter), m_id_counter++);
    m_connection = new Connection(*(CommSocket*)0, *m_server,
                                  "8d18a4e8-f14f-4a46-997e-ada120d5438f",
                                  compose("%1", m_id_counter), m_id_counter++);
    m_account = new SystemAccount(m_connection,
                                  "6c9f3236-5de7-4ba4-8b7a-b0222df0af38",
                                  "fa1a03a2-a745-4033-85cb-bb694e921e62",
                                  compose("%1", m_id_counter), m_id_counter++);
}

void SystemAccounttest::teardown()
{
    delete m_world;
    delete m_server;
    delete m_account;
    delete m_connection;
}

void SystemAccounttest::test_getType()
{
    const char * type = m_account->getType();

    ASSERT_EQUAL(std::string("sys"), type);
}

void SystemAccounttest::test_store()
{
    m_account->store();
}

int main()
{
    SystemAccounttest t;

    return t.run();
}

// stubs

#include "server/Connection.h"
#include "server/Persistence.h"
#include "server/PossessionAuthenticator.h"

#include "rulesets/Character.h"

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


LocatedEntity * Account::addNewCharacter(const std::string & typestr,
                                  const RootEntity & ent,
                                  const Root & arg)
{
    return 0;
}

LocatedEntity * Account::createCharacterEntity(const std::string &,
                                const Atlas::Objects::Entity::RootEntity &,
                                const Atlas::Objects::Root &)
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

bool Account::isPersisted() const {
    return true;
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

Admin::Admin(Connection * conn,
             const std::string & username,
             const std::string & passwd,
             const std::string & id,
             long intId) :
       Account(conn, username, passwd, id, intId)
{
}

Admin::~Admin()
{
    if (m_monitorConnection.connected()) {
        m_monitorConnection.disconnect();
    }
}

const char * Admin::getType() const
{
    return "admin";
}

void Admin::addToMessage(MapType & omap) const
{
}

void Admin::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

int Admin::characterError(const Operation & op,
                          const Root & ent, OpVector & res) const
{
    return 0;
}

void Admin::createObject(const std::string & type_str,
                           const Root & arg,
                           const Operation & op,
                           OpVector & res)
{
}

LocatedEntity * Admin::createCharacterEntity(const std::string &,
                                const Atlas::Objects::Entity::RootEntity &,
                                const Atlas::Objects::Root &)
{
    return 0;
}


void Admin::LogoutOperation(const Operation & op, OpVector & res)
{
}

void Admin::GetOperation(const Operation & op, OpVector & res)
{
}

void Admin::SetOperation(const Operation & op, OpVector & res)
{
}

void Admin::OtherOperation(const Operation & op, OpVector & res)
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

#include "stubs/server/stubServerRouting.h"
#include "stubs/server/stubPossessionAuthenticator.h"
#include "stubs/server/stubPersistence.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubThing.h"
#include "stubs/rulesets/stubCharacter.h"
#include "stubs/common/stubLink.h"
#include "stubs/rulesets/stubBaseWorld.h"
#include "stubs/common/stubRouter.h"
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
