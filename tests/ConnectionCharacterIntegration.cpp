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
#include "server/ServerRouting.h"

#include "rulesets/Character.h"
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

class ConnectionCharacterintegration : public Cyphesis::TestBase
{
  protected:
    long m_id_counter;
    static LogEvent m_logEvent_logged;
    static Operation m_Link_send_sent;
    static Operation m_BaseWorld_message_called;
    static LocatedEntity * m_BaseWorld_message_called_from;

    ServerRouting * m_server;
    Connection * m_connection;
    Character * m_character;
    TypeNode * m_characterType;
  public:
    ConnectionCharacterintegration();

    void setup();

    void teardown();

    void test_connect_up();
    void test_connected();
    void test_unlinked();
    void test_external_op();

    static void logEvent_logged(LogEvent le);
    static void Link_send_sent(const Operation & op);
    static void BaseWorld_message_called(const Operation & op, LocatedEntity &);
};

LogEvent ConnectionCharacterintegration::m_logEvent_logged = NONE;
Operation ConnectionCharacterintegration::m_Link_send_sent(0);
Operation ConnectionCharacterintegration::m_BaseWorld_message_called(0);
LocatedEntity * ConnectionCharacterintegration::m_BaseWorld_message_called_from(0);

void ConnectionCharacterintegration::logEvent_logged(LogEvent le)
{
    m_logEvent_logged = le;
}

void ConnectionCharacterintegration::Link_send_sent(const Operation & op)
{
    m_Link_send_sent = op;
}

void ConnectionCharacterintegration::BaseWorld_message_called(
      const Operation & op,
      LocatedEntity & ent)
{
    m_BaseWorld_message_called = op;
    m_BaseWorld_message_called_from = &ent;
}

ConnectionCharacterintegration::ConnectionCharacterintegration() :
    m_id_counter(0L),
    m_connection(0),
    m_character(0),
    m_characterType(0)
{
    ADD_TEST(ConnectionCharacterintegration::test_connect_up);
    ADD_TEST(ConnectionCharacterintegration::test_connected);
    ADD_TEST(ConnectionCharacterintegration::test_unlinked);
    ADD_TEST(ConnectionCharacterintegration::test_external_op);
}

void ConnectionCharacterintegration::setup()
{
    m_Link_send_sent = 0;

    Entity * gw = new Entity(compose("%1", m_id_counter),
                             m_id_counter++);
    m_server = new ServerRouting(*new TestWorld(*gw),
                                 "dd7452be-0137-4664-b90e-77dfb395ac39",
                                 "a2feda8e-62e9-4ba0-95c4-09f92eda6a78",
                                 compose("%1", m_id_counter), m_id_counter++,
                                 compose("%1", m_id_counter), m_id_counter++);
    m_connection = new Connection(*(CommSocket*)0,
                                  *m_server,
                                  "25251955-7e8c-4043-8a5e-adfb8a1e76f7",
                                  compose("%1", m_id_counter), m_id_counter++);
    m_character = new Character(compose("%1", m_id_counter), m_id_counter++);
    m_characterType = new TypeNode("test_avatar");
    m_character->setType(m_characterType);

    m_connection->addObject(m_character);

    m_BaseWorld_message_called = 0;
    m_BaseWorld_message_called_from = 0;
}

void ConnectionCharacterintegration::teardown()
{
    delete m_connection;
    delete m_character;
    delete m_characterType;
    delete m_server;
}

void ConnectionCharacterintegration::test_connect_up()
{
    // Dispatching an external op from the character should cause it to
    // get connected up with an external mind

    RootOperation op;
    op->setFrom(m_character->getId());

    ASSERT_NULL(m_character->m_externalMind);

    m_connection->externalOperation(op, *m_connection);

    ASSERT_NOT_NULL(m_character->m_externalMind);
    ExternalMind * em =
          dynamic_cast<ExternalMind*>(m_character->m_externalMind);
    ASSERT_NOT_NULL(em);
    ASSERT_TRUE(em->isLinked());
    ASSERT_TRUE(em->isLinkedTo(m_connection));
    ASSERT_TRUE(m_Link_send_sent.isValid());
    ASSERT_EQUAL(m_Link_send_sent->getClassNo(),
                 Atlas::Objects::Operation::INFO_NO);
    ASSERT_EQUAL(m_logEvent_logged, TAKE_CHAR);
}

