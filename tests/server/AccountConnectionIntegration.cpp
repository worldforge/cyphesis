// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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
#include "../TestWorld.h"

#include "server/Connection.h"
#include "server/Lobby.h"
#include "server/Player.h"
#include "server/ServerAccount.h"
#include "server/ServerRouting.h"
#include "server/SystemAccount.h"

#include "common/id.h"
#include "common/TypeNode.h"
#include "common/CommSocket.h"
#include "../DatabaseNull.h"
#include "rules/simulation/Entity.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <cassert>
#include <server/Persistence.h>
#include "rules/AtlasProperties.h"
#include "rules/PhysicalProperties.h"

using Atlas::Message::Element;
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
using Atlas::Objects::smart_dynamic_cast;

class SpawningTestWorld : public TestWorld {
  public:
    SpawningTestWorld(Ref<Entity> gw) : TestWorld(gw) { }

    void addEntity(const Ref<LocatedEntity>& ent, const Ref<LocatedEntity>& parent) override{
        ent->m_parent = parent.get();
        ent->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(0,0,0);
        m_eobjects[ent->getIntId()] = ent;
    }

    Ref<LocatedEntity> addNewEntity(const std::string & t,
                                  const Atlas::Objects::Entity::RootEntity &) override {
        auto id = newId();

        Ref<Entity>  e = new Entity(id);

        e->setType(new TypeNode(t));
        addEntity(e, m_gw);
        return e;
    }
};

class TestCommSocket : public CommSocket
{
  public:
    TestCommSocket() : CommSocket(*(boost::asio::io_context*)0)
    {
    }

    virtual void disconnect()
    {
    }

    virtual int flush()
    {
        return 0;
    }

};

class AccountConnectionintegration : public Cyphesis::TestBase {
  protected:
    DatabaseNull m_database;
    Persistence* m_persistence;
    Ref<Entity> m_tlve;
    BaseWorld * m_world;
    ServerRouting * m_server;
    Connection * m_connection;

    TestCommSocket m_commSocket;
  public:
    AccountConnectionintegration();

    void setup();
    void teardown();

    void test_account_creation();

    Connection * connection() const { return m_connection; }
    ServerRouting * server() const { return m_server; }
};

AccountConnectionintegration::AccountConnectionintegration()
{
    ADD_TEST(AccountConnectionintegration::test_account_creation);
}

void AccountConnectionintegration::setup()
{
    m_persistence = new Persistence(m_database);
    m_tlve = new Entity(0);
    m_world = new SpawningTestWorld(m_tlve);
    m_server = new ServerRouting(*m_world,
                                 *m_persistence,
                                 "testrules",
                                 "testname",
                                 2);
    m_connection = new Connection(m_commSocket,
                                  *m_server,
                                  "test_addr",
                                  3);
}

void AccountConnectionintegration::teardown()
{
    m_tlve = nullptr;

    delete m_world;
    delete m_connection;
    delete m_persistence;
    delete m_server;
}

static OpVector test_sent_ops;

