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

#include "server/Account.h"
#include "server/CommServer.h"
#include "server/ServerRouting.h"
#include "server/CommClient.h"
#include "server/Connection.h"
#include "server/WorldRouter.h"

#include "rulesets/Entity.h"
#include "rulesets/Character.h"

#include "TestWorld.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Objects::Entity::Anonymous;

class TestCommClient : public CommClient {
  public:
    TestCommClient(CommServer & cs) : CommClient(cs) { }
};

class TestAccount : public Account {
  public:
    TestAccount(Connection * conn, const std::string & username,
                                   const std::string & passwd,
                                   const std::string & id, long intId) :
        Account(conn, username, passwd, id, intId) {
    }

    virtual int characterError(const Operation & op,
                               const Atlas::Objects::Entity::RootEntity & ent,
                               OpVector & res) const {
        return 0;
    }

    Entity * testAddNewCharacter(const std::string & typestr,
                                 const Atlas::Objects::Entity::RootEntity & ent)
    {
        return addNewCharacter(typestr, ent);
    }
};

int main()
{
    database_flag = false;

    WorldRouter world;
    Entity & e = world.m_gameWorld;

    ServerRouting server(world, "noruleset", "unittesting",
                         "1", 1, "2", 2);

    CommServer commServer(server);

    TestCommClient * tc = new TestCommClient(commServer);
    Connection * c = new Connection(*tc, server, "addr", "3");
    TestAccount * ac = new TestAccount(c, "user", "password", "4", 4);

    {
        Entity * chr = new Entity("5", 5);
        chr->m_location.m_loc = &e;
        chr->m_location.m_loc->makeContainer();
        assert(chr->m_location.m_loc->m_contains != 0);
        chr->m_location.m_loc->m_contains->insert(chr);

        ac->addCharacter(chr);

        chr->destroy();

    }

    {
        Entity * chr = new Character("6", 6);
        chr->m_location.m_loc = &e;
        chr->m_location.m_loc->makeContainer();
        assert(chr->m_location.m_loc->m_contains != 0);
        chr->m_location.m_loc->m_contains->insert(chr);

        ac->addCharacter(chr);

        chr->destroy();

    }

    {
        Anonymous new_char;
        Entity * chr = ac->testAddNewCharacter("game_entity", new_char);
    }


    delete ac;

    return 0;
}
