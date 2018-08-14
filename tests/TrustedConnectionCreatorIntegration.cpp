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

#include "server/TrustedConnection.h"
#include "server/ServerRouting.h"

#include "rulesets/ExternalMind.h"

#include "common/const.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <Atlas/Objects/Operation.h>

#include <cstdio>
#include <cstring>

#include <cassert>

using Atlas::Objects::Operation::RootOperation;
using String::compose;

class TrustedConnectionCreatorintegration : public Cyphesis::TestBase
{
  protected:
    long m_id_counter;
    static LogEvent m_logEvent_logged;
    static Operation m_Link_send_sent;
    static Operation m_BaseWorld_message_called;
    static LocatedEntity * m_BaseWorld_message_called_from;
    static Operation m_Entity_callOperation_called;

    ServerRouting * m_server;
    Connection * m_connection;
    Creator * m_creator;
    TypeNode * m_creatorType;
    std::unique_ptr<TestWorld> m_world;

    public:
    TrustedConnectionCreatorintegration();

    void setup();

    void teardown();

    void test_external_op();
    void test_external_op_override();
    void test_external_op_puppet();
    void test_external_op_puppet_nonexistant();

    static void logEvent_logged(LogEvent le);
    static void Link_send_sent(const Operation & op);
    static void BaseWorld_message_called(const Operation & op, LocatedEntity &);
    static void Entity_callOperation_called(const Operation & op);
};

LogEvent TrustedConnectionCreatorintegration::m_logEvent_logged = NONE;
Operation TrustedConnectionCreatorintegration::m_Link_send_sent(0);
Operation TrustedConnectionCreatorintegration::m_BaseWorld_message_called(0);
LocatedEntity * TrustedConnectionCreatorintegration::m_BaseWorld_message_called_from(0);
Operation TrustedConnectionCreatorintegration::m_Entity_callOperation_called(0);

void TrustedConnectionCreatorintegration::logEvent_logged(LogEvent le)
{
    m_logEvent_logged = le;
}

void TrustedConnectionCreatorintegration::Link_send_sent(const Operation & op)
{
    m_Link_send_sent = op;
}

void TrustedConnectionCreatorintegration::BaseWorld_message_called(
      const Operation & op,
      LocatedEntity & ent)
{
    m_BaseWorld_message_called = op;
    m_BaseWorld_message_called_from = &ent;
}

void TrustedConnectionCreatorintegration::Entity_callOperation_called(
      const Operation & op)
{
    m_Entity_callOperation_called = op;
}

TrustedConnectionCreatorintegration::TrustedConnectionCreatorintegration() :
    m_id_counter(0L),
    m_connection(0),
    m_creator(0),
    m_creatorType(0)
{
    ADD_TEST(TrustedConnectionCreatorintegration::test_external_op);
    ADD_TEST(TrustedConnectionCreatorintegration::test_external_op_override);
    ADD_TEST(TrustedConnectionCreatorintegration::test_external_op_puppet);
    ADD_TEST(TrustedConnectionCreatorintegration::test_external_op_puppet_nonexistant);
}

void TrustedConnectionCreatorintegration::setup()
{
    TestWorld::extension.messageFn = &TrustedConnectionCreatorintegration::BaseWorld_message_called;
    m_Link_send_sent = 0;
    m_BaseWorld_message_called = 0;
    m_BaseWorld_message_called_from = 0;
    m_Entity_callOperation_called = 0;

    Ref<Entity> gw = new Entity(compose("%1", m_id_counter),
                             m_id_counter++);
    m_world.reset();
    m_world.reset(new TestWorld(gw));
    m_server = new ServerRouting(*m_world,
                                 "dd7452be-0137-4664-b90e-77dfb395ac39",
                                 "a2feda8e-62e9-4ba0-95c4-09f92eda6a78",
                                 compose("%1", m_id_counter), m_id_counter++,
                                 compose("%1", m_id_counter), m_id_counter++);
    m_connection = new Connection(*(CommSocket*)0,
                                  *m_server,
                                  "25251955-7e8c-4043-8a5e-adfb8a1e76f7",
                                  compose("%1", m_id_counter), m_id_counter++);
    m_creator = new Creator(compose("%1", m_id_counter), m_id_counter++);
    m_creatorType = new TypeNode("test_avatar");
    m_creator->setType(m_creatorType);

    m_connection->addObject(m_creator);

    m_BaseWorld_message_called = 0;
    m_BaseWorld_message_called_from = 0;
}