void AccountConnectionintegration::test_account_creation()
{
    // Basic player account creation
    {

        ASSERT_NOT_NULL(m_connection);
        ASSERT_TRUE(m_connection->objects().empty());

        Create op;
        Anonymous create_arg;
        create_arg->setParent("player");
        create_arg->setAttr("username", "39d409ec");
        create_arg->setAttr("password", "6a6e71bab281");
        op->setArgs1(create_arg);

        ASSERT_TRUE(test_sent_ops.empty());

        // Send the operation to create the account
        m_connection->externalOperation(op, *m_connection);
        m_connection->dispatch(1);

        // There should be a response op
        ASSERT_TRUE(!test_sent_ops.empty());
        ASSERT_EQUAL(test_sent_ops.size(), 1u);
        // and the account creation should have created an object bound
        // to this connection.
        ASSERT_TRUE(!m_connection->objects().empty());

        // Check the response is an info indicating successful account
        // creation.
        const Operation & reply = test_sent_ops.front();
        ASSERT_EQUAL(reply->getClassNo(), Atlas::Objects::Operation::INFO_NO);
        // The Info response should have an argument describing the created
        // account
        const std::vector<Root> & reply_args = reply->getArgs();
        ASSERT_TRUE(!reply_args.empty());
        RootEntity account = smart_dynamic_cast<RootEntity>(reply_args.front());
        ASSERT_TRUE(account.isValid());

        // The account ID should be provided
        ASSERT_TRUE(!account->isDefaultId());
        const std::string account_id = account->getId();
        ASSERT_TRUE(!account_id.empty());

        // Check the account has been registered in the server object
        Router * account_router_ptr = m_server->getObject(account_id);
        ASSERT_NOT_NULL(account_router_ptr);

        // Check the account has been logged into the lobby
        const auto & lobby_dict = m_server->getLobby().getAccounts();
        auto I = lobby_dict.find(account_id);
        ASSERT_TRUE(I != lobby_dict.end());
        auto account_ptr = I->second;
        ASSERT_EQUAL(account_router_ptr, account_ptr);

        // Basic login as now been established by account creation

        // Set up some other account details
        create_arg->setAttr("username", "89cae312");
        create_arg->setAttr("password", "d730b8bd2d6c");

        // and try an additional account creation, which should fail.
        // Multiple logins are ok, but there is no reason to allow multiple
        // account creations.
        test_sent_ops.clear();
        m_connection->externalOperation(op, *m_connection);
        m_connection->dispatch(1);
        ASSERT_TRUE(!test_sent_ops.empty());
        ASSERT_EQUAL(test_sent_ops.size(), 1u);

        const Operation & error_reply = test_sent_ops.front();
        ASSERT_EQUAL(error_reply->getClassNo(),
                     Atlas::Objects::Operation::ERROR_NO);


        // TODO Character creation etc?
        // TODO Lobby interaction?
        // TODO Logout ?
    }
}

int main()
{
    AccountConnectionintegration t;

    return t.run();
}

// stubs



#include "rules/simulation/ExternalMind.h"
#include "server/Juncture.h"
#include "server/Persistence.h"
#include "server/Ruleset.h"
#include "server/PossessionAuthenticator.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/PropertyManager.h"
#include "common/Variable.h"

#include <cstdlib>
#include "../stubs/server/stubBuildid.h"
#include "../stubs/common/stubconst.h"

// globals - why do we have these again?

const char * const CYPHESIS = "cyphesisAccountConnectionintegration";
int timeoffset = 0;
std::string instance(CYPHESIS);

int CommSocket::flush()
{
    return 0;
}


#define STUB_ExternalMind_connectionId
const std::string & ExternalMind::connectionId()
{
    assert(m_link != 0);
    return m_link->getId();
}

#define STUB_ExternalMind_linkUp
void ExternalMind::linkUp(Link * c)
{
    m_link = c;
}

#include "../stubs/rules/simulation/stubExternalMind.h"
#include "../stubs/rules/simulation/stubMindsProperty.h"
#include "../stubs/rules/simulation/stubAdminMind.h"

#include "../stubs/server/stubJuncture.h"

#define STUB_Link_send
void Link::send(const OpVector& opVector) const
{
    for (const auto& op : opVector) {
        test_sent_ops.push_back(op);
    }
}

void Link::send(const Operation & op) const
{
    test_sent_ops.push_back(op);
}


#include "../stubs/common/stubLink.h"
#include "server/PropertyRuleHandler.h"
#include "server/ArchetypeRuleHandler.h"
#include "server/EntityRuleHandler.h"
#include "server/OpRuleHandler.h"
#include "../stubs/server/stubRuleset.h"
#include "../stubs/common/stubDatabase.h"
#include "../stubs/server/stubPossessionAuthenticator.h"
#include "../stubs/server/stubPersistence.h"
#include "../stubs/common/stublog.h"
#include "../stubs/rules/simulation/stubThing.h"
#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/common/stubVariable.h"
#include "../stubs/common/stubMonitors.h"
#include "../stubs/server/stubExternalMindsManager.h"
#include "../stubs/server/stubExternalMindsConnection.h"
#include "../stubs/common/stubOperationsDispatcher.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/common/stubPropertyManager.h"
#include "../stubs/common/stubid.h"
#include "../stubs/rules/stubPhysicalProperties.h"

#include <cstdio>

void encrypt_password(const std::string & pwd, std::string & hash)
{
}

int check_password(const std::string & pwd, const std::string & hash)
{
    return 0;
}

bool_config_register::bool_config_register(bool & var,
                                           const char * section,
                                           const char * setting,
                                           const char * help)
{
}
void hash_password(const std::string & pwd, const std::string & salt,
                   std::string & hash)
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
std::string assets_directory = "";
