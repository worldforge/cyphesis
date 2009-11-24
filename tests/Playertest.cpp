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

#include "server/Player.h"
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
#include <Atlas/Objects/Operation.h>

#include <cassert>

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

class TestCommClient : public CommClient {
  public:
    TestCommClient(CommServer & cs) : CommClient(cs) { }
};

class TestPlayer : public Player {
  public:
    TestPlayer(Connection * conn, const std::string & username,
                                   const std::string & passwd,
                                   const std::string & id, long intId) :
        Player(conn, username, passwd, id, intId) {
    }

    virtual int testCharacterError(const Operation & op,
                               const Atlas::Objects::Entity::RootEntity & ent,
                               OpVector & res) const {
        return characterError(op, ent, res);
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
    TestPlayer * ac = new TestPlayer(c, "user", "password", "4", 4);
    Entity * chr;

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
        Entity * chr = ac->testAddNewCharacter("game_entity", new_char);
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

    ac->playableTypes.insert("game_entity");

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
        op_arg->setName("Bob");
        ac->operation(op, res);
        op_arg->setParents(std::list<std::string>(1, "non_entity"));
        ac->operation(op, res);
        op_arg->setName("admin");
        ac->operation(op, res);
    }

    {
        Get op;
        OpVector res;
        ac->operation(op, res);
        op->setArgs1(Root());
        ac->operation(op, res);
        Anonymous op_arg;
        op->setArgs1(op_arg);
        ac->operation(op, res);
        op_arg->setParents(std::list<std::string>());
        ac->operation(op, res);
    }

    {
        Imaginary op;
        OpVector res;
        ac->operation(op, res);
        op->setArgs1(Root());
        ac->operation(op, res);
        op->setSerialno(1);
        ac->operation(op, res);
        Anonymous op_arg;
        op->setArgs1(op_arg);
        ac->operation(op, res);
        op_arg->setLoc("2");
        ac->operation(op, res);
    }

    {
        Look op;
        OpVector res;
        ac->operation(op, res);
        op->setArgs1(Root());
        ac->operation(op, res);
        Anonymous op_arg;
        op->setArgs1(op_arg);
        ac->operation(op, res);
        op_arg->setId("1");
        ac->operation(op, res);
        op_arg->setId(chr->getId());
        ac->operation(op, res);
    }

    {
        Set op;
        OpVector res;
        ac->operation(op, res);
        op->setArgs1(Root());
        ac->operation(op, res);
        Anonymous op_arg;
        op->setArgs1(op_arg);
        ac->operation(op, res);
        op_arg->setId("1");
        ac->operation(op, res);
        op_arg->setId(chr->getId());
        ac->operation(op, res);
        op_arg->setAttr("guise", "foo");
        ac->operation(op, res);
        op_arg->setAttr("height", 3.0);
        ac->operation(op, res);
        BBox newBox(WFMath::Point<3>(-0.5, -0.5, 0.0),
                    WFMath::Point<3>(-0.5, -0.5, 2.0));
        chr->m_location.setBBox(newBox);
        op_arg->setAttr("height", 3.0);
        ac->operation(op, res);
        op_arg->setAttr("tasks", ListType());
        ac->operation(op, res);
    }

    {
        Talk op;
        OpVector res;
        ac->operation(op, res);
        op->setArgs1(Root());
        ac->operation(op, res);
        Anonymous op_arg;
        op->setArgs1(op_arg);
        ac->operation(op, res);
        op_arg->setParents(std::list<std::string>());
        ac->operation(op, res);
        op->setSerialno(1);
        ac->operation(op, res);
        op_arg->setLoc("1");
        ac->operation(op, res);
    }

    {
        Logout op;
        OpVector res;
        ac->operation(op, res);
        op->setSerialno(1);
        ac->operation(op, res);
        op->setArgs1(Root());
        ac->operation(op, res);
        Anonymous op_arg;
        op->setArgs1(op_arg);
        ac->operation(op, res);
        op_arg->setParents(std::list<std::string>());
        ac->operation(op, res);
    }

    {
        // Move has no meaning
        Move op;
        OpVector res;
        ac->operation(op, res);
        op->setArgs1(Root());
        ac->operation(op, res);
        Anonymous op_arg;
        op->setArgs1(op_arg);
        ac->operation(op, res);
        op_arg->setParents(std::list<std::string>());
        ac->operation(op, res);
    }

    delete ac;

    return 0;
}
