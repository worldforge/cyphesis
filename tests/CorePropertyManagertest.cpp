// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Alistair Riddoch
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

#include "server/CorePropertyManager.h"

#include "rulesets/Character.h"
#include "rulesets/Domain.h"
#include "rulesets/Entity.h"
#include "rulesets/ExternalMind.h"

#include "common/CommSocket.h"
#include "common/Inheritance.h"
#include "common/PropertyFactory.h"
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

class MinimalProperty : public PropertyBase {
  public:
    MinimalProperty() { }
    virtual int get(Atlas::Message::Element & val) const { return 0; }
    virtual void set(const Atlas::Message::Element & val) { }
    virtual MinimalProperty * copy() const { return 0; }
};

class CorePropertyManagertest : public Cyphesis::TestBase
{
    CorePropertyManager * m_propertyManager;

  public:
    CorePropertyManagertest();

    void setup();
    void teardown();

    void test_addProperty_int();
    void test_addProperty_float();
    void test_addProperty_string();
    void test_addProperty_list();
    void test_addProperty_map();
    void test_addProperty_none();
    void test_addProperty_named();
};

CorePropertyManagertest::CorePropertyManagertest()
{
    ADD_TEST(CorePropertyManagertest::test_addProperty_int);
    ADD_TEST(CorePropertyManagertest::test_addProperty_float);
    ADD_TEST(CorePropertyManagertest::test_addProperty_string);
    ADD_TEST(CorePropertyManagertest::test_addProperty_list);
    ADD_TEST(CorePropertyManagertest::test_addProperty_map);
    ADD_TEST(CorePropertyManagertest::test_addProperty_none);
    ADD_TEST(CorePropertyManagertest::test_addProperty_named);
}

void CorePropertyManagertest::setup()
{
    m_propertyManager = new CorePropertyManager;
    m_propertyManager->m_propertyFactories.insert(
        std::make_pair("named_type", new PropertyFactory<MinimalProperty>)
    );
   
}

void CorePropertyManagertest::teardown()
{
    delete m_propertyManager;
}

void CorePropertyManagertest::test_addProperty_int()
{
    auto * p = m_propertyManager->addProperty("non_existant_type",
                                              Element::TYPE_INT);
    ASSERT_NOT_NULL(p);
    ASSERT_NOT_NULL(dynamic_cast<Property<int> *>(p));
}

void CorePropertyManagertest::test_addProperty_float()
{
    auto * p = m_propertyManager->addProperty("non_existant_type",
                                              Element::TYPE_FLOAT);
    ASSERT_NOT_NULL(p);
    ASSERT_NOT_NULL(dynamic_cast<Property<double> *>(p));
}

void CorePropertyManagertest::test_addProperty_string()
{
    auto * p = m_propertyManager->addProperty("non_existant_type",
                                              Element::TYPE_STRING);
    ASSERT_NOT_NULL(p);
    ASSERT_NOT_NULL(dynamic_cast<Property<std::string> *>(p));
}

void CorePropertyManagertest::test_addProperty_list()
{
    auto * p = m_propertyManager->addProperty("non_existant_type",
                                              Element::TYPE_LIST);
    ASSERT_NOT_NULL(p);
    ASSERT_NOT_NULL(dynamic_cast<SoftProperty *>(p));
}

void CorePropertyManagertest::test_addProperty_map()
{
    auto * p = m_propertyManager->addProperty("non_existant_type",
                                              Element::TYPE_MAP);
    ASSERT_NOT_NULL(p);
    ASSERT_NOT_NULL(dynamic_cast<SoftProperty *>(p));
}

void CorePropertyManagertest::test_addProperty_none()
{
    auto * p = m_propertyManager->addProperty("non_existant_type",
                                              Element::TYPE_NONE);
    ASSERT_NOT_NULL(p);
    ASSERT_NOT_NULL(dynamic_cast<SoftProperty *>(p));
}

