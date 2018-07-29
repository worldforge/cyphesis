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

#include "TestBase.h"
#include "TestWorld.h"

#include "server/Connection.h"
#include "server/Player.h"
#include "server/ServerRouting.h"

#include "rulesets/Character.h"
#include "rulesets/ExternalMind.h"

#include "common/const.h"
#include "common/log.h"
#include "common/TypeNode.h"
#include "DatabaseNull.h"

#include <Atlas/Objects/Operation.h>

#include <cstdio>
#include <cstring>

#include <cassert>
#include <server/Persistence.h>

using Atlas::Objects::Operation::RootOperation;
using String::compose;

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
    Character * m_character;
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

    static void logEvent_logged(LogEvent le);
    static void Link_send_sent(const Operation & op);
};

LogEvent AccountConnectionCharacterintegration::m_logEvent_logged = NONE;
Operation AccountConnectionCharacterintegration::m_Link_send_sent(0);

void AccountConnectionCharacterintegration::logEvent_logged(LogEvent le)
{
    m_logEvent_logged = le;
}

void AccountConnectionCharacterintegration::Link_send_sent(const Operation & op)
{
    m_Link_send_sent = op;
}

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

    m_Link_send_sent = 0;

    Ref<Entity> gw = new Entity(compose("%1", m_id_counter),
                             m_id_counter++);
    m_world.reset();
    m_world.reset(new TestWorld(*gw));
    m_server = new ServerRouting(*m_world,
                                 "989cfbbe-67e3-4571-858c-488b91e06e7d",
                                 "10658e5e-373b-4565-b34e-954b9223961e",
                                 compose("%1", m_id_counter), m_id_counter++,
                                 compose("%1", m_id_counter), m_id_counter++);
    m_connection = new Connection(*(CommSocket*)0,
                                  *m_server,
                                  "a4754783-9909-476b-a418-6997477dff49",
                                  compose("%1", m_id_counter), m_id_counter++);
    m_account = new Player(m_connection,
                           "fred",
                           "25846125-f1bb-4963-852e-856a8be45515",
                           compose("%1", m_id_counter), m_id_counter++);
    m_character = new Character(compose("%1", m_id_counter), m_id_counter++);
    m_characterType = new TypeNode("test_avatar");
    m_character->setType(m_characterType);

}

void AccountConnectionCharacterintegration::teardown()
{
    delete m_connection;
    delete m_character;
    delete m_characterType;
    delete m_server;
    delete m_persistence;
}

void AccountConnectionCharacterintegration::test_subscribe()
{
    // Inject an external op through the connection which is from
    // the Character. This should result in the Character being set up
    // for IG with an external mind linked back to the Connection.

    // Initial state is that the account and character objects already
    // belong to the connection
    m_connection->m_objects[m_account->getIntId()] = m_account;
    m_connection->m_objects[m_character->getIntId()] = m_character;

    ASSERT_NULL(m_character->m_externalMind)

    RootOperation op;
    op->setFrom(m_character->getId());

    m_connection->externalOperation(op, *m_connection);

    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(m_connection))
    ASSERT_TRUE(m_Link_send_sent.isValid());
    ASSERT_EQUAL(m_Link_send_sent->getClassNo(),
                 Atlas::Objects::Operation::INFO_NO);
    ASSERT_EQUAL(m_logEvent_logged, TAKE_CHAR);
}

void AccountConnectionCharacterintegration::test_connect_existing()
{
    // Invote Account::connectCharacter to set up the character for IG
    // with an external mind linked back to the connection.

    // Initial state is that the account already belongs to the connection,
    // but the character does not yet, as it is new.

    m_connection->m_objects[m_account->getIntId()] = m_account;

    ASSERT_TRUE(m_connection->m_objects.find(m_character->getIntId()) ==
                m_connection->m_objects.end())
    
    ASSERT_NULL(m_character->m_externalMind)

    m_account->connectCharacter(m_character);

    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(m_connection))
    ASSERT_TRUE(m_connection->m_objects.find(m_character->getIntId()) !=
                m_connection->m_objects.end())
}

void AccountConnectionCharacterintegration::test_unsubscribe()
{
    // Initial state is that the account already belongs to the connection,
    // and the character is linked up.

    m_connection->m_objects[m_account->getIntId()] = m_account;
    m_connection->m_objects[m_character->getIntId()] = m_character;
    m_character->linkExternal(m_connection);

    ASSERT_TRUE(m_connection->m_objects.find(m_character->getIntId()) !=
                m_connection->m_objects.end())
    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(m_connection))

    m_connection->disconnectObject(
          m_connection->m_objects.find(m_character->getIntId()),
          "test_disconnect_event"
    );

    ASSERT_EQUAL(m_logEvent_logged, DROP_CHAR);
    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_TRUE(!m_character->m_externalMind->isLinked())
    ASSERT_TRUE(!m_character->m_externalMind->isLinkedTo(m_connection))
    ASSERT_TRUE(m_connection->m_objects.find(m_character->getIntId()) !=
                m_connection->m_objects.end())
}

