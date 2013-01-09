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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"

#include "server/Account.h"
#include "server/CommServer.h"
#include "server/EntityBuilder.h"
#include "server/Ruleset.h"
#include "server/ServerRouting.h"
#include "server/Connection.h"
#include "server/WorldRouter.h"

#include "rulesets/Character.h"
#include "rulesets/Domain.h"
#include "rulesets/Entity.h"
#include "rulesets/ExternalMind.h"

#include "common/CommSocket.h"
#include "common/Inheritance.h"
#include "common/SystemTime.h"

#include "TestWorld.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <cassert>

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

class TestCommSocket : public CommSocket {
  public:
    explicit TestCommSocket(CommServer & cs) : CommSocket(cs) { }
    virtual ~TestCommSocket() { }

    int getFd() const { return 0; }
    bool isOpen() const { return true; }
    bool eof() { return false; }
    int read() { return 0; }
    void dispatch() { }
    void disconnect() { }
    int flush() { return 0; }
};

class TestAccount : public Account {
  public:
    TestAccount(Connection * conn, const std::string & username,
                                   const std::string & passwd,
                                   const std::string & id, long intId) :
        Account(conn, username, passwd, id, intId) {
    }

    virtual int characterError(const Operation & op,
                               const Atlas::Objects::Root & ent,
                               OpVector & res) const {
        return 0;
    }
};

class Accountintegration : public Cyphesis::TestBase
{
    SystemTime * m_time;
    WorldRouter * m_world;

    ServerRouting * m_server;

    CommServer * m_commServer;

    CommSocket * m_tc;
    Connection * m_c;
    TestAccount * m_ac;

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
};

Accountintegration::Accountintegration()
{
    (void)new Domain;

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

void Accountintegration::setup()
{
    m_time = new SystemTime;
    m_world = new WorldRouter(*m_time);

    m_server = new ServerRouting(*m_world, "noruleset", "unittesting",
                         "1", 1, "2", 2);

    m_commServer = new CommServer;

    m_tc = new TestCommSocket(*m_commServer);
    m_c = new Connection(*m_tc, *m_server, "addr", "3", 3);
    m_ac = new TestAccount(m_c, "user", "password", "4", 4);
}

void Accountintegration::teardown()
{
    delete m_ac;
    delete m_world;
    EntityBuilder::del();
    Ruleset::del();
    Inheritance::clear();
}

void Accountintegration::test_addNewCharacter()
{
    Anonymous new_char;
    Entity * chr = m_ac->addNewCharacter("thing", new_char,
                                         RootEntity());
    assert(chr != 0);

    std::cout << "Test 1" << std::endl << std::flush;
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
    op_arg->setParents(std::list<std::string>(1, "game_entity"));
    op_arg->setName("Bob");

    Create op;
    op->setArgs1(op_arg);

    OpVector res;
    m_ac->operation(op, res);
}

void Accountintegration::test_GetOperation()
{
    Anonymous op_arg;
    op_arg->setParents(std::list<std::string>());

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
    Anonymous new_char;
    Entity * chr = m_ac->addNewCharacter("thing", new_char,
                                         RootEntity());

    Anonymous op_arg;
    op_arg->setId("1");
    op_arg->setId(chr->getId());

    Look op;
    op->setArgs1(op_arg);

    OpVector res;
    m_ac->operation(op, res);

    // FIXME This doesn't test a lot
}

void Accountintegration::test_SetOperation()
{
    Anonymous new_char;
    Entity * chr = m_ac->addNewCharacter("thing", new_char,
                                         RootEntity());
    BBox newBox(WFMath::Point<3>(-0.5, -0.5, 0.0),
                WFMath::Point<3>(-0.5, -0.5, 2.0));
    chr->m_location.setBBox(newBox);

    Anonymous op_arg;

    op_arg->setId(chr->getId());
    op_arg->setAttr("guise", "foo");
    op_arg->setAttr("height", 3.0);
    op_arg->setAttr("tasks", ListType());

    Set op;
    op->setArgs1(op_arg);

    OpVector res;
    m_ac->operation(op, res);

    // FIXME Ensure character has been modified
}

void Accountintegration::test_TalkOperation()
{
    Anonymous op_arg;
    op_arg->setParents(std::list<std::string>());
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
    op_arg->setParents(std::list<std::string>());
    op->setArgs1(op_arg);

    OpVector res;
    m_ac->operation(op, res);

    // FIXME Account should have been removed from Lobby, and also from
    // Connection
}

void Accountintegration::test_connectCharacter_entity()
{
    Entity *e = new Entity("7", 7);

    int ret = m_ac->connectCharacter(e);
    ASSERT_NOT_EQUAL(ret, 0);
}

void Accountintegration::test_connectCharacter_character()
{
    Character * e = new Character("8", 8);

    int ret = m_ac->connectCharacter(e);
    ASSERT_EQUAL(ret, 0);
    ASSERT_NOT_NULL(e->m_externalMind);
    ASSERT_TRUE(e->m_externalMind->isLinkedTo(m_c));
}

int main()
{
    database_flag = false;

    Accountintegration t;

    return t.run();
}

void TestWorld::message(const Operation & op, Entity & ent)
{
}

Entity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}

