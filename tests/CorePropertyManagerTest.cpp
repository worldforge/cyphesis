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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"

#include "server/CorePropertyManager.h"

#include "rulesets/Domain.h"
#include "rulesets/Entity.h"
#include "rulesets/ExternalMind.h"

#include "common/CommSocket.h"
#include "common/Inheritance.h"
#include "common/PropertyFactory.h"
#include "common/SystemTime.h"

#include "TestWorld.h"

#include "stubs/rulesets/stubCharacter.h"
#include "stubs/rulesets/stubPropelProperty.h"

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
    void test_installFactory();

        Inheritance* m_inheritance;
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
    ADD_TEST(CorePropertyManagertest::test_installFactory);
}

void CorePropertyManagertest::setup()
{
    m_inheritance = new Inheritance();
    m_propertyManager = new CorePropertyManager;
    m_propertyManager->m_propertyFactories.insert(
        std::make_pair("named_type", new PropertyFactory<MinimalProperty>)
    );
   
}

void CorePropertyManagertest::teardown()
{
    delete m_inheritance;
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

void CorePropertyManagertest::test_installFactory()
{
    int ret = m_propertyManager->installFactory(
          "new_named_type",
          Root(),
          new PropertyFactory<MinimalProperty>
    );

    ASSERT_EQUAL(ret, 0);
}

int main()
{
    CorePropertyManagertest t;

    return t.run();
}





#include "common/const.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/PropertyFactory.h"
#include "common/system.h"
#include "common/TypeNode.h"
#include "common/Variable.h"


// stubs


#include "server/ArithmeticBuilder.h"
#include "server/EntityFactory.h"
#include "server/Juncture.h"
#include "server/Persistence.h"
#include "server/Player.h"
#include "server/Ruleset.h"
#include "server/ServerRouting.h"
#include "server/TeleportProperty.h"

#include "rulesets/AreaProperty.h"
#include "rulesets/AtlasProperties.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/BiomassProperty.h"
#include "rulesets/BurnSpeedProperty.h"
#include "rulesets/CalendarProperty.h"
#include "rulesets/DecaysProperty.h"
#include "rulesets/EntityProperty.h"
#include "rulesets/ExternalProperty.h"
#include "rulesets/InternalProperties.h"
#include "rulesets/LineProperty.h"
#include "rulesets/MindProperty.h"
#include "rulesets/SolidProperty.h"
#include "rulesets/SpawnProperty.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/StatisticsProperty.h"
#include "rulesets/SuspendedProperty.h"
#include "rulesets/TasksProperty.h"
#include "rulesets/TerrainModProperty.h"
#include "rulesets/TerrainProperty.h"
#include "rulesets/TransientProperty.h"
#include "rulesets/VisibilityProperty.h"
#include "rulesets/SpawnerProperty.h"
#include "rulesets/ImmortalProperty.h"
#include "rulesets/RespawningProperty.h"
#include "rulesets/DefaultLocationProperty.h"
#include "rulesets/LimboProperty.h"
#include "rulesets/DomainProperty.h"
#include "rulesets/ScriptsProperty.h"
//void Entity::destroy()
//{
//    destroyed.emit();
//}
//
//void Entity::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
//{
//    ent->setId(getId());
//}

#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"

#include "stubs/common/stubProperty.h"
#include "stubs/common/stubCustom.h"
#include "stubs/rulesets/stubImmortalProperty.h"
#include "stubs/rulesets/stubRespawningProperty.h"
#include "stubs/rulesets/stubSpawnerProperty.h"
#include "stubs/rulesets/stubTerrainModProperty.h"
#include "stubs/rulesets/stubTerrainProperty.h"
#include "stubs/rulesets/stubBBoxProperty.h"
#include "stubs/rulesets/stubBiomassProperty.h"
#include "stubs/rulesets/stubBurnSpeedProperty.h"
#include "stubs/rulesets/stubContainsProperty.h"
#include "stubs/rulesets/stubDecaysProperty.h"
#include "stubs/rulesets/stubOutfitProperty.h"
#include "stubs/rulesets/stubStatusProperty.h"
#include "stubs/rulesets/stubTasksProperty.h"
#include "stubs/rulesets/stubSpawnProperty.h"
#include "stubs/rulesets/stubDefaultLocationProperty.h"
#include "stubs/rulesets/stubLimboProperty.h"
#include "stubs/rulesets/stubDomainProperty.h"
#include "stubs/rulesets/stubSuspendedProperty.h"
#include "stubs/rulesets/stubModeProperty.h"
#include "stubs/rulesets/stubQuaternionProperty.h"
#include "stubs/rulesets/stubAngularFactorProperty.h"
#include "stubs/rulesets/stubGeometryProperty.h"
#include "stubs/rulesets/stubDensityProperty.h"
#include "stubs/rulesets/stubVector3Property.h"
#include "stubs/rulesets/stubThing.h"
#include "stubs/rulesets/stubEntityProperty.h"
#include "stubs/rulesets/stubInternalProperties.h"
#include "stubs/rulesets/stubSolidProperty.h"
#include "stubs/rulesets/stubPerceptionSightProperty.h"
#include "stubs/rulesets/stubScaleProperty.h"
#include "stubs/rulesets/stubScriptsProperty.h"
#include "stubs/server/stubAccount.h"
#include "stubs/server/stubArithmeticBuilder.h"

ConnectableRouter::ConnectableRouter(const std::string & id,
                                 long iid,
                                 Connection *c) :
                 Router(id, iid),
                 m_connection(c)
{
}


#define STUB_EntityFactory_newEntity
template <typename T>
Ref<LocatedEntity> EntityFactory<T>::newEntity(const std::string & id, long intId, const Atlas::Objects::Entity::RootEntity & attributes, LocatedEntity* location)
{
    return new Entity(id, intId);
}

class Creator;
class Plant;
class Stackable;
class World;

template <>
Ref<LocatedEntity> EntityFactory<World>::newEntity(const std::string & id, long intId,
        const Atlas::Objects::Entity::RootEntity & attributes, LocatedEntity* location)
{
    return 0;
}

template <>
Ref<LocatedEntity> EntityFactory<Character>::newEntity(const std::string & id, long intId,
        const Atlas::Objects::Entity::RootEntity & attributes, LocatedEntity* location)
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

#include "stubs/server/stubPersistence.h"
#include "stubs/server/stubPlayer.h"

#include "stubs/server/stubRuleHandler.h"

#include "stubs/server/stubEntityRuleHandler.h"
#include "stubs/server/stubArchetypeRuleHandler.h"
#include "stubs/server/stubOpRuleHandler.h"
#include "stubs/server/stubPropertyRuleHandler.h"
#include "stubs/server/stubRuleset.h"
#include "stubs/server/stubServerRouting.h"
#include "stubs/rulesets/stubTask.h"
#include "stubs/rulesets/stubUsagesProperty.h"
#include "stubs/rulesets/entityfilter/stubFilter.h"

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




PropertyKit::~PropertyKit()
{
}

template <class T>
PropertyBase * PropertyFactory<T>::newProperty()
{
    return new T();
}

template <class T>
PropertyFactory<T> * PropertyFactory<T>::duplicateFactory() const
{
    return new PropertyFactory<T>;
}

template class PropertyFactory<MinimalProperty>;


void TeleportProperty::install(LocatedEntity * owner, const std::string & name)
{
}

HandlerResult TeleportProperty::operation(LocatedEntity * ent,
                                          const Operation & op,
                                          OpVector & res)
{
    return OPERATION_IGNORED;
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

#include "stubs/rulesets/stubVisibilityProperty.h"

StatisticsProperty::StatisticsProperty() : m_script(0)
{
}

StatisticsProperty::~StatisticsProperty()
{
}

void StatisticsProperty::install(LocatedEntity * ent, const std::string & name)
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

#include "stubs/rulesets/stubTransientProperty.h"
#include "stubs/rulesets/stubLocation.h"

#include "stubs/rulesets/stubBaseWorld.h"

bool_config_register::bool_config_register(bool & var,
                                           const char * section,
                                           const char * setting,
                                           const char * help)
{
}

CommSocket::CommSocket(boost::asio::io_service & svr) : m_io_service(svr) { }

CommSocket::~CommSocket()
{
}

#include "stubs/common/stubEntityKit.h"
#include "stubs/server/stubEntityFactory.h"


Root atlasType(const std::string & name,
               const std::string & parent,
               bool abstract)
{
    return Atlas::Objects::Root();
}

#define STUB_Inheritance_getClass
const Atlas::Objects::Root& Inheritance::getClass(const std::string & parent)
{
    return noClass;
}

#define STUB_Inheritance_addChild
TypeNode * Inheritance::addChild(const Root & obj)
{
    return new TypeNode(obj->getId());
}


#include "stubs/common/stubInheritance.h"



#include "stubs/common/stubMonitors.h"
#include "stubs/common/stubPropertyManager.h"
#include "stubs/common/stubShaker.h"

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

#include "stubs/common/stubVariable.h"
#include "stubs/server/stubBuildid.h"

const char * const CYPHESIS = "cyphesis";

static const char * DEFAULT_INSTANCE = "cyphesis";

std::string instance(DEFAULT_INSTANCE);
int timeoffset = 0;
bool database_flag = false;


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