void ConnectionCharacterintegration::test_connected()
{
    // Dispatching an external op from the character should have no effect
    // if the external mind is already in place.

    m_character->linkExternal(m_connection);

    ASSERT_NOT_NULL(m_character->m_externalMind);
    ExternalMind * em =
          dynamic_cast<ExternalMind*>(m_character->m_externalMind);
    ASSERT_NOT_NULL(em);
    ASSERT_TRUE(em->isLinked());
    ASSERT_TRUE(em->isLinkedTo(m_connection));

    Router * saved_em = m_character->m_externalMind;

    RootOperation op;
    op->setFrom(m_character->getId());

    m_connection->externalOperation(op, *m_connection);

    ASSERT_TRUE(!m_Link_send_sent.isValid());
    ASSERT_NOT_EQUAL(m_logEvent_logged, TAKE_CHAR);
    ASSERT_NOT_NULL(m_character->m_externalMind);
    ASSERT_EQUAL(m_character->m_externalMind, saved_em);
    em = dynamic_cast<ExternalMind*>(m_character->m_externalMind);
    ASSERT_NOT_NULL(em);
    ASSERT_TRUE(em->isLinked());
    ASSERT_TRUE(em->isLinkedTo(m_connection));
}

void ConnectionCharacterintegration::test_unlinked()
{
    // Dispatching an external op from the character if the external mind is
    // already in place, but is not linked to a connection should link it
    // back up.

    m_character->linkExternal(m_connection);

    ASSERT_NOT_NULL(m_character->m_externalMind);
    ExternalMind * em =
          dynamic_cast<ExternalMind*>(m_character->m_externalMind);
    ASSERT_NOT_NULL(em);
    ASSERT_TRUE(em->isLinked());
    ASSERT_TRUE(em->isLinkedTo(m_connection));

    // Remove the link from the external mind back to m_connection
    em->linkUp(0);

    ASSERT_TRUE(!em->isLinked());

    Router * saved_em = m_character->m_externalMind;

    RootOperation op;
    op->setFrom(m_character->getId());

    m_connection->externalOperation(op, *m_connection);

    ASSERT_NOT_NULL(m_character->m_externalMind);
    ASSERT_EQUAL(m_character->m_externalMind, saved_em);
    em = dynamic_cast<ExternalMind*>(m_character->m_externalMind);
    ASSERT_NOT_NULL(em);
    ASSERT_TRUE(em->isLinked());
    ASSERT_TRUE(em->isLinkedTo(m_connection));
    ASSERT_TRUE(m_Link_send_sent.isValid());
    ASSERT_EQUAL(m_logEvent_logged, TAKE_CHAR);
}

void ConnectionCharacterintegration::test_external_op()
{
    // Dispatching a Talk external op from the character should result in
    // it being passed on to the world.

    m_character->linkExternal(m_connection);

    Atlas::Objects::Operation::Talk op;
    op->setFrom(m_character->getId());

    m_connection->externalOperation(op, *m_connection);

    // BaseWorld::message should have been called from Enitty::sendWorld
    // with the Talk operation, modified to have TO set to the character.
    ASSERT_TRUE(m_BaseWorld_message_called.isValid());
    ASSERT_EQUAL(m_BaseWorld_message_called->getClassNo(),
                 Atlas::Objects::Operation::TALK_NO);
    ASSERT_TRUE(!m_BaseWorld_message_called->isDefaultTo());
    ASSERT_EQUAL(m_BaseWorld_message_called->getTo(), m_character->getId());
    ASSERT_NOT_NULL(m_BaseWorld_message_called_from);
    ASSERT_EQUAL(m_BaseWorld_message_called_from, m_character);
}

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
    ConnectionCharacterintegration::BaseWorld_message_called(op, ent);
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}

