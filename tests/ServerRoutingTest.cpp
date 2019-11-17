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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/ServerRouting.h"

#include "server/Account.h"

#include "rules/simulation/BaseWorld.h"
#include "common/id.h"
#include "common/log.h"
#include "DatabaseNull.h"

#include <Atlas/Objects/Anonymous.h>

#include <iostream>

#include <cassert>
#include <server/Persistence.h>

class Entity;

static bool stub_deny_newid = false;
static bool stub_generate_accounts = false;

#include "TestWorld.h"

class TestRouter : public ConnectableRouter
{
    public:
        TestRouter(const std::string& id, int iid) : ConnectableRouter(id, iid)
        {}

        void externalOperation(const Operation&, Link&) override
        {}

        void operation(const Operation&, OpVector&) override
        {}

        void setConnection(Connection* connection) override
        {}

        Connection* getConnection() const override
        {
            return nullptr;
        }
};

class TestAccount : public Account
{
    public:
        TestAccount(Connection* conn, const std::string& username,
                    const std::string& passwd,
                    const std::string& id, long intId) :
            Account(conn, username, passwd, id, intId)
        {
        }

        virtual int characterError(const Operation& op,
                                   const Atlas::Objects::Root& ent,
                                   OpVector& res) const
        {
            return 0;
        }
};

int main()
{
    TestWorld world;

    std::string ruleset = "test_rules";
    std::string server_name = "test_svr";
    std::string server_id, lobby_id;
    long int_id, lobby_int_id;

    if (((int_id = newId(server_id)) < 0) ||
        ((lobby_int_id = newId(lobby_id)) < 0)) {
        std::cerr << "Unable to get server IDs newid";
        return 1;
    }

    {
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);
    }

    {
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id;
        int iid = newId(id);
        assert(iid >= 0);

        server.addObject(std::make_unique<TestRouter>(id, iid));
        assert(server.getObjects().size() == 1);
    }

    {
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id;
        int iid = newId(id);
        assert(iid >= 0);

        auto r = new TestRouter(id, iid);
        server.addObject(std::unique_ptr<TestRouter>(r));
        assert(server.getObjects().size() == 1);
        server.delObject(r);
        assert(server.getObjects().size() == 0);
    }

    {
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id, id2;
        int iid = newId(id);
        assert(iid >= 0);

        newId(id2);

        ConnectableRouter* r = new TestRouter(id, iid);
        server.addObject(std::unique_ptr<ConnectableRouter>(r));
        assert(server.getObjects().size() == 1);

        ConnectableRouter* r2 = server.getObject(id);
        assert(r == r2);

        r2 = server.getObject(id2);
        assert(0 == r2);
    }

    {
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id;
        int iid = newId(id);
        assert(iid >= 0);

        server.addAccount(std::make_unique<TestAccount>(nullptr, "bob", "", id, iid));
        assert(server.getObjects().size() == 1);
    }

    {
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id;
        int iid = newId(id);
        assert(iid >= 0);

        Account* ac = new TestAccount(0, "bob", "", id, iid);
        server.addAccount(std::unique_ptr<Account>(ac));;
        assert(server.getObjects().size() == 1);
        Account* rac = server.getAccountByName("bob");
        assert(rac == ac);
    }

    {
        DatabaseNull database;
        Persistence persistence(database);

        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id;
        int iid = newId(id);
        assert(iid >= 0);

        Account* ac = new TestAccount(0, "bob", "", id, iid);
        server.addAccount(std::unique_ptr<Account>(ac));;
        assert(server.getObjects().size() == 1);
        Account* rac = server.getAccountByName("alice");
        assert(rac == 0);
    }

    {
        DatabaseNull database;
        Persistence persistence(database);
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id;
        int iid = newId(id);
        assert(iid >= 0);

        Account* rac = server.getAccountByName("alice");
        assert(rac == 0);
    }

    {
        stub_generate_accounts = true;
        DatabaseNull database;
        Persistence persistence(database);
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id;
        int iid = newId(id);
        assert(iid >= 0);

        Account* rac = server.getAccountByName("alice");
        assert(rac != 0);
    }

    {
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        Atlas::Message::MapType map;
        server.addToMessage(map);
        restricted_flag = true;
        server.addToMessage(map);
        restricted_flag = false;
    }

    {
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        Atlas::Objects::Entity::Anonymous ent;
        server.addToEntity(ent);
        restricted_flag = true;
        server.addToEntity(ent);
        restricted_flag = false;
    }


    return 0;
}

// stubs

#include "server/Lobby.h"
#include "server/Persistence.h"

#include "common/const.h"
#include "common/Monitors.h"
#include "common/Variable.h"

#include <cstdio>
#include <cstdlib>
#include "stubs/common/stubDatabase.h"
#include "stubs/server/stubAccount.h"
#include "stubs/server/stubConnectableRouter.h"
#include "stubs/rules/simulation/stubExternalMind.h"
#include "stubs/common/stubLink.h"

#define STUB_Persistence_getAccount

Account* Persistence::getAccount(const std::string& name)
{
    if (!stub_generate_accounts) {
        return 0;
    }

    std::string id;
    int iid = newId(id);
    assert(iid >= 0);

    return new TestAccount(0, name, "", id, iid);
}

#include "stubs/server/stubPersistence.h"
#include "stubs/common/stubTypeNode.h"
Lobby::Lobby(ServerRouting& s, const std::string& id, long intId) :
    Router(id, intId),
    m_server(s)
{
}

Lobby::~Lobby()
{
}

void Lobby::externalOperation(const Operation&, Link&)
{
}

void Lobby::operation(const Operation& op, OpVector& res)
{
}

void Lobby::addToMessage(Atlas::Message::MapType& omap) const
{
}

void Lobby::addToEntity(const Atlas::Objects::Entity::RootEntity& ent) const
{
}

#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"
#include "stubs/server/stubBuildid.h"

bool_config_register::bool_config_register(bool& var,
                                           const char* section,
                                           const char* setting,
                                           const char* help)
{
}

#include "stubs/common/stubRouter.h"

#ifndef STUB_BaseWorld_getEntity
#define STUB_BaseWorld_getEntity

Ref<LocatedEntity> BaseWorld::getEntity(const std::string& id) const
{
    return getEntity(integerId(id));
}

Ref<LocatedEntity> BaseWorld::getEntity(long id) const
{
    auto I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second);
        return I->second;
    } else {
        return nullptr;
    }
}

#endif //STUB_BaseWorld_getEntity

#include "stubs/rules/simulation/stubBaseWorld.h"


static long idGenerator = 0;

long newId(std::string& id)
{
    if (stub_deny_newid) {
        return -1;
    }
    static char buf[32];
    long new_id = ++idGenerator;
    sprintf(buf, "%ld", new_id);
    id = buf;
    assert(!id.empty());
    return new_id;
}

long integerId(const std::string& id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

const char* const CYPHESIS = "cyphesis";

static const char* DEFAULT_INSTANCE = "cyphesis";

std::string instance(DEFAULT_INSTANCE);
int timeoffset = 0;
bool database_flag = false;
std::string assets_directory = "";

namespace consts {
    const char* version = "test_version";
}

#include <common/Shaker.h>

Shaker::Shaker()
{
}

std::string Shaker::generateSalt(size_t length)
{
    return "";
}
