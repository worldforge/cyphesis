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

#include "../TestWorld.h"
#include "../TestBase.h"

#include "server/Account.h"
#include "server/Connection.h"
#include "server/Lobby.h"
#include "server/ServerRouting.h"

#include "rules/simulation/Entity.h"

#include "common/compose.hpp"
#include "../DatabaseNull.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <server/Persistence.h>

class CommSocket;

using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Imaginary;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Talk;

using String::compose;

static int test_send_count;

class TestAccount;
class AccountServerLobbyintegration : public Cyphesis::TestBase
{
  private:
    DatabaseNull m_database;
    Persistence* m_persistence;
    ServerRouting * m_server;
    TestAccount * m_account;
    long m_id_counter;
    std::unique_ptr<TestWorld> m_world;
  public:
    AccountServerLobbyintegration();

    void setup();
    void teardown();

    void test_talk();
    void test_emote();
    void test_lobby_look();
};

class TestAccount : public Account
{
  public:
    TestAccount(ServerRouting & svr, RouterId id);
    ~TestAccount();
    int characterError(const Operation & op,
                       const Atlas::Objects::Root & ent,
                       OpVector & res) const
    {
        return false;
    }

        void test_processExternalOperation(const Operation & op, OpVector& res) {
            processExternalOperation(op, res);
        }
                  
};

AccountServerLobbyintegration::AccountServerLobbyintegration() :
      m_server(0), m_id_counter(0L)
{
    ADD_TEST(AccountServerLobbyintegration::test_talk);
    ADD_TEST(AccountServerLobbyintegration::test_emote);
    ADD_TEST(AccountServerLobbyintegration::test_lobby_look);
}

void AccountServerLobbyintegration::setup()
{
    m_persistence = new Persistence(m_database);

    Ref<LocatedEntity> gw = new Entity(m_id_counter++);
    m_world.reset();
    m_world.reset(new TestWorld(gw));
    m_server = new ServerRouting(*m_world, *m_persistence,
                                 "59331d74-bb5d-4a54-b1c2-860999a4e344",
                                 "93e1f67f-63c5-4b07-af4c-574b2273563d",
                                 m_id_counter++,
                                 AssetsHandler({}));
    for (int i = 0; i < 3; ++i) {
        m_account = new TestAccount(*m_server,
                                    m_id_counter++);
        m_server->addAccount(std::unique_ptr<Account>(m_account));
        m_server->getLobby().addAccount(m_account);
    }
    ASSERT_NOT_NULL(m_account);
}

void AccountServerLobbyintegration::teardown()
{
    delete m_server;
    delete m_persistence;
}

void AccountServerLobbyintegration::test_talk()
{
    test_send_count = 0;

    Anonymous talk_arg;
    talk_arg->setAttr("say", "bb6a71d1-3ee9-43ac-8750-cd9d8f7921f6");

    Talk op;
    op->setArgs1(talk_arg);
    op->setFrom(m_account->getId());

    OpVector res;
    m_account->test_processExternalOperation(op, res);
    ASSERT_TRUE(res.empty());

    // Ensure the resulting broadcast sound was sent to all three accounts
    ASSERT_EQUAL(test_send_count, 3);
}

void AccountServerLobbyintegration::test_emote()
{
    test_send_count = 0;

    Anonymous emote_arg;
    emote_arg->setAttr("description", "c7ef270a-c4be-484c-a6a7-94efc4ff6ade");

    Imaginary op;
    op->setArgs1(emote_arg);
    op->setFrom(m_account->getId());

    OpVector res;
    m_account->test_processExternalOperation(op, res);
    ASSERT_TRUE(res.empty());

    // Ensure the resulting broadcast sound was sent to all three accounts
    ASSERT_EQUAL(test_send_count, 3);
}

void AccountServerLobbyintegration::test_lobby_look()
{
    test_send_count = 0;

    Look op;
    op->setFrom(m_account->getId());

    OpVector res;
    m_account->test_processExternalOperation(op, res);
    ASSERT_TRUE(!res.empty());
}

TestAccount::TestAccount(ServerRouting & svr, RouterId cid) :
          Account(new Connection(*(CommSocket*)0,
                                 svr,
                                 "7546215f-ac75-4e1a-a2c3-a9226219259b",
                                 cid),
                  "cec7a6f5-ebf1-4531-a0d9-ed9bb46882ad",
                  "59cf380e-7398-48a7-81cc-961265fadcd0",
                  cid)
{
}

TestAccount::~TestAccount()
{
    delete m_connection;
}

int main()
{
    AccountServerLobbyintegration test_case;

    return test_case.run();
}

// stubs


#include "server/Connection.h"
#include "server/Persistence.h"
#include "server/PossessionAuthenticator.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/id.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/Variable.h"

#include <cstdlib>
#include "../stubs/common/stubAssetsHandler.h"

std::string assets_directory("");

#include "../stubs/server/stubConnectableRouter.h"
#include "../stubs/server/stubPossessionAuthenticator.h"
#include "../stubs/server/stubPersistence.h"
#include "../stubs/rules/simulation/stubThing.h"
#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/common/stubVariable.h"
#include "../stubs/common/stubMonitors.h"
#include "../stubs/common/stubDatabase.h"
#include "../stubs/server/stubBuildid.h"

#include "../stubs/server/stubConnection.h"

#define STUB_Link_send
void Link::send(const Operation & op) const
{
    ++test_send_count;
}

void Link::send(const OpVector& opVector) const
{
}

#include "../stubs/common/stubLink.h"

#include "../stubs/rules/simulation/stubBaseWorld.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/rules/simulation/stubExternalMind.h"
#include "../stubs/rules/simulation/stubMindsProperty.h"
#include "../stubs/common/stubProperty.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/common/stubcustom.h"
#include "../stubs/common/stubid.h"
#include "../stubs/common/stublog.h"


bool database_flag = false;
std::string instance("130779df-1e84-4c61-9caf-3e1506597fe1");

#include "../stubs/common/stubconst.h"


const char * const CYPHESIS = "cyphesis";
int timeoffset = 0;

bool_config_register::bool_config_register(bool & var,
                                           const char * section,
                                           const char * setting,
                                           const char * help)
{
}

#include <common/Shaker.h>

Shaker::Shaker()
{
}

std::string Shaker::generateSalt(size_t length)
{
    return "";
}
