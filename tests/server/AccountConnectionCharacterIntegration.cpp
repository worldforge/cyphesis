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
#include "server/Player.h"
#include "server/ServerRouting.h"

#include "rules/simulation/ExternalMind.h"
#include "rules/simulation/Entity.h"
#include "rules/simulation/MindsProperty.h"

#include "common/const.h"
#include "common/log.h"
#include "common/TypeNode.h"
#include "../DatabaseNull.h"

#include <Atlas/Objects/Operation.h>

#include <cstdio>
#include <cstring>

#include <cassert>
#include <server/Persistence.h>
#include <common/Monitors.h>

using Atlas::Objects::Operation::RootOperation;
using String::compose;

Monitors monitors;

/// Test code paths between account, connection and avatar classes
class AccountConnectionCharacterintegration : public Cyphesis::TestBase
{
  protected:
    long m_id_counter;
    static LogEvent m_logEvent_logged;
    static Operation m_Link_send_sent;

    DatabaseNull m_database;
    Persistence* m_persistence;
    ServerRouting * m_server;
    Connection * m_connection;
    Account * m_account;
    Ref<Entity> m_character;
    TypeNode * m_characterType;
    std::unique_ptr<TestWorld> m_world;
  public:
    AccountConnectionCharacterintegration();

    void setup();

    void teardown();

    void test_subscribe();
    void test_connect_existing();
    void test_unsubscribe();
    void test_unsubscribe_other();

};

LogEvent AccountConnectionCharacterintegration::m_logEvent_logged = NONE;

AccountConnectionCharacterintegration::AccountConnectionCharacterintegration() :
    m_id_counter(0L),
    m_connection(0),
    m_character(0),
    m_characterType(0)
{

    ADD_TEST(AccountConnectionCharacterintegration::test_subscribe);
    ADD_TEST(AccountConnectionCharacterintegration::test_connect_existing);
    ADD_TEST(AccountConnectionCharacterintegration::test_unsubscribe);
    ADD_TEST(AccountConnectionCharacterintegration::test_unsubscribe_other);
}

void AccountConnectionCharacterintegration::setup()
{
    m_persistence = new Persistence(m_database);

    Ref<Entity> gw = new Entity(m_id_counter++);
    m_world.reset();
    m_world.reset(new TestWorld(gw));
    m_server = new ServerRouting(*m_world,
                                 *m_persistence,
                                 "989cfbbe-67e3-4571-858c-488b91e06e7d",
                                 "10658e5e-373b-4565-b34e-954b9223961e",
                                 m_id_counter++,
                                 AssetsHandler({}));
    m_connection = new Connection(*(CommSocket*)0,
                                  *m_server,
                                  "a4754783-9909-476b-a418-6997477dff49",
                                  m_id_counter++);
    m_account = new Player(m_connection,
                           "fred",
                           "25846125-f1bb-4963-852e-856a8be45515",
                           m_id_counter++);
    m_character = new Entity(m_id_counter++);
    m_characterType = new TypeNode("test_avatar");
    m_character->setType(m_characterType);

}

void AccountConnectionCharacterintegration::teardown()
{
    m_character.reset();
    m_world->shutdown();
    m_world.reset();
    delete m_connection;
    delete m_account;
    delete m_characterType;
    delete m_server;
    delete m_persistence;
}

void AccountConnectionCharacterintegration::test_subscribe()
{

    //Test Possess instead

//    // Inject an external op through the connection which is from
//    // the Character. This should result in the Character being set up
//    // for IG with an external mind linked back to the Connection.
//
//    // Initial state is that the account and character objects already
//    // belong to the connection
//    m_connection->m_objects[m_account->getIntId()] = m_account;
//    m_connection->m_objects[m_character->getIntId()] = m_character;
//
//    ASSERT_NULL(m_character->m_externalMind)
//
//    RootOperation op;
//    op->setFrom(m_character->getId());
//
//    m_connection->externalOperation(op, *m_connection);
//
//    ASSERT_NOT_NULL(m_character->m_externalMind)
//    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(m_connection))
//    ASSERT_TRUE(m_Link_send_sent.isValid());
//    ASSERT_EQUAL(m_Link_send_sent->getClassNo(),
//                 Atlas::Objects::Operation::INFO_NO);
//    ASSERT_EQUAL(m_logEvent_logged, TAKE_CHAR);
}

void AccountConnectionCharacterintegration::test_connect_existing()
{
    // Invote Account::connectCharacter to set up the character for IG
    // with an external mind linked back to the connection.

    // Initial state is that the account already belongs to the connection,
    // but the character does not yet, as it is new.

    m_connection->m_routers[m_account->getIntId()].router = m_account;

    ASSERT_TRUE(m_connection->m_routers.find(m_character->getIntId()) ==
                m_connection->m_routers.end())

    ASSERT_NULL(m_character->getPropertyClassFixed<MindsProperty>())

    OpVector res;
    m_account->connectCharacter(m_character, res);

    ASSERT_NOT_NULL(m_character->getPropertyClassFixed<MindsProperty>())
    auto mind = dynamic_cast<ExternalMind*>(m_character->getPropertyClassFixed<MindsProperty>()->getMinds().front());
    ASSERT_TRUE(mind->isLinkedTo(m_connection))
//    ASSERT_TRUE(m_connection->m_objects.find(m_character->getIntId()) !=
//                m_connection->m_objects.end())
}