void AccountConnectionCharacterintegration::test_unsubscribe_other()
{
    // Initial state is that the account already belongs to the connection,
    // and the character is linked up to another connection

    m_connection->m_objects[m_account->getIntId()] = m_account;
    m_connection->m_objects[m_character->getIntId()] = m_character;

    Connection * other_connection =
          new Connection(*(CommSocket*)0,
                         *m_server,
                         "242eedae-6a2e-4c5b-9901-711b14d7e851",
                         compose("%1", m_id_counter), m_id_counter++);

    m_character->linkExternal(other_connection);

    ASSERT_TRUE(m_connection->m_objects.find(m_character->getIntId()) !=
                m_connection->m_objects.end())
    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_TRUE(m_character->m_externalMind->isLinked())
    ASSERT_TRUE(!m_character->m_externalMind->isLinkedTo(m_connection))
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(other_connection))

    m_connection->disconnectObject(
          m_connection->m_objects.find(m_character->getIntId()),
          "test_disconnect_event"
    );

    ASSERT_NOT_EQUAL(m_logEvent_logged, DROP_CHAR);
    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_TRUE(m_character->m_externalMind->isLinked())
    ASSERT_TRUE(!m_character->m_externalMind->isLinkedTo(m_connection))
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(other_connection))
    ASSERT_TRUE(m_connection->m_objects.find(m_character->getIntId()) !=
                m_connection->m_objects.end())
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

#include "rulesets/AtlasProperties.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/Domain.h"
#include "rulesets/EntityProperty.h"
#include "rulesets/ExternalProperty.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/Script.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/Task.h"
#include "rulesets/TasksProperty.h"

#include "common/CommSocket.h"
#include "common/Inheritance.h"
#include "common/Property_impl.h"
#include "common/PropertyManager.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

bool restricted_flag;

#include "stubs/rulesets/stubProxyMind.h"
#include "stubs/rulesets/stubBaseMind.h"
#include "stubs/rulesets/stubMemEntity.h"
#include "stubs/rulesets/stubMemMap.h"
#include "stubs/rulesets/stubPropelProperty.h"
#include "stubs/rulesets/stubPedestrian.h"
#include "stubs/rulesets/stubMovement.h"
#include "stubs/server/stubExternalMindsManager.h"
#include "stubs/server/stubExternalMindsConnection.h"
#include "stubs/common/stubOperationsDispatcher.h"
#include "stubs/common/stubDatabase.h"
#include "stubs/modules/stubWorldTime.h"
#include "stubs/modules/stubDateTime.h"
#include "stubs/rulesets/stubLocation.h"
#include "stubs/physics/stubVector3D.h"

namespace Atlas { namespace Objects { namespace Operation {
int ACTUATE_NO = -1;
int ATTACK_NO = -1;
int EAT_NO = -1;
int GOAL_INFO_NO = -1;
int NOURISH_NO = -1;
int SETUP_NO = -1;
int TICK_NO = -1;
int THOUGHT_NO = -1;
int UNSEEN_NO = -1;
int UPDATE_NO = -1;
int RELAY_NO = -1;
int THINK_NO = -1;
int COMMUNE_NO = -1;
} } }

CommSocket::CommSocket(boost::asio::io_service & svr) : m_io_service(svr) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
}

PossessionAuthenticator * PossessionAuthenticator::m_instance = nullptr;

int PossessionAuthenticator::removePossession(const std::string &entity_id)
{
    return 0;
}

LocatedEntity *PossessionAuthenticator::authenticatePossession(const std::string &entity_id,
                                            const std::string &possess_key)
{
    return 0;
}

#include "stubs/server/stubPlayer.h"

ConnectableRouter::ConnectableRouter(const std::string & id,
                                 long iid,
                                 Connection *c) :
                 Router(id, iid),
                 m_connection(c)
{
}

ServerRouting::ServerRouting(BaseWorld & wrld,
                             const std::string & ruleset,
                             const std::string & name,
                             const std::string & id, long intId,
                             const std::string & lId, long lIntId) :
        Router(id, intId),
        m_svrRuleset(ruleset), m_svrName(name),
        m_numClients(0), m_world(wrld), m_lobby(*(Lobby*)0)
{
}

ServerRouting::~ServerRouting()
{
}

void ServerRouting::addToMessage(Atlas::Message::MapType & omap) const
{
}

void ServerRouting::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void ServerRouting::externalOperation(const Operation &, Link &)
{
}

void ServerRouting::operation(const Operation &, OpVector &)
{
}

Account * ServerRouting::getAccountByName(const std::string & username)
{
    return 0;
}

void ServerRouting::addAccount(Account * a)
{
}

#include "stubs/server/stubPersistence.h"

Lobby::Lobby(ServerRouting & s, const std::string & id, long intId) :
       Router(id, intId),
       m_server(s)
{
}

Lobby::~Lobby()
{
}

void Lobby::delAccount(Account * ac)
{
}

void Lobby::addToMessage(MapType & omap) const
{
}

void Lobby::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Lobby::addAccount(Account * ac)
{
}

void Lobby::externalOperation(const Operation &, Link &)
{
}

