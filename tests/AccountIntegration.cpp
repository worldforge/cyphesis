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

#include "TestBase.h"

#include "server/Account.h"
#include "server/EntityBuilder.h"
#include "server/EntityRuleHandler.h"
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

class TestCommSocket : public CommSocket
{
  public:
    TestCommSocket() : CommSocket(*(boost::asio::io_service*)0)
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

        Inheritance* m_inheritance;
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

Atlas::Objects::Root composeDeclaration(std::string class_name, std::string parent, Atlas::Message::MapType rawAttributes) {

    Atlas::Objects::Root decl;
    decl->setObjtype("class");
    decl->setId(class_name);
    decl->setParent(parent);

    Atlas::Message::MapType composed;
    for (const auto& entry : rawAttributes) {
        composed[entry.first] = Atlas::Message::MapType{
            {"default",    entry.second},
            {"visibility", "public"}
        };
    }

    decl->setAttr("attributes", composed);
    return decl;
};


void Accountintegration::setup()
{
    m_inheritance = new Inheritance();
    m_time = new SystemTime;
    EntityBuilder::init();
    auto entityRuleHandler = new EntityRuleHandler(EntityBuilder::instance());

    m_world = new WorldRouter(*m_time);

    m_server = new ServerRouting(*m_world, "noruleset", "unittesting",
                         "1", 1, "2", 2);

    m_tc = new TestCommSocket();
    m_c = new Connection(*m_tc, *m_server, "addr", "3", 3);
    m_ac = new TestAccount(m_c, "user", "password", "4", 4);

    std::string dependent, reason;

    {
        auto decl = composeDeclaration("thing", "game_entity", {});
        std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;
        entityRuleHandler->install(decl->getId(), decl->getParent(), decl, dependent, reason, changes);
    }
}

void Accountintegration::teardown()
{
    delete m_ac;
    delete m_world;
    EntityBuilder::del();
    delete m_inheritance;
}

void Accountintegration::test_addNewCharacter()
{
    Anonymous new_char;
    LocatedEntity * chr = m_ac->addNewCharacter("thing", new_char,
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
    Anonymous new_char;
    LocatedEntity * chr = m_ac->addNewCharacter("thing", new_char,
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
    LocatedEntity * chr = m_ac->addNewCharacter("thing", new_char,
                                                RootEntity());
    BBox newBox(WFMath::Point<3>(-0.5, 0.0, -0.5),
                WFMath::Point<3>(-0.5, 2.0, -0.5));
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

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}

// stubs

#include "server/ArithmeticBuilder.h"
#include "server/EntityFactory.h"
#include "server/ArchetypeFactory.h"
#include "server/Juncture.h"
#include "server/Persistence.h"
#include "server/Player.h"
#include "server/Ruleset.h"
#include "server/TeleportProperty.h"

#include "rulesets/AreaProperty.h"
#include "rulesets/AtlasProperties.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/BiomassProperty.h"
#include "rulesets/BurnSpeedProperty.h"
#include "rulesets/DecaysProperty.h"
#include "rulesets/CalendarProperty.h"
#include "rulesets/EntityProperty.h"
#include "rulesets/ExternalProperty.h"
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
#include "rulesets/SuspendedProperty.h"
#include "rulesets/SpawnerProperty.h"
#include "rulesets/ImmortalProperty.h"
#include "rulesets/RespawningProperty.h"
#include "rulesets/DefaultLocationProperty.h"
#include "rulesets/DomainProperty.h"
#include "rulesets/LimboProperty.h"
#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Stackable.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/PropertyFactory.h"
#include "common/system.h"
#include "common/TypeNode.h"
#include "common/Variable.h"


#include "rulesets/PythonScriptFactory.h"

#define STUB_PythonScriptFactory_PythonScriptFactory
template<>
PythonScriptFactory<LocatedEntity>::PythonScriptFactory(const std::string & p,
                                                        const std::string & t) :
    PythonClass(p, t, nullptr)
{
}

template <>
int PythonScriptFactory<LocatedEntity>::setup()
{
    return load();
}


#include "stubs/rulesets/stubSpawnProperty.h"
#include "stubs/rulesets/stubRespawningProperty.h"
#include "stubs/rulesets/stubImmortalProperty.h"
#include "stubs/rulesets/stubTerrainModProperty.h"
#include "stubs/rulesets/stubTerrainProperty.h"
#include "stubs/rulesets/stubDecaysProperty.h"
#include "stubs/rulesets/stubBurnSpeedProperty.h"
#include "stubs/rulesets/stubBiomassProperty.h"
#include "stubs/rulesets/stubSpawnerProperty.h"
#include "stubs/rulesets/stubBBoxProperty.h"
#include "stubs/rulesets/stubDefaultLocationProperty.h"
#include "stubs/rulesets/stubLimboProperty.h"
#include "stubs/rulesets/stubDomainProperty.h"
#include "stubs/rulesets/stubSuspendedProperty.h"
#include "stubs/rulesets/stubProxyMind.h"
#include "stubs/rulesets/stubBaseMind.h"
#include "stubs/rulesets/stubMemEntity.h"
#include "stubs/rulesets/stubMemMap.h"
#include "stubs/rulesets/stubModeProperty.h"
#include "stubs/rulesets/stubCreator.h"
#include "stubs/rulesets/stubPropelProperty.h"
#include "stubs/rulesets/stubQuaternionProperty.h"
#include "stubs/rulesets/stubDensityProperty.h"
#include "stubs/rulesets/stubAngularFactorProperty.h"
#include "stubs/rulesets/stubGeometryProperty.h"
#include "stubs/rulesets/stubVector3Property.h"
#include "stubs/rulesets/stubOutfitProperty.h"
#include "stubs/rulesets/stubPythonScriptFactory.h"
#include "stubs/rulesets/stubPythonClass.h"
#include "stubs/rulesets/stubInternalProperties.h"
#include "stubs/rulesets/stubEntityProperty.h"
#include "stubs/rulesets/stubSolidProperty.h"
#include "stubs/rulesets/stubPerceptionSightProperty.h"

#include "stubs/server/stubRuleHandler.h"
#include "stubs/server/stubExternalMindsManager.h"
#include "stubs/server/stubExternalMindsConnection.h"
#include "stubs/server/stubPlayer.h"
#include "stubs/common/stubOperationsDispatcher.h"
#include "stubs/modules/stubWorldTime.h"
#include "stubs/common/stubCustom.h"
#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"
#include "stubs/common/stubProperty.h"

#include "stubs/server/stubTaskRuleHandler.h"
#include "stubs/server/stubArchetypeRuleHandler.h"
#include "stubs/server/stubOpRuleHandler.h"
#include "stubs/server/stubPropertyRuleHandler.h"

template class OperationsDispatcher<LocatedEntity>;
template class OpQueEntry<LocatedEntity>;

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


Plant::Plant(const std::string& id, long idInt)
:Thing::Thing(id, idInt)
{
}

Plant::~Plant(){}

void Plant::NourishOperation(const Operation & op, OpVector &)
{
}

void Plant::TickOperation(const Operation & op, OpVector &)
{
}

void Plant::TouchOperation(const Operation & op, OpVector &)
{
}

Stackable::Stackable(const std::string& id, long idInt)
:Thing::Thing(id, idInt)
{
}

Stackable::~Stackable(){}

void Stackable::CombineOperation(const Operation & op, OpVector &)
{
}

void Stackable::DivideOperation(const Operation & op, OpVector &)
{
}

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

Persistence * Persistence::m_instance = nullptr;

Persistence::Persistence() : m_db(*(Database*)0)
{
}

Persistence * Persistence::instance()
{
    if (m_instance == nullptr) {
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

void Persistence::addCharacter(const Account &, const LocatedEntity &)
{
}

void Persistence::delCharacter(const std::string &)
{
}

#include "stubs/server/stubRuleset.h"

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

PropertyKit::~PropertyKit()
{
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

StatusProperty * StatusProperty::copy() const
{
    return 0;
}

void StatusProperty::apply(LocatedEntity * owner)
{
}

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

#include "stubs/server/stubBuildid.h"
#include "stubs/rulesets/stubPedestrian.h"
#include "stubs/rulesets/stubMovement.h"

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

#define STUB_ArchetypeFactory_newEntity
LocatedEntity* ArchetypeFactory::newEntity(const std::string & id, long intId, const Atlas::Objects::Entity::RootEntity & attributes, LocatedEntity* location)
{
    return new Entity(id, intId);
}

#include "stubs/server/stubArchetypeFactory.h"

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
TypeNode* Inheritance::addChild(const Atlas::Objects::Root & obj)
{
    return new TypeNode(obj->getId());
}

#include "stubs/common/stubInheritance.h"

Shaker::Shaker()
{
}

std::string Shaker::generateSalt(size_t length)
{
    return std::string("x", length * 2);
}

#include "stubs/common/stubTypeNode.h"

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
std::string assets_directory = "";
sigc::signal<void> python_reload_scripts;
