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

#include "server/Connection.h"
#include "server/CommServer.h"
#include "server/ServerRouting.h"
#include "server/CommClient.h"
#include "server/WorldRouter.h"
#include "server/Account.h"

#include <cassert>

class TestCommClient : public CommClient {
  public:
    TestCommClient(CommServer & cs) : CommClient(cs) { }
};

class TestConnection : public Connection {
  public:
    TestConnection(CommClient & cc, ServerRouting & svr,
                   const std::string & addr, const std::string & id) :
        Connection(cc, svr, addr, id) {
      
    }

    Account * testAddPlayer(const std::string & username,
                            const std::string & password) {
        return addPlayer(username, password);
    }

    Account * testRemovePlayer(Router * obj) {
        return removePlayer(obj, "test_event");
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

    TestCommClient * tcc = new TestCommClient(commServer);
    TestConnection * tc = new TestConnection(*tcc, server, "addr", "3");

    Account * ac = tc->testAddPlayer("bob", "jim");
    assert(ac != 0);

    ac = tc->testRemovePlayer(ac);
    assert(ac != 0);

    delete tc;
}
