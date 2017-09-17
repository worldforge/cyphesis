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

#include "common/BaseWorld.h"
#include "common/id.h"
#include "common/log.h"

#include <Atlas/Objects/Anonymous.h>

#include <iostream>

#include <cassert>

class Entity;

static bool stub_deny_newid = false;
static bool stub_generate_accounts = false;

class TestWorld : public BaseWorld {
  public:
    explicit TestWorld() : BaseWorld(*(LocatedEntity*)0) {
    }

    virtual bool idle() { return false; }
    virtual LocatedEntity * addEntity(LocatedEntity * ent) { 
        return 0;
    }
    virtual LocatedEntity * addNewEntity(const std::string &,
                                         const Atlas::Objects::Entity::RootEntity &) {
        return 0;
    }
    void delEntity(LocatedEntity * obj) {}
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         LocatedEntity *) { return 0; }
    int removeSpawnPoint(LocatedEntity *) {return 0; }
    int getSpawnList(Atlas::Message::ListType & data) { return 0; }
    LocatedEntity * spawnNewEntity(const std::string & name,
                                   const std::string & type,
                                   const Atlas::Objects::Entity::RootEntity & desc) {
        return addNewEntity(type, desc);
    }
    virtual int moveToSpawn(const std::string & name,
                            Location& location){return 0;}
    virtual Task * newTask(const std::string &, LocatedEntity &) { return 0; }
    virtual Task * activateTask(const std::string &, const std::string &,
                                LocatedEntity *, LocatedEntity &) { return 0; }
    virtual ArithmeticScript * newArithmetic(const std::string &,
                                             LocatedEntity *) {
        return 0;
    }
    virtual void message(const Operation & op, LocatedEntity & ent) { }
    virtual LocatedEntity * findByName(const std::string & name) { return 0; }
    virtual LocatedEntity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(LocatedEntity *) { }
};

class TestRouter : public Router
{
  public:
    TestRouter(const std::string &id, int iid) : Router(id, iid) { }

    void externalOperation(const Operation &, Link &) override { }
    void operation(const Operation &, OpVector &) override { }
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

        server.addObject(new TestRouter(id, iid));
        assert(server.getObjects().size() == 1);
    }

    {
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id;
        int iid = newId(id);
        assert(iid >= 0);

        Router * r = new TestRouter(id, iid);
        server.addObject(r);
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

        Router * r = new TestRouter(id, iid);
        server.addObject(r);
        assert(server.getObjects().size() == 1);

        Router * r2 = server.getObject(id);
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

        server.addAccount(new TestAccount(0, "bob", "", id, iid));
        assert(server.getObjects().size() == 1);
    }

    {
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id;
        int iid = newId(id);
        assert(iid >= 0);

        Account * ac = new TestAccount(0, "bob", "", id, iid);
        server.addAccount(ac);;
        assert(server.getObjects().size() == 1);
        Account * rac = server.getAccountByName("bob");
        assert(rac == ac);
    }

    {
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id;
        int iid = newId(id);
        assert(iid >= 0);

        Account * ac = new TestAccount(0, "bob", "", id, iid);
        server.addAccount(ac);;
        assert(server.getObjects().size() == 1);
        Account * rac = server.getAccountByName("alice");
        assert(rac == 0);
    }

    {
        database_flag = true;
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id;
        int iid = newId(id);
        assert(iid >= 0);

        Account * rac = server.getAccountByName("alice");
        assert(rac == 0);
        database_flag = false;
    }

    {
        database_flag = true;
        stub_generate_accounts = true;
        ServerRouting server(world, ruleset, server_name,
                             server_id, int_id,
                             lobby_id, lobby_int_id);

        std::string id;
        int iid = newId(id);
        assert(iid >= 0);

        Account * rac = server.getAccountByName("alice");
        assert(rac != 0);
        database_flag = false;
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
    return "test_account";
}

void Account::store() const
{
}

bool Account::isPersisted() const {
    return true;
}

void Account::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Account::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Account::createObject(const std::string & type_str,
                           const Atlas::Objects::Root & arg,
                           const Operation & op,
                           OpVector & res)
{
}

LocatedEntity * Account::createCharacterEntity(const std::string &,
                                const Atlas::Objects::Entity::RootEntity &,
                                const Atlas::Objects::Root &)
{
    return 0;
}

void Account::externalOperation(const Operation &, Link &)
{
}

void Account::operation(const Operation & op, OpVector & res)
{
}

void Account::LogoutOperation(const Operation & op, OpVector & res)
{
}

void Account::CreateOperation(const Operation & op, OpVector & res)
{
}

void Account::SetOperation(const Operation & op, OpVector & res)
{
}

void Account::ImaginaryOperation(const Operation & op, OpVector & res)
{
}

void Account::TalkOperation(const Operation & op, OpVector & res)
{
}

void Account::LookOperation(const Operation & op, OpVector & res)
{
}

void Account::GetOperation(const Operation & op, OpVector & res)
{
}

void Account::OtherOperation(const Operation & op, OpVector & res)
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

Account * Persistence::getAccount(const std::string & name)
{
    if (!stub_generate_accounts) {
        return 0;
    }

    std::string id;
    int iid = newId(id);
    assert(iid >= 0);

    return new TestAccount(0, name, "", id, iid);
}

void Persistence::registerCharacters(Account & ac,
                                     const EntityDict & worldObjects)
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

void Lobby::externalOperation(const Operation &, Link &)
{
}

void Lobby::operation(const Operation & op, OpVector & res)
{
}

void Lobby::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Lobby::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"
#include "stubs/server/stubBuildid.h"

bool_config_register::bool_config_register(bool & var,
                                           const char * section,
                                           const char * setting,
                                           const char * help)
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

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
{
}

BaseWorld::~BaseWorld()
{
}

double BaseWorld::getTime() const
{
    return .0;
}

static long idGenerator = 0;

long newId(std::string & id)
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

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

const char * const CYPHESIS = "cyphesis";

static const char * DEFAULT_INSTANCE = "cyphesis";

std::string instance(DEFAULT_INSTANCE);
int timeoffset = 0;
bool database_flag = false;

namespace consts {
  const char * version = "test_version";
}

#include <common/Shaker.h>

Shaker::Shaker()
{
}

std::string Shaker::generateSalt(size_t length)
{
    return "";
}