void AccountConnectionCharacterintegration::test_unsubscribe()
{
//    // Initial state is that the account already belongs to the connection,
//    // and the character is linked up.
//
//    m_connection->m_objects[m_account->getIntId()] = m_account;
//    m_connection->m_connectableRouters[m_account->getIntId()] = m_account;
//    m_connection->m_objects[m_character->getIntId()] = m_character.get();
//
//    auto mind = new ExternalMind(6, *m_character);
//    m_character->requirePropertyClassFixed<MindsProperty>()->addMind(mind);
//    mind->linkUp(m_connection);
//
//    ASSERT_TRUE(m_connection->m_objects.find(m_character->getIntId()) !=
//                m_connection->m_objects.end())
//    ASSERT_NOT_NULL(mind)
//    ASSERT_TRUE(mind->isLinkedTo(m_connection))
//
//    m_connection->disconnectObject(
//          m_connection->m_connectableRouters.find(m_account->getIntId())->second,
//          "test_disconnect_event"
//    );
//
//    //ASSERT_EQUAL(m_logEvent_logged, DROP_CHAR);
//    ASSERT_TRUE(!mind->isLinked())
//    ASSERT_TRUE(!mind->isLinkedTo(m_connection))
//    ASSERT_TRUE(m_connection->m_objects.find(m_character->getIntId()) !=
//                m_connection->m_objects.end())
}

void AccountConnectionCharacterintegration::test_unsubscribe_other()
{
    // Initial state is that the account already belongs to the connection,
    // and the character is linked up to another connection

    m_connection->m_routers[m_account->getIntId()].router = m_account;
    m_connection->m_connectableRouters[m_account->getIntId()] = m_account;
    m_connection->m_routers[m_character->getIntId()].router = m_character.get();

    Connection  other_connection(*(CommSocket*)0,
                         *m_server,
                         "242eedae-6a2e-4c5b-9901-711b14d7e851",
                         m_id_counter++);


    ExternalMind mind(6, m_character);
    m_character->requirePropertyClassFixed<MindsProperty>().addMind(&mind);
    mind.linkUp(&other_connection);

    ASSERT_TRUE(m_connection->m_routers.find(m_character->getIntId()) !=
                m_connection->m_routers.end())
    ASSERT_TRUE(mind.isLinked())
    ASSERT_TRUE(!mind.isLinkedTo(m_connection))
    ASSERT_TRUE(mind.isLinkedTo(&other_connection))

    m_connection->disconnectObject(
        m_connection->m_connectableRouters.find(m_account->getIntId())->second,
        "test_disconnect_event"
    );

    //ASSERT_NOT_EQUAL(m_logEvent_logged, DROP_CHAR);
    ASSERT_TRUE(mind.isLinked())
    ASSERT_TRUE(!mind.isLinkedTo(m_connection))
    ASSERT_TRUE(mind.isLinkedTo(&other_connection))
    ASSERT_TRUE(m_connection->m_routers.find(m_character->getIntId()) !=
                m_connection->m_routers.end())
}



int main()
{
    AccountConnectionCharacterintegration t;

    return t.run();
}

// stubs

#include "server/Lobby.h"
#include "server/Persistence.h"
#include "server/PossessionAuthenticator.h"

#include "rules/AtlasProperties.h"
#include "rules/BBoxProperty.h"
#include "rules/Domain.h"
#include "rules/simulation/EntityProperty.h"
#include "rules/Script.h"
#include "rules/simulation/StatusProperty.h"
#include "rules/simulation/Task.h"
#include "rules/simulation/TasksProperty.h"

#include "common/CommSocket.h"
#include "common/Inheritance.h"
#include "common/Property_impl.h"
#include "common/PropertyManager.h"
#include "../stubs/common/stubAssetsHandler.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

bool restricted_flag;


#include "../stubs/server/stubExternalMindsManager.h"
#include "../stubs/server/stubExternalMindsConnection.h"
#include "../stubs/server/stubPossessionAuthenticator.h"
#include "../stubs/server/stubPlayer.h"
#include "../stubs/server/stubConnectableRouter.h"
#include "../stubs/server/stubServerRouting.h"
#include "../stubs/server/stubPersistence.h"
#include "../stubs/server/stubLobby.h"
#include "../stubs/rules/entityfilter/stubProviderFactory.h"
#include "../stubs/rules/entityfilter/stubFilter.h"