// stubs

#include "Property_stub_impl.h"

#include "server/ArithmeticBuilder.h"
#include "server/CommServer.h"
#include "server/EntityFactory.h"
#include "server/Juncture.h"
#include "server/Persistence.h"
#include "server/Player.h"
#include "server/Ruleset.h"

#include "rulesets/Motion.h"
#include "rulesets/Pedestrian.h"
#include "rulesets/AreaProperty.h"
#include "rulesets/AtlasProperties.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/BiomassProperty.h"
#include "rulesets/BurnSpeedProperty.h"
#include "rulesets/CalendarProperty.h"
#include "rulesets/EntityProperty.h"
#include "rulesets/ExternalProperty.h"
#include "rulesets/HandlerProperty.h"
#include "rulesets/InternalProperties.h"
#include "rulesets/LineProperty.h"
#include "rulesets/MindProperty.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/SolidProperty.h"
#include "rulesets/SpawnProperty.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/StatisticsProperty.h"
#include "rulesets/TasksProperty.h"
#include "rulesets/TerrainModProperty.h"
#include "rulesets/TerrainProperty.h"
#include "rulesets/TransientProperty.h"
#include "rulesets/VisibilityProperty.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/PropertyFactory.h"
#include "common/system.h"
#include "common/TypeNode.h"
#include "common/Variable.h"

namespace Atlas { namespace Objects { namespace Operation {
int ACTUATE_NO = -1;
int ATTACK_NO = -1;
int BURN_NO = -1;
int DROP_NO = -1;
int EAT_NO = -1;
int GOAL_INFO_NO = -1;
int NOURISH_NO = -1;
int SETUP_NO = -1;
int PICKUP_NO = -1;
int TELEPORT_NO = -1;
int TICK_NO = -1;
int THOUGHT_NO = -1;
int UNSEEN_NO = -1;
int UPDATE_NO = -1;
} } }

ArithmeticBuilder * ArithmeticBuilder::m_instance = 0;

ArithmeticBuilder::ArithmeticBuilder()
{
}

ArithmeticBuilder * ArithmeticBuilder::instance()
{
    if (m_instance == 0) {
        m_instance = new ArithmeticBuilder;
    }
    return m_instance;
}

ArithmeticScript * ArithmeticBuilder::newArithmetic(const std::string & name,
                                                    Entity * owner)
{
    return 0;
}

CommServer::CommServer() : m_epollFd(-1),
                           m_congested(false),
                           m_tick(0)
{
}

template <class T>
EntityFactory<T>::EntityFactory(EntityFactory<T> & o)
{
}

template <class T>
EntityFactory<T>::EntityFactory()
{
}

template <class T>
EntityFactory<T>::~EntityFactory()
{
}

template <class T>
Entity * EntityFactory<T>::newEntity(const std::string & id, long intId)
{
    return new Entity(id, intId);
}

template <class T>
EntityKit * EntityFactory<T>::duplicateFactory()
{
    return 0;
}

class Creator;
class Plant;
class Stackable;
class World;

template <>
Entity * EntityFactory<World>::newEntity(const std::string & id, long intId)
{
    return 0;
}

template <>
Entity * EntityFactory<Character>::newEntity(const std::string & id, long intId)
{
    return new Character(id, intId);
}

template class EntityFactory<Thing>;
template class EntityFactory<Character>;
template class EntityFactory<Creator>;
template class EntityFactory<Plant>;
template class EntityFactory<Stackable>;
template class EntityFactory<World>;

Juncture::~Juncture()
{
}

void Juncture::externalOperation(const Operation & op, Link &)
{
}

void Juncture::operation(const Operation & op, OpVector & res)
{
}

