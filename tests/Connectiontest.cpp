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

#include "common/compose.hpp"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

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

    Account * testAddAccount(const std::string & username,
                            const std::string & password) {
        return addAccount("player", username, password);
    }

    Account * testRemoveAccount(Router * obj) {
        return removeAccount(obj, "test_event");
    }

    size_t numObjects() const {
        return m_objects.size();
    }

    const RouterMap & getObjects() const {
        return m_objects;
    }

    void removeObject(Router * obj) {
        RouterMap::iterator I = m_objects.find(obj->getIntId());
        if (I != m_objects.end()) {
            m_objects.erase(I);
        }
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

    Account * ac = tc->testAddAccount("bob", "foo");
    assert(ac != 0);

    ac = tc->testRemoveAccount(ac);
    assert(ac != 0);
    tc->removeObject(ac);

    assert(tc->numObjects() == 0);

    {
        Create op;
        OpVector res;
        tc->operation(op, res);
        op->setArgs1(Root());
        tc->operation(op, res);
        restricted_flag = true;
        tc->operation(op, res);
        restricted_flag = false;
        Anonymous op_arg;
        op->setArgs1(op_arg);
        tc->operation(op, res);
        op_arg->setId("jim");
        // Legacy op
        tc->operation(op, res);
        op_arg->setAttr("username", 1);
        // Malformed username
        tc->operation(op, res);
        op_arg->setAttr("username", "jim");
        // username, no password
        tc->operation(op, res);
        op_arg->setAttr("password", "");
        // zero length password
        tc->operation(op, res);
        op_arg->setAttr("username", "");
        op_arg->setAttr("password", "foo");
        // zero length username
        tc->operation(op, res);
        op_arg->setAttr("username", "jim");
        // valid username and password
        tc->operation(op, res);
        assert(tc->numObjects() != 0);
    }

    {
        Login op;
        OpVector res;
        tc->operation(op, res);
        op->setArgs1(Root());
        tc->operation(op, res);
        Anonymous op_arg;
        op->setArgs1(op_arg);
        tc->operation(op, res);
        op_arg->setId("bob");
        tc->operation(op, res);
        op_arg->setAttr("username", 1);
        tc->operation(op, res);
        op_arg->setAttr("username", "");
        tc->operation(op, res);
        op_arg->setAttr("username", "bob");
        tc->operation(op, res);
        op_arg->setAttr("password", "foo");
        tc->operation(op, res);
        tc->operation(op, res);
    }

    {
        Get op;
        OpVector res;
        tc->operation(op, res);
        Root op_arg;
        op->setArgs1(op_arg);
        tc->operation(op, res);
        op_arg->setId("1");
        tc->operation(op, res);
        op_arg->setId("game_entity");
        tc->operation(op, res);
    }

    {
        Logout op;
        OpVector res;
        tc->operation(op, res);
        op->setSerialno(24);
        tc->operation(op, res);
        Root op_arg;
        op->setArgs1(op_arg);
        tc->operation(op, res);
        op_arg->setId("-1");
        tc->operation(op, res);
        op_arg->setId("23");
        tc->operation(op, res);
        // How to determine the real ID?
        const RouterMap & rm = tc->getObjects();
        RouterMap::const_iterator I = rm.begin();
        for (;I != rm.end(); ++I) {
            std::string object_id = String::compose("%1", I->first);
            std::cout << "ID: " << object_id << std::endl;
            op_arg->setId(object_id);
            tc->operation(op, res);
        }
    }

    delete tc;
}