void CorePropertyManagertest::test_addProperty_named()
{
    auto * p = m_propertyManager->addProperty("named_type",
                                              Element::TYPE_NONE);
    ASSERT_NOT_NULL(p);
    ASSERT_NOT_NULL(dynamic_cast<MinimalProperty *>(p));
}

int main()
{
    CorePropertyManagertest t;

    return t.run();
}

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
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
#include "server/ServerRouting.h"
#include "server/TeleportProperty.h"

#include "rulesets/Motion.h"
#include "rulesets/Pedestrian.h"
#include "rulesets/AreaProperty.h"
#include "rulesets/AtlasProperties.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/BiomassProperty.h"
#include "rulesets/BurnSpeedProperty.h"
#include "rulesets/CalendarProperty.h"
#include "rulesets/DecaysProperty.h"
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

Account::Account(Connection * conn,
                 const std::string & uname,
                 const std::string & passwd,
                 const std::string & id,
                 long intId) :
         ConnectableRouter(id, intId, conn),
         m_username(uname), m_password(passwd)
{
}

Account::~Account()
{
}

const char * Account::getType() const
{
    return "test_account";
}

void Account::store() const
{
}

void Account::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Account::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Account::createObject(const std::string & type_str,
                           const Atlas::Objects::Root & arg,
                           const Operation & op,
                           OpVector & res)
{
}

void Account::externalOperation(const Operation &, Link &)
{
}

void Account::operation(const Operation & op, OpVector & res)
{
}

void Account::LogoutOperation(const Operation & op, OpVector & res)
{
}

void Account::CreateOperation(const Operation & op, OpVector & res)
{
}

void Account::SetOperation(const Operation & op, OpVector & res)
{
}

void Account::ImaginaryOperation(const Operation & op, OpVector & res)
{
}

void Account::TalkOperation(const Operation & op, OpVector & res)
{
}

void Account::LookOperation(const Operation & op, OpVector & res)
{
}

void Account::GetOperation(const Operation & op, OpVector & res)
{
}

void Account::OtherOperation(const Operation & op, OpVector & res)
{
}

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
                                                    LocatedEntity * owner)
{
    return 0;
}