void Lobby::operation(const Operation & op, OpVector & res)
{
}

ExternalProperty::ExternalProperty(ExternalMind * & data) : m_data(data)
{
}

int ExternalProperty::get(Atlas::Message::Element & val) const
{
    return 0;
}

void ExternalProperty::set(const Atlas::Message::Element & val)
{
}

void ExternalProperty::add(const std::string & s,
                         Atlas::Message::MapType & map) const
{
}

void ExternalProperty::add(const std::string & s,
                         const Atlas::Objects::Entity::RootEntity & ent) const
{
}

ExternalProperty * ExternalProperty::copy() const
{
    return 0;
}

#include "stubs/rulesets/stubThing.h"
#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/rulesets/stubOutfitProperty.h"
#include "stubs/rulesets/stubEntityProperty.h"

Task::~Task()
{
}

void Task::initTask(const Operation & op, OpVector & res)
{
}

void Task::operation(const Operation & op, OpVector & res)
{
}

void Task::irrelevant()
{
}

#include "stubs/rulesets/stubTasksProperty.h"
#define STUB_SoftProperty_get
int SoftProperty::get(Atlas::Message::Element & val) const
{
    val = m_data;
    return 0;
}
#include "stubs/common/stubProperty.h"

ContainsProperty::ContainsProperty(LocatedEntitySet & data) :
      PropertyBase(per_ephem), m_data(data)
{
}

int ContainsProperty::get(Atlas::Message::Element & e) const
{
    return 0;
}

void ContainsProperty::set(const Atlas::Message::Element & e)
{
}

void ContainsProperty::add(const std::string & s,
                           const Atlas::Objects::Entity::RootEntity & ent) const
{
}

ContainsProperty * ContainsProperty::copy() const
{
    return 0;
}

StatusProperty * StatusProperty::copy() const
{
    return 0;
}

void StatusProperty::apply(LocatedEntity * owner)
{
}

void BBoxProperty::apply(LocatedEntity * ent)
{
}

int BBoxProperty::get(Element & val) const
{
    return -1;
}

void BBoxProperty::set(const Element & val)
{
}

void BBoxProperty::add(const std::string & key,
                       MapType & map) const
{
}

void BBoxProperty::add(const std::string & key,
                       const RootEntity & ent) const
{
}

BBoxProperty * BBoxProperty::copy() const
{
    return 0;
}

#include "stubs/common/stubPropertyManager.h"

Link::Link(CommSocket & socket, const std::string & id, long iid) :
            Router(id, iid), m_encoder(0), m_commSocket(socket)
{
}

Link::~Link()
{
}

void Link::send(const Operation & op) const
{
    AccountConnectionCharacterintegration::Link_send_sent(op);
}

void Link::send(const OpVector & ops) const
{
}

void Link::sendError(const Operation & op,
                     const std::string &,
                     const std::string &) const
{
}

void Link::disconnect()
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

void Router::error(const Operation & op,
                   const std::string & errstring,
                   OpVector & res,
                   const std::string & to) const
{
}

void Router::clientError(const Operation & op,
                         const std::string & errstring,
                         OpVector & res,
                         const std::string & to) const
{
}

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

TypeNode::~TypeNode()
{
}
#include "stubs/rulesets/stubBaseWorld.h"
#include "stubs/rulesets/stubUsagesProperty.h"
#include "stubs/rulesets/entityfilter/stubFilter.h"

#define STUB_Inheritance_getClass
const Atlas::Objects::Root& Inheritance::getClass(const std::string & parent)
{
    return noClass;
}


#define STUB_Inheritance_getType
const TypeNode* Inheritance::getType(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second;}


#include "stubs/common/stubInheritance.h"


WeakEntityRef::WeakEntityRef(LocatedEntity* e) : m_inner(e)
{
}

WeakEntityRef::WeakEntityRef(const WeakEntityRef& ref) : m_inner(ref.m_inner)
{
}

WeakEntityRef& WeakEntityRef::operator=(const WeakEntityRef& ref)
{
    m_inner = ref.m_inner;

    return *this;
}

void WeakEntityRef::onEntityDeleted()
{
}

template<class V>
const Quaternion quaternionFromTo(const V & from, const V & to)
{
    return Quaternion(1.f, 0.f, 0.f, 0.f);
}

template
const Quaternion quaternionFromTo<Vector3D>(const Vector3D &, const Vector3D&);

void log(LogLevel lvl, const std::string & msg)
{
}

void logEvent(LogEvent lev, const std::string & msg)
{
    AccountConnectionCharacterintegration::logEvent_logged(lev);
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

static long idGenerator = 0;

long newId(std::string & id)
{
    static char buf[32];
    long new_id = ++idGenerator;
    sprintf(buf, "%ld", new_id);
    id = buf;
    assert(!id.empty());
    return new_id;
}


Shaker::Shaker()
{
}

std::string Shaker::generateSalt(size_t length)
{
    return "";
}

void hash_password(const std::string & pwd, const std::string & salt,
                   std::string & hash )
{
}

int check_password(const std::string & pwd, const std::string & hash)
{
    return 0;
}

bool database_flag = false;