int main()
{
    ConnectionCharacterintegration t;

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
#include "rulesets/OutfitProperty.h"
#include "rulesets/Pedestrian.h"
#include "rulesets/Script.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/Task.h"
#include "rulesets/TasksProperty.h"

#include "common/CommSocket.h"
#include "common/Inheritance.h"
#include "common/Property_impl.h"
#include "common/PropertyManager.h"

#include "stubs/common/stubCustom.h"
#include "stubs/server/stubAccount.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/rulesets/stubThing.h"
#include "stubs/rulesets/stubTasksProperty.h"
#include "stubs/rulesets/stubOutfitProperty.h"
#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"
#include "stubs/rulesets/stubProxyMind.h"
#include "stubs/rulesets/stubBaseMind.h"
#include "stubs/rulesets/stubMemEntity.h"
#include "stubs/rulesets/stubMemMap.h"
#include "stubs/server/stubExternalMindsManager.h"
#include "stubs/server/stubExternalMindsConnection.h"
#include "stubs/common/stubOperationsDispatcher.h"
#include "stubs/modules/stubDateTime.h"
#include "stubs/modules/stubWorldTime.h"
#include "stubs/modules/stubLocation.h"
#include "stubs/physics/stubVector3D.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

bool restricted_flag;

Pedestrian::Pedestrian(LocatedEntity & body) : Movement(body)
{
}

Pedestrian::~Pedestrian()
{
}

double Pedestrian::getTickAddition(const Point3D & coordinates,
                                   const Vector3D & velocity) const
{
    return consts::basic_tick;
}

int Pedestrian::getUpdatedLocation(Location & return_location)
{
    return 1;
}

Operation Pedestrian::generateMove(const Location & new_location)
{
    Atlas::Objects::Operation::RootOperation moveOp;
    return moveOp;
}

Movement::Movement(LocatedEntity & body) : m_body(body),
                                    m_serialno(0)
{
}

Movement::~Movement()
{
}

bool Movement::updateNeeded(const Location & location) const
{
    return true;
}

void Movement::reset()
{
}

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

Player::~Player() { }

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


ConnectableRouter::ConnectableRouter(const std::string & id,
                                 long iid,
                                 Connection *c) :
                 Router(id, iid),
                 m_connection(c)
{
}

ConnectableRouter::~ConnectableRouter()
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

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId)
{
}

Entity::~Entity()
{
}

void Entity::destroy()
{
    destroyed.emit();
}

void Entity::ActuateOperation(const Operation &, OpVector &)
{
}

void Entity::AppearanceOperation(const Operation &, OpVector &)
{
}

void Entity::AttackOperation(const Operation &, OpVector &)
{
}

void Entity::CombineOperation(const Operation &, OpVector &)
{
}

void Entity::CreateOperation(const Operation &, OpVector &)
{
}

void Entity::DeleteOperation(const Operation &, OpVector &)
{
}

void Entity::DisappearanceOperation(const Operation &, OpVector &)
{
}

void Entity::DivideOperation(const Operation &, OpVector &)
{
}

void Entity::EatOperation(const Operation &, OpVector &)
{
}

void Entity::GetOperation(const Operation &, OpVector &)
{
}

void Entity::InfoOperation(const Operation &, OpVector &)
{
}

void Entity::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Entity::LookOperation(const Operation &, OpVector &)
{
}

void Entity::MoveOperation(const Operation &, OpVector &)
{
}

void Entity::NourishOperation(const Operation &, OpVector &)
{
}

void Entity::SetOperation(const Operation &, OpVector &)
{
}

void Entity::SightOperation(const Operation &, OpVector &)
{
}

void Entity::SoundOperation(const Operation &, OpVector &)
{
}

void Entity::TalkOperation(const Operation &, OpVector &)
{
}

void Entity::TickOperation(const Operation &, OpVector &)
{
}

void Entity::TouchOperation(const Operation &, OpVector &)
{
}

void Entity::UpdateOperation(const Operation &, OpVector &)
{
}

void Entity::UseOperation(const Operation &, OpVector &)
{
}

void Entity::WieldOperation(const Operation &, OpVector &)
{
}

void Entity::RelayOperation(const Operation &, OpVector &)
{
}

void Entity::externalOperation(const Operation & op, Link &)
{
}

void Entity::operation(const Operation & op, OpVector & res)
{
}

void Entity::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Entity::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

PropertyBase * Entity::setAttr(const std::string & name,
                               const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return m_properties[name] = prop;
}

void Entity::installDelegate(int class_no, const std::string & delegate)
{
}

void Entity::removeDelegate(int class_no, const std::string & delegate)
{
}

void Entity::addChild(LocatedEntity& childEntity)
{
}

void Entity::removeChild(LocatedEntity& childEntity)
{
}

Domain * Entity::getDomain()
{
    return 0;
}

const Domain * Entity::getDomain() const
{
    return 0;
}

void Entity::sendWorld(const Operation & op)
{
    BaseWorld::instance().message(op, *this);
}

void Entity::onContainered(const LocatedEntity*)
{
}