void TrustedConnectionCreatorintegration::teardown()
{
    delete m_connection;
    delete m_creator;
    delete m_creatorType;
    delete m_server;
}

void TrustedConnectionCreatorintegration::test_external_op()
{
    // Dispatching a Talk external op from the creator should result in
    // it being passed on to the world, exactly as if this was a Character
    // except that we assume that Creator was set up linked.

    m_creator->m_externalMind = new ExternalMind(*m_creator);
    m_creator->m_externalMind->linkUp(m_connection);

    Atlas::Objects::Operation::Talk op;
    op->setFrom(m_creator->getId());

    m_connection->externalOperation(op, *m_connection);

    // BaseWorld::message should have been called from Enitty::sendWorld
    // with the Talk operation, modified to have TO set to the character.
    ASSERT_TRUE(m_BaseWorld_message_called.isValid());
    ASSERT_EQUAL(m_BaseWorld_message_called->getClassNo(),
                 Atlas::Objects::Operation::TALK_NO);
    ASSERT_TRUE(!m_BaseWorld_message_called->isDefaultTo());
    ASSERT_EQUAL(m_BaseWorld_message_called->getTo(), m_creator->getId());
    ASSERT_NOT_NULL(m_BaseWorld_message_called_from);
    ASSERT_EQUAL(m_BaseWorld_message_called_from, m_creator);
}

void TrustedConnectionCreatorintegration::test_external_op_override()
{
    // Dispatching a Talk external op from the creator should result in
    // it being passed on to the world, exactly as if this was a Character
    // except that we assume that Creator was set up linked.

    m_creator->m_externalMind = new ExternalMind(*m_creator);
    m_creator->m_externalMind->linkUp(m_connection);

    Atlas::Objects::Operation::Talk op;
    op->setFrom(m_creator->getId());
    op->setTo(m_creator->getId());

    m_connection->externalOperation(op, *m_connection);

    // The operation should have been passed to Entity::callOperation for
    // dispatch, completely unfiltered.
    ASSERT_TRUE(m_Entity_callOperation_called.isValid());
    ASSERT_EQUAL(m_Entity_callOperation_called->getClassNo(),
                 Atlas::Objects::Operation::TALK_NO);
    ASSERT_TRUE(!m_Entity_callOperation_called->isDefaultTo());
    ASSERT_EQUAL(m_Entity_callOperation_called->getTo(), m_creator->getId());
}

void TrustedConnectionCreatorintegration::test_external_op_puppet()
{
    // Dispatching a Talk external op from the creator, to the creator should
    // result in it being passed directly to the normal op dispatch,
    // shortcutting the world.

    m_creator->m_externalMind = new ExternalMind(*m_creator);
    m_creator->m_externalMind->linkUp(m_connection);

    Ref<Entity>  other = new Entity(compose("%1", m_id_counter), m_id_counter++);
    other->setType(m_creatorType);
    m_server->m_world.addEntity(other);

    Atlas::Objects::Operation::Talk op;
    op->setFrom(m_creator->getId());
    op->setTo(other->getId());

    m_connection->externalOperation(op, *m_connection);

    // Operation should be via world dispatch, as if it was from the Entity
    // we are puppeting.
    ASSERT_TRUE(m_BaseWorld_message_called.isValid());
    ASSERT_EQUAL(m_BaseWorld_message_called->getClassNo(),
                 Atlas::Objects::Operation::TALK_NO);
    ASSERT_TRUE(!m_BaseWorld_message_called->isDefaultTo());
    ASSERT_EQUAL(m_BaseWorld_message_called->getTo(), other->getId());
    ASSERT_NOT_NULL(m_BaseWorld_message_called_from);
    ASSERT_EQUAL(m_BaseWorld_message_called_from, other.get());
}