void Juncture::addToMessage(MapType & omap) const
{
}

void Juncture::addToEntity(const RootEntity & ent) const
{
}

void Juncture::LoginOperation(const Operation & op, OpVector & res)
{
}

void Juncture::OtherOperation(const Operation & op, OpVector & res)
{
}

int Juncture::teleportEntity(const Entity * ent)
{
    return 0;
}

Persistence * Persistence::m_instance = NULL;

Persistence::Persistence() : m_db(*(Database*)0)
{
}

Persistence * Persistence::instance()
{
    if (m_instance == NULL) {
        m_instance = new Persistence();
    }
    return m_instance;
}

Account * Persistence::getAccount(const std::string & name)
{
    return 0;
}

void Persistence::putAccount(const Account & ac)
{
}

void Persistence::registerCharacters(Account & ac,
                                     const EntityDict & worldObjects)
{
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

Ruleset * Ruleset::m_instance = NULL;

Ruleset::~Ruleset()
{
}

CalendarProperty::CalendarProperty()
{
}

int CalendarProperty::get(Element & ent) const
{
    return 0;
}

void CalendarProperty::set(const Element & ent)
{
}

CalendarProperty * CalendarProperty::copy() const
{
    return 0;
}

IdProperty::IdProperty(const std::string & data) : PropertyBase(per_ephem),
                                                   m_data(data)
{
}

int IdProperty::get(Atlas::Message::Element & e) const
{
    e = m_data;
    return 0;
}

void IdProperty::set(const Atlas::Message::Element & e)
{
}

void IdProperty::add(const std::string & key,
                     Atlas::Message::MapType & ent) const
{
}

void IdProperty::add(const std::string & key,
                     const Atlas::Objects::Entity::RootEntity & ent) const
{
}

IdProperty * IdProperty::copy() const
{
    return 0;
}

AreaProperty::AreaProperty()
{
}

AreaProperty::~AreaProperty()
{
}

void AreaProperty::set(const Atlas::Message::Element & ent)
{
}

AreaProperty * AreaProperty::copy() const
{
    return 0;
}

void AreaProperty::apply(Entity * owner)
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

OutfitProperty::OutfitProperty()
{
}

OutfitProperty::~OutfitProperty()
{
}

int OutfitProperty::get(Atlas::Message::Element & val) const
{
    return 0;
}

void OutfitProperty::set(const Atlas::Message::Element & val)
{
}

void OutfitProperty::add(const std::string & key,
                         Atlas::Message::MapType & map) const
{
}

void OutfitProperty::add(const std::string & key,
                         const Atlas::Objects::Entity::RootEntity & ent) const
{
}

OutfitProperty * OutfitProperty::copy() const
{
    return 0;
}

void OutfitProperty::cleanUp()
{
}

void OutfitProperty::wear(Entity * wearer,
                          const std::string & location,
                          Entity * garment)
{
}

void OutfitProperty::itemRemoved(Entity * garment, Entity * wearer)
{
}

TasksProperty::TasksProperty() : PropertyBase(per_ephem), m_task(0)
{
}

int TasksProperty::get(Atlas::Message::Element & val) const
{
    return 0;
}

void TasksProperty::set(const Atlas::Message::Element & val)
{
}

TasksProperty * TasksProperty::copy() const
{
    return 0;
}

int TasksProperty::startTask(Task *, Entity *, const Operation &, OpVector &)
{
    return 0;
}

int TasksProperty::updateTask(Entity *, OpVector &)
{
    return 0;
}

int TasksProperty::clearTask(Entity *, OpVector &)
{
    return 0;
}

void TasksProperty::stopTask(Entity *, OpVector &)
{
}

void TasksProperty::TickOperation(Entity *, const Operation &, OpVector &)
{
}

void TasksProperty::UseOperation(Entity *, const Operation &, OpVector &)
{
}

HandlerResult TasksProperty::operation(Entity *, const Operation &, OpVector &)
{
    return OPERATION_IGNORED;
}

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(Entity *)
{
}

void PropertyBase::apply(Entity *)
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

HandlerResult PropertyBase::operation(Entity *,
                                      const Operation &,
                                      OpVector &)
{
    return OPERATION_IGNORED;
}

template class Property<int>;
template class Property<double>;
template class Property<std::string>;
template class Property<ListType>;
template class Property<MapType>;
template class Property<std::vector<std::string>>;

PropertyKit::~PropertyKit()
{
}

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

void StatusProperty::apply(Entity * owner)
{
}

BBoxProperty::BBoxProperty()
{
}

void BBoxProperty::apply(Entity * ent)
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

HandlerResult BiomassProperty::eat_handler(Entity * e,
                                           const Operation & op,
                                           OpVector & res)
{
    return OPERATION_IGNORED;
}

HandlerResult BurnSpeedProperty::burn_handler(Entity * e,
                                              const Operation & op,
                                              OpVector & res)
{
    return OPERATION_IGNORED;
}

TerrainProperty::TerrainProperty() :
      m_data(*(Mercator::Terrain*)0),
      m_tileShader(*(Mercator::TileShader*)0)
{
}

TerrainProperty::~TerrainProperty()
{
}

int TerrainProperty::get(Element & ent) const
{
    return 0;
}

void TerrainProperty::set(const Element & ent)
{
}

TerrainProperty * TerrainProperty::copy() const
{
    return 0;
}

int TerrainProperty::getSurface(const Point3D & pos, int & material)
{
    return 0;
}

bool TerrainProperty::getHeightAndNormal(float x,
                                         float y,
                                         float & height,
                                         Vector3D & normal) const
{
    return true;
}

TerrainModProperty::TerrainModProperty(const HandlerMap & handlers) :
                    m_modptr(0), m_handlers(handlers), m_innerMod(0)
{
}

TerrainModProperty::~TerrainModProperty()
{
}

TerrainModProperty * TerrainModProperty::copy() const
{
    return 0;
}

void TerrainModProperty::install(Entity * owner)
{
}

void TerrainModProperty::apply(Entity * owner)
{
}

void TerrainModProperty::move(Entity* owner)
{
}

void TerrainModProperty::remove(Entity * owner)
{
}

Pedestrian::Pedestrian(Entity & body) : Movement(body)
{
}

Mercator::TerrainMod * TerrainModProperty::parseModData(Entity * owner,
                                                        const MapType & modMap)
{
    return 0;
}

int TerrainModProperty::getAttr(const std::string & name,
                                Element & val)
{
    return 0;
}

void TerrainModProperty::setAttr(const std::string & name,
                                 const Element & val)
{
}

SetupProperty::SetupProperty()
{
}

SetupProperty * SetupProperty::copy() const
{
    return 0;
}

void SetupProperty::install(Entity * ent)
{
}

TickProperty::TickProperty()
{
}

TickProperty * TickProperty::copy() const
{
    return 0;
}

void TickProperty::apply(Entity * ent)
{
}

SimpleProperty::SimpleProperty()
{
}

int SimpleProperty::get(Element & ent) const
{
    return 0;
}

void SimpleProperty::set(const Element & ent)
{
}

SimpleProperty * SimpleProperty::copy() const
{
    return 0;
}

void SimpleProperty::apply(Entity * owner)
{
}

LineProperty::LineProperty()
{
}

int LineProperty::get(Element & ent) const
{
    return 0;
}

void LineProperty::set(const Element & ent)
{
}

void LineProperty::add(const std::string & s, MapType & ent) const
{
}

LineProperty * LineProperty::copy() const
{
    return 0;
}

MindProperty::MindProperty() : m_factory(0)
{
}

int MindProperty::get(Element & val) const
{
    return 0;
}

void MindProperty::set(const Element & val)
{
}

MindProperty * MindProperty::copy() const
{
    return 0;
}

void MindProperty::apply(Entity * ent)
{
}

SpawnProperty::SpawnProperty()
{
}

SpawnProperty::~SpawnProperty()
{
}

SpawnProperty * SpawnProperty::copy() const
{
    return 0;
}

void SpawnProperty::apply(Entity * ent)
{
}

VisibilityProperty::VisibilityProperty()
{
}

VisibilityProperty::~VisibilityProperty()
{
}

VisibilityProperty * VisibilityProperty::copy() const
{
    return 0;
}

void VisibilityProperty::apply(Entity * ent)
{
}

StatisticsProperty::StatisticsProperty() : m_script(0)
{
}

StatisticsProperty::~StatisticsProperty()
{
}

void StatisticsProperty::install(Entity * ent)
{
}

void StatisticsProperty::apply(Entity * ent)
{
}

int StatisticsProperty::get(Element & val) const
{
    return 0;
}

void StatisticsProperty::set(const Element & ent)
{
}

StatisticsProperty * StatisticsProperty::copy() const
{
    return 0;
}

SolidProperty::SolidProperty()
{
}

int SolidProperty::get(Element & ent) const
{
    return 0;
}

void SolidProperty::set(const Element & ent)
{
}

SolidProperty * SolidProperty::copy() const
{
    return 0;
}

void SolidProperty::apply(Entity * owner)
{
}

TransientProperty::TransientProperty()
{
}

TransientProperty::~TransientProperty()
{
}

TransientProperty * TransientProperty::copy() const
{
    return 0;
}

void TransientProperty::install(Entity * ent)
{
}

void TransientProperty::apply(Entity * ent)
{
}

template <typename T>
HandlerProperty<T>::HandlerProperty(int op, Handler handler) :
                                    m_operationClassNo(op),
                                    m_handler(handler)
{
}

template <typename T>
HandlerProperty<T> * HandlerProperty<T>::copy() const
{
    return 0;
}

template <typename T>
void HandlerProperty<T>::install(Entity * ent)
{
}

template class HandlerProperty<int>;
template class HandlerProperty<double>;
template class HandlerProperty<std::string>;

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
    Atlas::Objects::Operation::Move moveOp;
    return moveOp;
}