CommServer::CommServer() : m_epollFd(-1),
                           m_congested(false),
                           m_tick(0)
{
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
LocatedEntity * EntityFactory<T>::newEntity(const std::string & id, long intId)
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
LocatedEntity * EntityFactory<World>::newEntity(const std::string & id,
                                                long intId)
{
    return 0;
}

template <>
LocatedEntity * EntityFactory<Character>::newEntity(const std::string & id,
                                                    long intId)
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

int Juncture::teleportEntity(const LocatedEntity * ent)
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

ServerRouting * ServerRouting::m_instance = 0;

Router * ServerRouting::getObject(const std::string & id) const
{
    return 0;
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

Character::Character(const std::string & id, long intId) :
           Thing(id, intId),
               m_movement(*(Movement*)0),
               m_mind(0), m_externalMind(0)
{
}

Character::~Character()
{
}

void Character::operation(const Operation & op, OpVector &)
{
}

void Character::externalOperation(const Operation & op, Link &)
{
}


void Character::ImaginaryOperation(const Operation & op, OpVector &)
{
}

void Character::InfoOperation(const Operation & op, OpVector &)
{
}

void Character::TickOperation(const Operation & op, OpVector &)
{
}

void Character::TalkOperation(const Operation & op, OpVector &)
{
}

void Character::NourishOperation(const Operation & op, OpVector &)
{
}

void Character::UseOperation(const Operation & op, OpVector &)
{
}

void Character::WieldOperation(const Operation & op, OpVector &)
{
}

void Character::AttackOperation(const Operation & op, OpVector &)
{
}

void Character::ActuateOperation(const Operation & op, OpVector &)
{
}

void Character::mindActuateOperation(const Operation &, OpVector &)
{
}

void Character::mindAttackOperation(const Operation &, OpVector &)
{
}

void Character::mindCombineOperation(const Operation &, OpVector &)
{
}

void Character::mindCreateOperation(const Operation &, OpVector &)
{
}

void Character::mindDeleteOperation(const Operation &, OpVector &)
{
}

void Character::mindDivideOperation(const Operation &, OpVector &)
{
}

void Character::mindEatOperation(const Operation &, OpVector &)
{
}

void Character::mindGoalInfoOperation(const Operation &, OpVector &)
{
}

void Character::mindImaginaryOperation(const Operation &, OpVector &)
{
}

void Character::mindLookOperation(const Operation &, OpVector &)
{
}

void Character::mindMoveOperation(const Operation &, OpVector &)
{
}

void Character::mindSetOperation(const Operation &, OpVector &)
{
}

void Character::mindSetupOperation(const Operation &, OpVector &)
{
}

void Character::mindTalkOperation(const Operation &, OpVector &)
{
}

void Character::mindThoughtOperation(const Operation &, OpVector &)
{
}

void Character::mindTickOperation(const Operation &, OpVector &)
{
}

void Character::mindTouchOperation(const Operation &, OpVector &)
{
}

void Character::mindUpdateOperation(const Operation &, OpVector &)
{
}

void Character::mindUseOperation(const Operation &, OpVector &)
{
}

void Character::mindWieldOperation(const Operation &, OpVector &)
{
}


void Character::mindOtherOperation(const Operation &, OpVector &)
{
}

void Character::sendMind(const Operation & op, OpVector & res)
{
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

void AreaProperty::apply(LocatedEntity * owner)
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
        LocatedEntity(id, intId), m_motion(0)
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
    ent->setId(getId());
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
    return 0;
}

void Entity::installHandler(int class_no, Handler handler)
{
}

void Entity::installDelegate(int class_no, const std::string & delegate)
{
}

Domain * Entity::getMovementDomain()
{
    return 0;
}

void Entity::sendWorld(const Operation & op)
{
}

void Entity::onContainered()
{
}

void Entity::onUpdated()
{
}

void Entity::callOperation(const Operation & op, OpVector & res)
{
}

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

int LocatedEntity::getAttr(const std::string & name,
                           Atlas::Message::Element & attr) const
{
    return -1;
}

int LocatedEntity::getAttrType(const std::string & name,
                               Atlas::Message::Element & attr,
                               int type) const
{
    return -1;
}

PropertyBase * LocatedEntity::setAttr(const std::string & name,
                                      const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * LocatedEntity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * LocatedEntity::setProperty(const std::string & name,
                                          PropertyBase * prop)
{
    return 0;
}

void LocatedEntity::installHandler(int, Handler)
{
}

void LocatedEntity::installDelegate(int, const std::string &)
{
}

void LocatedEntity::destroy()
{
}

Domain * LocatedEntity::getMovementDomain()
{
    return 0;
}

void LocatedEntity::sendWorld(const Operation & op)
{
}

void LocatedEntity::onContainered()
{
}

void LocatedEntity::onUpdated()
{
}

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
}

void LocatedEntity::merge(const MapType & ent)
{
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

void OutfitProperty::wear(LocatedEntity * wearer,
                          const std::string & location,
                          LocatedEntity * garment)
{
}

void OutfitProperty::itemRemoved(LocatedEntity * garment, LocatedEntity * wearer)
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

int TasksProperty::startTask(Task *, LocatedEntity *, const Operation &, OpVector &)
{
    return 0;
}

int TasksProperty::updateTask(LocatedEntity *, OpVector &)
{
    return 0;
}

int TasksProperty::clearTask(LocatedEntity *, OpVector &)
{
    return 0;
}

void TasksProperty::stopTask(LocatedEntity *, OpVector &)
{
}

void TasksProperty::TickOperation(LocatedEntity *, const Operation &, OpVector &)
{
}

void TasksProperty::UseOperation(LocatedEntity *, const Operation &, OpVector &)
{
}

HandlerResult TasksProperty::operation(LocatedEntity *, const Operation &, OpVector &)
{
    return OPERATION_IGNORED;
}

Thing::Thing(const std::string & id, long intId) :
       Entity(id, intId)
{
}

Thing::~Thing()
{
}

void Thing::DeleteOperation(const Operation & op, OpVector & res)
{
}

void Thing::MoveOperation(const Operation & op, OpVector & res)
{
}

void Thing::SetOperation(const Operation & op, OpVector & res)
{
}

void Thing::LookOperation(const Operation & op, OpVector & res)
{
}

void Thing::CreateOperation(const Operation & op, OpVector & res)
{
}

void Thing::UpdateOperation(const Operation & op, OpVector & res)
{
}

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(LocatedEntity *)
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

template class Property<int>;
template class Property<double>;
template class Property<std::string>;
template class Property<ListType>;
template class Property<MapType>;
template class Property<std::vector<std::string>>;

PropertyKit::~PropertyKit()
{
}

template <class T>
PropertyBase * PropertyFactory<T>::newProperty()
{
    return new T();
}

template class PropertyFactory<MinimalProperty>;

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

HandlerResult BiomassProperty::eat_handler(LocatedEntity * e,
                                           const Operation & op,
                                           OpVector & res)
{
    return OPERATION_IGNORED;
}

HandlerResult BurnSpeedProperty::burn_handler(LocatedEntity * e,
                                              const Operation & op,
                                              OpVector & res)
{
    return OPERATION_IGNORED;
}

HandlerResult DecaysProperty::del_handler(LocatedEntity * e,
                                          const Operation &,
                                          OpVector & res)
{
    return OPERATION_IGNORED;
}

HandlerResult TeleportProperty::teleport_handler(LocatedEntity * e,
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

void TerrainModProperty::install(LocatedEntity * owner)
{
}

void TerrainModProperty::apply(LocatedEntity * owner)
{
}

void TerrainModProperty::move(LocatedEntity* owner)
{
}

void TerrainModProperty::remove(LocatedEntity * owner)
{
}

Pedestrian::Pedestrian(LocatedEntity & body) : Movement(body)
{
}

Mercator::TerrainMod * TerrainModProperty::parseModData(LocatedEntity * owner,
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

HandlerResult TerrainModProperty::move_handler(LocatedEntity * e,
                                               const Operation & op,
                                               OpVector & res)
{
    return OPERATION_IGNORED;
}

HandlerResult TerrainModProperty::delete_handler(LocatedEntity * e,
                                                 const Operation & op,
                                                 OpVector & res)
{
    return OPERATION_IGNORED;
}

SetupProperty::SetupProperty()
{
}

SetupProperty * SetupProperty::copy() const
{
    return 0;
}

void SetupProperty::install(LocatedEntity * ent)
{
}

TickProperty::TickProperty()
{
}

TickProperty * TickProperty::copy() const
{
    return 0;
}

void TickProperty::apply(LocatedEntity * ent)
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

void SimpleProperty::apply(LocatedEntity * owner)
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

void MindProperty::apply(LocatedEntity * ent)
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

void SpawnProperty::apply(LocatedEntity * ent)
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

void VisibilityProperty::apply(LocatedEntity * ent)
{
}

StatisticsProperty::StatisticsProperty() : m_script(0)
{
}

StatisticsProperty::~StatisticsProperty()
{
}

void StatisticsProperty::install(LocatedEntity * ent)
{
}

void StatisticsProperty::apply(LocatedEntity * ent)
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

void SolidProperty::apply(LocatedEntity * owner)
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

void TransientProperty::install(LocatedEntity * ent)
{
}

void TransientProperty::apply(LocatedEntity * ent)
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
void HandlerProperty<T>::install(LocatedEntity * ent)
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

Location::Location() :
    m_simple(true), m_solid(true),
    m_boxSize(0),
    m_squareBoxSize(0),
    m_loc(0)
{
}

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
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
    res.push_back(Atlas::Objects::Operation::Error());
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

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
}
