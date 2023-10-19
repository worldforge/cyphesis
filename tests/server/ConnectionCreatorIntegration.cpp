#include <memory>

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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
#include "server/ServerRouting.h"

#include "rules/simulation/ExternalMind.h"

#include "common/const.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <Atlas/Objects/Operation.h>

#include <cstdio>
#include <cstring>

#include <cassert>
#include <rules/simulation/Entity.h>
#include <common/TypeNode.h>
#include <rules/simulation/AdminMind.h>
#include <rules/simulation/MindsProperty.h>
#include <common/Monitors.h>
#include <common/operations/Think.h>
#include <common/CommSocket.h>

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Encoder.h>


using Atlas::Objects::Operation::RootOperation;
using String::compose;
Atlas::Objects::Factories factories;

class StubSocket : public CommSocket
{
    public:
        explicit StubSocket(boost::asio::io_context& io_context) : CommSocket(io_context)
        {
        }

        void disconnect() override
        {
        }

        int flush() override
        {
            return 0;
        }

};


struct TestDecoder : public Atlas::Objects::ObjectsDecoder
{
    Atlas::Objects::Root m_obj;

    TestDecoder(const Atlas::Objects::Factories& factories) :
            ObjectsDecoder(factories)
    {}

    void objectArrived(Atlas::Objects::Root obj) override
    {
        m_obj = obj;
    }
};

class ConnectionCreatorintegration : public Cyphesis::TestBase
{
    protected:
        long m_id_counter;

        StubSocket* m_commSocket;
        ServerRouting* m_server;
        Connection* m_connection;
        Ref<Entity> m_gw;
        Ref<Entity> m_creator;
        TypeNode* m_creatorType;
        std::unique_ptr<TestWorld> m_world;
    public:

        static Operation m_BaseWorld_message_called;
        static Ref<LocatedEntity> m_BaseWorld_message_called_from;

        ConnectionCreatorintegration();

        void setup();

        void teardown();

        void test_external_op();

        void test_external_op_override();

        void test_external_op_puppet();

        void test_external_op_puppet_nonexistant();

        static void BaseWorld_message_called(const Operation& op, LocatedEntity&);
};

Operation ConnectionCreatorintegration::m_BaseWorld_message_called(0);
Ref<LocatedEntity> ConnectionCreatorintegration::m_BaseWorld_message_called_from(0);

void ConnectionCreatorintegration::BaseWorld_message_called(
        const Operation& op,
        LocatedEntity& ent)
{
    m_BaseWorld_message_called = op;
    m_BaseWorld_message_called_from = &ent;
}

ConnectionCreatorintegration::ConnectionCreatorintegration() :
        m_id_counter(0L),
        m_connection(0),
        m_creator(0),
        m_creatorType(0)
{

    new Monitors();

    ADD_TEST(ConnectionCreatorintegration::test_external_op);
    ADD_TEST(ConnectionCreatorintegration::test_external_op_override);
    ADD_TEST(ConnectionCreatorintegration::test_external_op_puppet);
    ADD_TEST(ConnectionCreatorintegration::test_external_op_puppet_nonexistant);
}

void ConnectionCreatorintegration::setup()
{
    boost::asio::io_context io_context;

    m_gw = new Entity(m_id_counter++);
    m_world = std::make_unique<TestWorld>(m_gw);
    TestWorld::extension.messageFn = [](const Operation& op, LocatedEntity& ent) {
        ConnectionCreatorintegration::BaseWorld_message_called(op, ent);
    };
    m_server = new ServerRouting(*m_world,
                                 *(Persistence*)nullptr,
                                 "dd7452be-0137-4664-b90e-77dfb395ac39",
                                 "a2feda8e-62e9-4ba0-95c4-09f92eda6a78",
                                 m_id_counter++,
                                 AssetsHandler({}));
    m_commSocket = new StubSocket(io_context);
    m_connection = new Connection(*m_commSocket,
                                  *m_server,
                                  "25251955-7e8c-4043-8a5e-adfb8a1e76f7",
                                  m_id_counter++);
    m_creator = new Entity(m_id_counter++);
    m_creatorType = new TypeNode("test_avatar");
    m_creator->setType(m_creatorType);
    m_world->addEntity(m_creator, m_gw);

    m_connection->addRouter(m_creator.get());

}

void ConnectionCreatorintegration::teardown()
{
    m_BaseWorld_message_called = nullptr;
    m_BaseWorld_message_called_from = nullptr;

    m_creator = nullptr;

    m_gw.reset();
    m_world->shutdown();
    m_world.reset();

    delete m_connection;
    m_creator = nullptr;
    delete m_creatorType;
    delete m_server;
    delete m_commSocket;
}

void ConnectionCreatorintegration::test_external_op()
{
    // Dispatching a Talk external op from the creator should result in
    // it being passed on to the world, exactly as if this was a Character
    // except that we assume that Creator was set up linked.

    AdminMind mind(1, m_creator);
    m_connection->addRouter(&mind);
    mind.linkUp(m_connection);
    m_creator->requirePropertyClassFixed<MindsProperty>().addMind(&mind);

    Atlas::Objects::Operation::Talk op;
    op->setFrom(mind.getId());

    m_connection->externalOperation(op, *m_connection);
    m_connection->dispatch(1);

    // BaseWorld::message should have been called from Enitty::sendWorld
    // with the Talk operation, modified to have TO set to the character.
    ASSERT_TRUE(m_BaseWorld_message_called.isValid());
    ASSERT_EQUAL(m_BaseWorld_message_called->getClassNo(),
                 Atlas::Objects::Operation::TALK_NO);
    ASSERT_TRUE(!m_BaseWorld_message_called->isDefaultTo());
    ASSERT_EQUAL(m_BaseWorld_message_called->getTo(), m_creator->getId());
    ASSERT_TRUE(m_BaseWorld_message_called_from);
    ASSERT_EQUAL(m_BaseWorld_message_called_from.get(), m_creator.get());
}