Movement::Movement(Entity & body) : m_body(body),
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

Motion::Motion(Entity & body) : m_entity(body), m_serialno(0),
                                m_collision(false)
{
}

Motion::~Motion()
{
}

float Motion::checkCollisions()
{
    return consts::move_tick;
}

bool Motion::resolveCollision()
{
    return true;
}

void Motion::setMode(const std::string & mode)
{
}

void Motion::adjustPostion()
{
}

Operation * Motion::genUpdateOperation()
{
    return 0;
}

Operation * Motion::genMoveOperation()
{
    return 0;
}

bool_config_register::bool_config_register(bool & var,
                                           const char * section,
                                           const char * setting,
                                           const char * help)
{
}

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

EntityKit::EntityKit() : m_scriptFactory(0),
                         m_parent(0),
                         m_type(0),
                         m_createdCount(0)
{
}

EntityKit::~EntityKit()
{
}

void EntityKit::addProperties()
{
}

void EntityKit::updateProperties()
{
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

const Root & Inheritance::getClass(const std::string & parent)
{
    return noClass;
}

const TypeNode * Inheritance::getType(const std::string & parent)
{
    return 0;
}

bool Inheritance::isTypeOf(const TypeNode * instance,
                           const std::string & base_type) const
{
    return false;
}

bool Inheritance::isTypeOf(const std::string & instance,
                           const std::string & base_type) const
{
    return false;
}

TypeNode * Inheritance::addChild(const Root & obj)
{
    return new TypeNode(obj->getId());
}

void Inheritance::clear()
{
}

Monitors * Monitors::m_instance = NULL;

Monitors::Monitors()
{
}

Monitors::~Monitors()
{
}

Monitors * Monitors::instance()
{
    if (m_instance == NULL) {
        m_instance = new Monitors();
    }
    return m_instance;
}

void Monitors::insert(const std::string & key,
                      const Atlas::Message::Element & val)
{
}

void Monitors::watch(const::std::string & name, VariableBase * monitor)
{
}

Shaker::Shaker()
{
}

std::string Shaker::generateSalt(size_t length)
{
    return std::string("x", length * 2);
}

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

bool TypeNode::isTypeOf(const std::string & base_type) const
{
    return false;
}

VariableBase::~VariableBase()
{
}

template <typename T>
Variable<T>::Variable(const T & variable) : m_variable(variable)
{
}

template <typename T>
Variable<T>::~Variable()
{
}

template <typename T>
void Variable<T>::send(std::ostream & o)
{
}

template class Variable<int>;
template class Variable<std::string>;
template class Variable<const char *>;

const char * CYPHESIS = "cyphesis";

static const char * DEFAULT_INSTANCE = "cyphesis";

std::string instance(DEFAULT_INSTANCE);
int timeoffset = 0;
bool database_flag = false;

namespace consts {

  const char * buildTime = __TIME__;
  const char * buildDate = __DATE__;
  const int buildId = -1;
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

void log(LogLevel lvl, const std::string & msg)
{
}

void logEvent(LogEvent lev, const std::string & msg)
{
}

Root atlasClass(const std::string & name, const std::string & parent)
{
    return Root();
}

void hash_password(const std::string & pwd, const std::string & salt,
                   std::string & hash)
{
}

int check_password(const std::string & pwd, const std::string & hash)
{
    return -1;
}