void Entity::onUpdated()
{
}

void Entity::setType(const TypeNode* t) {
    m_type = t;
}



EntityProperty::EntityProperty()
{
}

int EntityProperty::get(Atlas::Message::Element & val) const
{
    return 0;
}

void EntityProperty::set(const Atlas::Message::Element & val)
{
}

void EntityProperty::add(const std::string & s,
                         Atlas::Message::MapType & map) const
{
}

void EntityProperty::add(const std::string & s,
                         const Atlas::Objects::Entity::RootEntity & ent) const
{
}

EntityProperty * EntityProperty::copy() const
{
    return 0;
}

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

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(LocatedEntity *, const std::string & name)
{
}

void PropertyBase::remove(LocatedEntity *, const std::string & name)
{
}

void PropertyBase::apply(LocatedEntity *)
{
}

void PropertyBase::add(const std::string & s,
                       Atlas::Message::MapType & ent) const
{
    get(ent[s]);
}

void PropertyBase::add(const std::string & s,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
}

HandlerResult PropertyBase::operation(LocatedEntity *,
                                      const Operation &,
                                      OpVector &)
{
    return OPERATION_IGNORED;
}

template<>
void Property<int>::set(const Atlas::Message::Element & e)
{
    if (e.isInt()) {
        this->m_data = e.asInt();
    }
}

template<>
void Property<double>::set(const Atlas::Message::Element & e)
{
    if (e.isNum()) {
        this->m_data = e.asNum();
    }
}

template<>
void Property<std::string>::set(const Atlas::Message::Element & e)
{
    if (e.isString()) {
        this->m_data = e.String();
    }
}

template class Property<int>;
template class Property<double>;
template class Property<std::string>;

SoftProperty::SoftProperty()
{
}

SoftProperty::SoftProperty(const Atlas::Message::Element & data) :
              PropertyBase(0), m_data(data)
{
}

int SoftProperty::get(Atlas::Message::Element & val) const
{
    val = m_data;
    return 0;
}

void SoftProperty::set(const Atlas::Message::Element & val)
{
}

SoftProperty * SoftProperty::copy() const
{
    return 0;
}

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

StatusProperty::StatusProperty()
{
}

StatusProperty * StatusProperty::copy() const
{
    return 0;
}

void StatusProperty::apply(LocatedEntity * owner)
{
}

BBoxProperty::BBoxProperty()
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

PropertyManager * PropertyManager::m_instance = 0;

PropertyManager::PropertyManager()
{
    assert(m_instance == 0);
    m_instance = this;
}

PropertyManager::~PropertyManager()
{
   m_instance = 0;
}

int PropertyManager::installFactory(const std::string & type_name,
                                    const Atlas::Objects::Root & type_desc,
                                    PropertyKit * factory)
{
    return 0;
}

Link::Link(CommSocket & socket, const std::string & id, long iid) :
            Router(id, iid), m_encoder(0), m_commSocket(socket)
{
}

Link::~Link()
{
}

void Link::send(const OpVector& opVector) const
{
}

void Link::send(const Operation & op) const
{
    ConnectionCharacterintegration::Link_send_sent(op);
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

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
    delete &m_gameWorld;
}

double BaseWorld::getTime() const
{
    return .0;
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    return 0;
}

LocatedEntity * BaseWorld::getEntity(long id) const
{
    return 0;
}

LocatedEntity& BaseWorld::getDefaultLocation() {
    return m_gameWorld;
}

LocatedEntity& BaseWorld::getDefaultLocation() const {
    return m_gameWorld;
}

LocatedEntity& BaseWorld::getRootEntity() {
    return m_gameWorld;
}

LocatedEntity& BaseWorld::getRootEntity() const {
    return m_gameWorld;
}


Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance() : noClass(0)
{
}

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
    }
    return *m_instance;
}

const TypeNode * Inheritance::getType(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second;
}

const Root & Inheritance::getClass(const std::string & parent)
{
    return noClass;
}

EntityRef::EntityRef(LocatedEntity* e) : m_inner(e)
{
}

EntityRef::EntityRef(const EntityRef& ref) : m_inner(ref.m_inner)
{
}

EntityRef& EntityRef::operator=(const EntityRef& ref)
{
    m_inner = ref.m_inner;

    return *this;
}

void EntityRef::onEntityDeleted()
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
    ConnectionCharacterintegration::logEvent_logged(lev);
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
