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

#include "../TestBase.h"

#include "server/Account.h"
#include "server/EntityBuilder.h"
#include "server/EntityRuleHandler.h"
#include "server/Ruleset.h"
#include "server/ServerRouting.h"
#include "server/Connection.h"
#include "rules/simulation/WorldRouter.h"

#include "rules/Domain.h"
#include "rules/simulation/Entity.h"
#include "rules/simulation/ExternalMind.h"
#include "rules/simulation/MindsProperty.h"

#include "common/CommSocket.h"
#include "common/Inheritance.h"

#include "../TestWorld.h"
#include "../DatabaseNull.h"
#include "../TestPropertyManager.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <cassert>
#include <server/Persistence.h>
#include <common/Monitors.h>
#include <server/ExternalMindsManager.h>
#include <server/PossessionAuthenticator.h>

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

Atlas::Objects::Factories factories;


class TestCommSocket : public CommSocket
{
    public:
        TestCommSocket() : CommSocket(*(boost::asio::io_context*) 0)
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

class TestAccount : public Account
{
    public:
        TestAccount(Connection* conn, const std::string& username,
                    const std::string& passwd,
                    RouterId id) :
                Account(conn, username, passwd, std::move(id))
        {
        }

        virtual int characterError(const Operation& op,
                                   const Atlas::Objects::Root& ent,
                                   OpVector& res) const
        {
            return 0;
        }
};

class Accountintegration : public Cyphesis::TestBase
{
        DatabaseNull m_database;
        Persistence* m_persistence;

        WorldRouter* m_world;

        ServerRouting* m_server;

        CommSocket* m_tc;
        Connection* m_c;
        TestAccount* m_ac;
        EntityRuleHandler* m_entityRuleHandler;

    public:
        Accountintegration();

        void setup();

        void teardown();

        void test_addNewCharacter();

        void test_getType();

        void test_addToMessage();

        void test_addToEntity();

        void test_CreateOperation();

        void test_GetOperation();

        void test_ImaginaryOperation();

        void test_LookOperation();

        void test_SetOperation();

        void test_TalkOperation();

        void test_LogoutOperation();

        void test_connectCharacter_entity();

        void test_connectCharacter_character();

