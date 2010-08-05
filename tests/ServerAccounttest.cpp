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

#include "server/ServerAccount.h"
#include "server/CommServer.h"
#include "server/ServerRouting.h"
#include "server/CommClient.h"
#include "server/Connection.h"
#include "server/WorldRouter.h"
#include "server/Ruleset.h"
#include "server/Player.h"

#include "rulesets/Python_API.h"
#include "rulesets/Entity.h"
#include "rulesets/Character.h"
#include "rulesets/World.h"

#include "common/Monitor.h"
#include "common/Connect.h"

#include "TestWorld.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <cassert>

using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Connect;

class TestCommClient : public CommClient {
  public:
    TestCommClient(CommServer & cs) : CommClient(cs) { }
};

class TestServerAccount : public ServerAccount {
  public:
    TestServerAccount(Connection * conn, const std::string & username,
                                   const std::string & passwd,
                                   const std::string & id, long intId) :
        ServerAccount(conn, username, passwd, id, intId) {
    }

    virtual int testCharacterError(const Operation & op,
                               const Atlas::Objects::Entity::RootEntity & ent,
                               OpVector & res) const {
        return characterError(op, ent, res);
    }

    Entity * testAddNewCharacter(const std::string & typestr,
                                 const Atlas::Objects::Entity::RootEntity & ent,
                                 const Atlas::Objects::Entity::RootEntity & arg)
    {
        return addNewCharacter(typestr, ent, arg);
    }
};

void run_operation_checks(TestServerAccount * ac, Entity * chr, WorldRouter & world)
{
    // Entity injection test
    {
        Entity * ent = new Entity("1", 1);
        
        ent->m_location.m_loc = new World("0", 0);
        ent->m_location.m_loc->makeContainer();
        assert(ent->m_location.m_loc->m_contains != 0);
        ent->m_location.m_loc->m_contains->insert(ent);

        // Add the test attributes
        ent->setAttr("name", "test_entity");
        ent->setAttr("test_int", 1);
        ent->setAttr("test_float", 1.f);
        ent->setAttr("test_list_string", "test_value");
        ent->setAttr("test_list_int", ListType(1, 1));
        ent->setAttr("test_list_float", ListType(1, 1.f));
        ent->setAttr("test_map_string", ListType(1, "test_value"));
        MapType test_map;
        test_map["test_key"] = 1;
        ent->setAttr("test_map_int", test_map);
        test_map["test_key"] = 1.f;
        ent->setAttr("test_map_float", test_map);
        test_map["test_key"] = "test_value";
        ent->setAttr("test_map_string", test_map);
        
        Atlas::Objects::Entity::Anonymous atlas_repr;
        ent->addToEntity(atlas_repr);
        
        Create op;
        OpVector res;
        op->setArgs1(atlas_repr);
        ac->operation(op, res);
        
        Entity *reply = world.findByName("test_entity");
        assert(reply != 0);
        
        Atlas::Message::Element val;
        // Check the integer attribute
        assert(reply->getAttr("test_int", val));
        assert(val.isInt());
        assert(val.Int() == 1);
        // Check the float attribute
        assert(reply->getAttr("test_float", val));
        assert(val.isFloat());
        assert(val.Float() == 1.f);
        // Check the string attribute
        assert(reply->getAttr("test_list_string", val));
        assert(val.isString());
        assert(val.String() == "test_value");
        // Check the integer list attribute
        assert(ent->getAttr("test_list_int", val));
        assert(val == ListType(1, 1));
        // Check the float list attribute
        assert(ent->getAttr("test_list_float", val));
        assert(val == ListType(1, 1.f));
        // Check the string map attribute
        assert(ent->getAttr("test_map_string", val));
        assert(val == ListType(1, "test_value"));
        
        MapType reply_map;
        // Check the integer map value
        reply_map["test_key"] = 1;
        assert(ent->getAttr("test_map_int", val));
        assert(val == reply_map);
        // Check the float map value
        test_map["test_key"] = 1.f;
        assert(ent->getAttr("test_map_float", val));
        assert(val == reply_map);
        // Check the string map value
        test_map["test_key"] = "test_value";
        assert(ent->getAttr("test_map_string", val));
        assert(val == reply_map);
    }
    
    // Regular create op tests
    {
        // This is the only op we've overridden
        Create op;
        OpVector res;
        ac->operation(op, res);
        op->setArgs1(Root());
        ac->operation(op, res);
        Anonymous op_arg;
        op->setArgs1(op_arg);
        ac->operation(op, res);
        op_arg->setParents(std::list<std::string>());
        ac->operation(op, res);
        op_arg->setParents(std::list<std::string>(1, "game_entity"));
        ac->operation(op, res);
        op_arg->setObjtype("obj");
        ac->operation(op, res);
        op_arg->setName("Bob");
        ac->operation(op, res);
        op_arg->setObjtype("class");
        ac->operation(op, res);
        op_arg->setId("game_entity");
        ac->operation(op, res);
        op_arg->setId("new_class");
        ac->operation(op, res);
        op_arg->setParents(std::list<std::string>(1, ""));
        ac->operation(op, res);
        op_arg->setParents(std::list<std::string>(1, "non_exist"));
        ac->operation(op, res);
    }
}

int main()
{
    database_flag = false;

    init_python_api();

    WorldRouter world;
    Entity & e = world.m_gameWorld;

    Ruleset::init();

    ServerRouting server(world, "noruleset", "unittesting",
                         "1", 1, "2", 2);


    CommServer commServer(server);

    TestCommClient * tc = new TestCommClient(commServer);
    Connection * c = new Connection(*tc, server, "addr", "3");
    TestServerAccount * ac = new TestServerAccount(c, "user", "password", "4", 4);
    Entity * chr;

    Player * p = new Player(0, "bob", "bobspass", "8", 8);
    server.addAccount(p);

    {
        chr = new Entity("5", 5);
        chr->m_location.m_loc = &e;
        chr->m_location.m_loc->makeContainer();
        assert(chr->m_location.m_loc->m_contains != 0);
        chr->m_location.m_loc->m_contains->insert(chr);

        ac->addCharacter(chr);

        chr->destroy();

    }

    {
        chr = new Character("6", 6);
        chr->m_location.m_loc = &e;
        chr->m_location.m_loc->makeContainer();
        assert(chr->m_location.m_loc->m_contains != 0);
        chr->m_location.m_loc->m_contains->insert(chr);

        ac->addCharacter(chr);

        chr->destroy();

    }

    {
        chr = new Character("7", 7);
        chr->m_location.m_loc = &e;
        chr->m_location.m_loc->makeContainer();
        assert(chr->m_location.m_loc->m_contains != 0);
        chr->m_location.m_loc->m_contains->insert(chr);

        ac->addCharacter(chr);
    }

    {
        Anonymous new_char;
        ac->testAddNewCharacter("game_entity", new_char, RootEntity());
    }

    ac->getType();

    {
        MapType emap;
        ac->addToMessage(emap);
    }

    {
        RootEntity ent;
        ac->addToEntity(ent);
    }

    {
        Create op;
        Anonymous ent;
        OpVector res;

        ac->testCharacterError(op, ent, res);
        ent->setParents(std::list<std::string>(1, "game_entity"));
        ac->testCharacterError(op, ent, res);
        ent->setParents(std::list<std::string>());
        ac->testCharacterError(op, ent, res);
    }

    run_operation_checks(ac, chr, world);

    ac->m_connection = 0;

    run_operation_checks(ac, chr, world);

    delete ac;

    shutdown_python_api();
    return 0;
}