void TrustedConnectionCreatorintegration::test_external_op_puppet_nonexistant()
{
    // Dispatching a Talk external op from the creator, to the creator should
    // result in it being passed directly to the normal op dispatch,
    // shortcutting the world.

    m_creator->m_externalMind = new ExternalMind(*m_creator);
    m_creator->m_externalMind->linkUp(m_connection);

    Ref<Entity>  other = new Entity(compose("%1", m_id_counter), m_id_counter++);
    other->setType(m_creatorType);
    m_server->m_world.addEntity(other);

    Atlas::Objects::Operation::Talk op;
    op->setFrom(m_creator->getId());
    op->setTo(compose("%1", m_id_counter++));

    m_connection->externalOperation(op, *m_connection);

    // Operation should be via world dispatch, as if it was from the Entity
    // we are puppeting.
    ASSERT_TRUE(m_Link_send_sent.isValid());
    ASSERT_EQUAL(m_Link_send_sent->getParent(),
                 "unseen");
    ASSERT_TRUE(!m_Link_send_sent->isDefaultTo());
    ASSERT_EQUAL(m_Link_send_sent->getTo(), m_creator->getId());
}

int main()
{
    TrustedConnectionCreatorintegration t;

    return t.run();
}

// stubs

#include "server/Lobby.h"
#include "server/Player.h"

#include "rulesets/AtlasProperties.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/Domain.h"
#include "rulesets/EntityProperty.h"
#include "rulesets/ExternalProperty.h"
#include "rulesets/Script.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/Task.h"
#include "rulesets/TasksProperty.h"

#include "common/CommSocket.h"
#include "common/id.h"
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
#include "stubs/rulesets/stubPedestrian.h"
#include "stubs/rulesets/stubMovement.h"
#include "stubs/server/stubExternalMindsManager.h"
#include "stubs/server/stubExternalMindsConnection.h"
#include "stubs/common/stubOperationsDispatcher.h"
#include "stubs/modules/stubWorldTime.h"
#include "stubs/modules/stubDateTime.h"
#include "stubs/rulesets/stubLocation.h"



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

Player::Player(Connection * conn,
               const std::string & username,
               const std::string & passwd,
               const std::string & id,
               long intId) :
        Account(conn, username, passwd, id, intId)
{
}

const char * Player::getType() const
{
    return "player";
}

void Player::addToMessage(MapType & omap) const
{
}

void Player::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

int Player::characterError(const Operation & op,
                           const Root & ent, OpVector & res) const
{
    return 0;
}
#include "stubs/server/stubAccount.h"

ConnectableRouter::ConnectableRouter(const std::string & id,
                                 long iid,
                                 Connection *c) :
                 Router(id, iid),
                 m_connection(c)
{
}
#include "stubs/server/stubServerRouting.h"


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
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/rulesets/stubOutfitProperty.h"
#include "stubs/physics/stubVector3D.h"

#define STUB_Entity_callOperation
void Entity::callOperation(const Operation & op, OpVector & res)
{
    TrustedConnectionCreatorintegration::Entity_callOperation_called(op);
}

#define STUB_Entity_setProperty
PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return m_properties[name] = prop;
}

#define STUB_Entity_sendWorld
void Entity::sendWorld(const Operation & op)
{
    BaseWorld::instance().message(op, *this);
}

#define STUB_Entity_setType
void Entity::setType(const TypeNode* t) {
    m_type = t;

}

#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubEntityProperty.h"
#include "stubs/rulesets/stubUsagesProperty.h"
#include "stubs/rulesets/entityfilter/stubFilter.h"
#include "stubs/rulesets/stubTask.h"
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
    TrustedConnectionCreatorintegration::Link_send_sent(op);
}

void Link::send(const OpVector& opVector) const
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

#ifndef STUB_BaseWorld_getEntity
#define STUB_BaseWorld_getEntity
Ref<LocatedEntity> BaseWorld::getEntity(const std::string & id) const
{
    return getEntity(integerId(id));
}

Ref<LocatedEntity> BaseWorld::getEntity(long id) const
{
    auto I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second);
        return I->second;
    } else {
        return nullptr;
    }
}
#endif //STUB_BaseWorld_getEntity

#include "stubs/rulesets/stubBaseWorld.h"

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
#include "stubs/modules/stubWeakEntityRef.h"


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
    TrustedConnectionCreatorintegration::logEvent_logged(lev);
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