        Inheritance* m_inheritance;
        Ref<Entity> m_rootEntity;
        EntityBuilder* m_eb;
        PropertyManager* m_propertyManager;
};

Accountintegration::Accountintegration()
{
    ADD_TEST(Accountintegration::test_addNewCharacter);
    ADD_TEST(Accountintegration::test_getType);
    ADD_TEST(Accountintegration::test_addToMessage);
    ADD_TEST(Accountintegration::test_addToEntity);
    ADD_TEST(Accountintegration::test_CreateOperation);
    ADD_TEST(Accountintegration::test_GetOperation);
    ADD_TEST(Accountintegration::test_ImaginaryOperation);
    ADD_TEST(Accountintegration::test_LookOperation);
    ADD_TEST(Accountintegration::test_SetOperation);
    ADD_TEST(Accountintegration::test_TalkOperation);
    ADD_TEST(Accountintegration::test_LogoutOperation);
    ADD_TEST(Accountintegration::test_connectCharacter_entity);
    ADD_TEST(Accountintegration::test_connectCharacter_character);
}

Atlas::Objects::Root composeDeclaration(std::string class_name, std::string parent, Atlas::Message::MapType rawAttributes)
{

    Atlas::Objects::Root decl;
    decl->setObjtype("class");
    decl->setId(class_name);
    decl->setParent(parent);

    Atlas::Message::MapType composed;
    for (const auto& entry : rawAttributes) {
        composed[entry.first] = Atlas::Message::MapType{
                {"default", entry.second}
        };
    }

    decl->setAttr("attributes", composed);
    return decl;
};


void Accountintegration::setup()
{
    m_rootEntity = new Entity(0);
    m_persistence = new Persistence(m_database);
    m_inheritance = new Inheritance(factories);
    m_eb = new EntityBuilder();
    m_propertyManager = new TestPropertyManager();
    m_entityRuleHandler = new EntityRuleHandler(*m_eb, *m_propertyManager);

    m_world = new WorldRouter(m_rootEntity, *m_eb, []() { return std::chrono::milliseconds(0); });

    m_server = new ServerRouting(*m_world, *m_persistence, "noruleset", "unittesting",
                                 2, AssetsHandler({}));

    m_tc = new TestCommSocket();
    m_c = new Connection(*m_tc, *m_server, "addr", 3);
    m_ac = new TestAccount(m_c, "user", "password", 4);

    std::string dependent, reason;

    {
        auto decl = composeDeclaration("thing", "game_entity", {});
        std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;
        m_entityRuleHandler->install(decl->getId(), decl->getParent(), decl, dependent, reason, changes);
    }
}

void Accountintegration::teardown()
{
    delete m_entityRuleHandler;
    delete m_c;
    delete m_server;
    delete m_ac;
    delete m_tc;
    m_world->shutdown();
    delete m_world;
    m_rootEntity = nullptr;
    delete m_eb;
    delete m_inheritance;
    delete m_persistence;
    delete m_propertyManager;
}

void Accountintegration::test_addNewCharacter()
{
//    WorldRouter::instance().createSpawnPoint(Atlas::Message::MapType{
//        {"name",     "foo"},
//        {"entities", Atlas::Message::MapType{{"thing", Atlas::Message::MapType{}}}}
//    }, m_rootEntity.get());
//
//    OpVector res;
//    Anonymous new_char;
//    RootEntity args;
//    args->setAttr("spawn_name", "foo");
//    args->setParent("thing");
//    auto chr = m_ac->addNewCharacter(new_char, args, res);
//    assert(chr);
//
//    std::cout << "Test 1" << std::endl << std::flush;
}

void Accountintegration::test_getType()
{
    m_ac->getType();
}

void Accountintegration::test_addToMessage()
{
    MapType emap;
    m_ac->addToMessage(emap);
}

void Accountintegration::test_addToEntity()
{
    RootEntity ent;
    m_ac->addToEntity(ent);
}

void Accountintegration::test_CreateOperation()
{
    Anonymous op_arg;
    op_arg->setParent("game_entity");
    op_arg->setName("Bob");

    Create op;
    op->setArgs1(op_arg);

    OpVector res;
    m_ac->operation(op, res);
}

void Accountintegration::test_GetOperation()
{
    Anonymous op_arg;
    op_arg->setParent("");

    Get op;
    op->setArgs1(op_arg);

    OpVector res;
    m_ac->operation(op, res);
}

void Accountintegration::test_ImaginaryOperation()
{
    Anonymous op_arg;
    op_arg->setLoc("2");

    Imaginary op;
    op->setArgs1(op_arg);
    op->setSerialno(1);

    OpVector res;
    m_ac->operation(op, res);

    // FIXME Test response is sent to Lobby
}

void Accountintegration::test_LookOperation()
{
//    WorldRouter::instance().createSpawnPoint(Atlas::Message::MapType{
//        {"name",     "foo"},
//        {"entities", Atlas::Message::MapType{{"thing", Atlas::Message::MapType{}}}}
//    }, m_rootEntity.get());
//
//    OpVector res;
//    Anonymous new_char;
//    RootEntity args;
//    args->setAttr("spawn_name", "foo");
//    args->setParent("thing");
//    auto chr = m_ac->addNewCharacter(new_char, args, res);
//
//    Anonymous op_arg;
//    op_arg->setId("1");
//    op_arg->setId(chr->getId());
//
//    Look op;
//    op->setArgs1(op_arg);
//
//    m_ac->operation(op, res);
//
//    // FIXME This doesn't test a lot
}

void Accountintegration::test_SetOperation()
{
//    WorldRouter::instance().createSpawnPoint(Atlas::Message::MapType{
//        {"name",     "foo"},
//        {"entities", Atlas::Message::MapType{{"thing", Atlas::Message::MapType{}}}}
//    }, m_rootEntity.get());
//
//    OpVector res;
//    Anonymous new_char;
//    RootEntity args;
//    args->setAttr("spawn_name", "foo");
//    args->setParent("thing");
//    auto chr = m_ac->addNewCharacter(new_char, args, res);
//
//    BBox newBox(WFMath::Point<3>(-0.5, 0.0, -0.5),
//                WFMath::Point<3>(-0.5, 2.0, -0.5));
//    chr->m_location.setBBox(newBox);
//
//    Anonymous op_arg;
//
//    op_arg->setId(chr->getId());
//    op_arg->setAttr("guise", "foo");
//    op_arg->setAttr("height", 3.0);
//    op_arg->setAttr("tasks", ListType());
//
//    Set op;
//    op->setArgs1(op_arg);
//
//    m_ac->operation(op, res);
//
//    // FIXME Ensure character has been modified
}

void Accountintegration::test_TalkOperation()
{
    Anonymous op_arg;
    op_arg->setParent("");
    op_arg->setLoc("1");

    Talk op;
    op->setSerialno(1);
    op->setArgs1(op_arg);

    OpVector res;
    m_ac->operation(op, res);

    // FIXME Sound op should have been sent to the lobby
}

void Accountintegration::test_LogoutOperation()
{
    Logout op;
    op->setSerialno(1);

    Anonymous op_arg;
    op_arg->setParent("");
    op->setArgs1(op_arg);

    OpVector res;
    m_ac->operation(op, res);

    // FIXME Account should have been removed from Lobby, and also from
    // Connection
}

void Accountintegration::test_connectCharacter_entity()
{
    OpVector res;
    Ref<Entity> e = new Entity(7);

    int ret = m_ac->connectCharacter(e.get(), res);
    ASSERT_EQUAL(ret, 0);
}

void Accountintegration::test_connectCharacter_character()
{
    Ref<Entity> e = new Entity(8);
    OpVector res;
    int ret = m_ac->connectCharacter(e.get(), res);
    ASSERT_EQUAL(ret, 0);
    ASSERT_NOT_NULL(e->getPropertyClassFixed<MindsProperty>());
    ASSERT_FALSE(e->getPropertyClassFixed<MindsProperty>()->getMinds().empty());
}

int main()
{
    PossessionAuthenticator possesionAuthenticator;
    ExternalMindsManager externalMindsManager(possesionAuthenticator);
    Monitors m;
    Accountintegration t;

    return t.run();
}



// stubs

#include "server/EntityFactory.h"
#include "server/ArchetypeFactory.h"
#include "server/Juncture.h"
#include "server/Persistence.h"
#include "server/Player.h"
#include "server/Ruleset.h"
#include "server/TeleportProperty.h"

#include "rules/simulation/AreaProperty.h"
#include "rules/AtlasProperties.h"
#include "rules/BBoxProperty.h"
#include "rules/simulation/CalendarProperty.h"
#include "rules/simulation/EntityProperty.h"
#include "rules/simulation/LineProperty.h"
#include "server/MindProperty.h"
#include "rules/SolidProperty.h"
#include "rules/simulation/SpawnProperty.h"
#include "rules/simulation/StatusProperty.h"
#include "rules/simulation/TasksProperty.h"
#include "rules/simulation/TerrainModProperty.h"
#include "rules/simulation/TerrainProperty.h"
#include "rules/simulation/TransientProperty.h"
#include "rules/simulation/VisibilityProperty.h"
#include "rules/simulation/SuspendedProperty.h"
#include "rules/simulation/DomainProperty.h"
#include "../stubs/rules/stubScaleProperty.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/PropertyFactory.h"
#include "common/system.h"
#include "common/TypeNode.h"
#include "common/Variable.h"


#include "rules/python/PythonScriptFactory.h"
#include "../stubs/common/stubAssetsHandler.h"