void ConnectionCreatorintegration::test_external_op_override()
{
    // Dispatching a Talk external op from the creator should result in
    // it being passed on to the world, exactly as if this was a Character
    // except that we assume that Creator was set up linked.

    AdminMind mind(1, m_creator);
    m_connection->addRouter(&mind);
    mind.linkUp(m_connection);
    m_creator->requirePropertyClassFixed<MindsProperty>().addMind(&mind);

    Atlas::Objects::Operation::Talk op;
    op->setFrom(mind.getId());
    op->setTo(m_creator->getId());

    m_connection->externalOperation(op, *m_connection);
    m_connection->dispatch(1);

    // The operation should have been passed to Entity::callOperation for
    // dispatch, completely unfiltered.
    ASSERT_TRUE(m_BaseWorld_message_called.isValid());
    ASSERT_EQUAL(m_BaseWorld_message_called->getClassNo(),
                 Atlas::Objects::Operation::TALK_NO);
    ASSERT_TRUE(!m_BaseWorld_message_called->isDefaultTo());
    ASSERT_EQUAL(m_BaseWorld_message_called->getTo(), m_creator->getId());
}

void ConnectionCreatorintegration::test_external_op_puppet()
{
    // Dispatching a Talk external op from the creator, to the creator should
    // result in it being passed directly to the normal op dispatch,
    // shortcutting the world.

    AdminMind mind(1, m_creator);
    m_connection->addRouter(&mind);
    mind.linkUp(m_connection);
    m_creator->requirePropertyClassFixed<MindsProperty>().addMind(&mind);

    Ref<Entity> other = new Entity(m_id_counter++);
    other->setType(m_creatorType);
    m_server->m_world.addEntity(other, m_gw);

    Atlas::Objects::Operation::Talk op;
    op->setFrom(mind.getId());
    op->setTo(other->getId());

    m_connection->externalOperation(op, *m_connection);
    m_connection->dispatch(1);

    // Operation should be via world dispatch, as if it was from the Entity
    // we are puppeting.
    ASSERT_TRUE(m_BaseWorld_message_called.isValid());
    ASSERT_EQUAL(m_BaseWorld_message_called->getClassNo(),
                 Atlas::Objects::Operation::TALK_NO);
    ASSERT_TRUE(!m_BaseWorld_message_called->isDefaultTo());
    ASSERT_EQUAL(m_BaseWorld_message_called->getTo(), other->getId());
    ASSERT_TRUE(m_BaseWorld_message_called_from);
    ASSERT_EQUAL(m_BaseWorld_message_called_from.get(), m_creator.get());
}

void ConnectionCreatorintegration::test_external_op_puppet_nonexistant()
{
    // Dispatching a Talk external op from the creator, to the creator should
    // result in it being passed directly to the normal op dispatch,
    // shortcutting the world.

    AdminMind mind(1, m_creator);
    m_connection->addRouter(&mind);
    mind.linkUp(m_connection);
    m_creator->requirePropertyClassFixed<MindsProperty>().addMind(&mind);

    Ref<Entity> other = new Entity(m_id_counter++);
    other->setType(m_creatorType);
    m_server->m_world.addEntity(other, m_gw);

    Atlas::Objects::Operation::Talk op;
    op->setFrom(mind.getId());
    op->setTo(compose("%1", m_id_counter++));


    TestDecoder decoder{factories};
    decoder.streamBegin();
    Atlas::Objects::ObjectsEncoder encoder(decoder);
    m_connection->setEncoder(&encoder);
    m_connection->externalOperation(op, *m_connection);
    m_connection->dispatch(1);

    // Should result in Unseen
    ASSERT_TRUE(decoder.m_obj.isValid());
    ASSERT_EQUAL(decoder.m_obj->getParent(),
                 "unseen");
    ASSERT_TRUE(!Atlas::Objects::smart_dynamic_cast<Atlas::Objects::Operation::RootOperation>(decoder.m_obj)->isDefaultTo());
    ASSERT_EQUAL(Atlas::Objects::smart_dynamic_cast<Atlas::Objects::Operation::RootOperation>(decoder.m_obj)->getTo(), mind.getId());
}

int main()
{
    ConnectionCreatorintegration t;

    return t.run();
}

// stubs

#include "server/Lobby.h"
#include "server/Player.h"

#include "rules/AtlasProperties.h"
#include "rules/BBoxProperty.h"
#include "rules/Domain.h"
#include "rules/simulation/EntityProperty.h"
#include "rules/Script.h"
#include "rules/simulation/StatusProperty.h"
#include "rules/simulation/Task.h"
#include "rules/simulation/TasksProperty.h"

#include "common/CommSocket.h"
#include "common/id.h"
#include "common/Inheritance.h"
#include "common/Property_impl.h"
#include "common/PropertyManager.h"
#include "../stubs/rules/entityfilter/stubFilter.h"
#include "../stubs/rules/entityfilter/stubProviderFactory.h"
#include "../stubs/common/stubAssetsHandler.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

bool restricted_flag;

#include "../stubs/server/stubExternalMindsManager.h"
#include "../stubs/server/stubExternalMindsConnection.h"

#include "../stubs/server/stubPlayer.h"
#include "../stubs/server/stubAccount.h"
#include "../stubs/server/stubConnectableRouter.h"
#include "../stubs/server/stubServerRouting.h"
#include "../stubs/server/stubLobby.h"

