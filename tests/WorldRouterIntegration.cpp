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

#include "server/WorldRouter.h"

#include "server/EntityBuilder.h"
#include "server/EntityRuleHandler.h"
#include "server/SpawnEntity.h"
#include "server/EntityFactory.h"

#include "rulesets/Domain.h"
#include "rulesets/World.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/id.h"
#include "common/Inheritance.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/SystemTime.h"
#include "common/Tick.h"
#include "common/Variable.h"

#include <Atlas/Objects/Anonymous.h>

#include <cstdio>
#include <cstdlib>

#include <cassert>
#include <server/Ruleset.h>
#include <rulesets/Character.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Tick;

class WorldRouterintegration : public Cyphesis::TestBase
{
  public:
    WorldRouterintegration();

    void setup();
    void teardown();

    void test_sequence();

        Inheritance* m_inheritance;
};

WorldRouterintegration::WorldRouterintegration()
{
    ADD_TEST(WorldRouterintegration::test_sequence);
}




void WorldRouterintegration::setup()
{
    m_inheritance = new Inheritance();
    EntityBuilder::init();


    class TestEntityRuleHandler : public EntityRuleHandler {
        public:
            explicit TestEntityRuleHandler(EntityBuilder * eb) : EntityRuleHandler(eb) {}

            int test_installEntityClass(const std::string & class_name,
                                        const std::string & parent,
                                        const Atlas::Objects::Root & class_desc,
                                        std::string & dependent,
                                        std::string & reason,
                                        EntityFactoryBase* factory)
            {
                std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;
                return installEntityClass(class_name, parent, class_desc, dependent, reason, factory, changes);
            }
    };

    auto entityRuleHandler = new TestEntityRuleHandler(EntityBuilder::instance());

    auto composeDeclaration = [](std::string class_name, std::string parent, Atlas::Message::MapType rawAttributes) {

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
    std::string dependent, reason;
    {
        auto decl = composeDeclaration("thing", "game_entity", {});
        entityRuleHandler->test_installEntityClass(decl->getId(), decl->getParent(), decl, dependent, reason, new EntityFactory<Thing>());
    }
    {
        auto decl = composeDeclaration("character", "thing", {});
        entityRuleHandler->test_installEntityClass(decl->getId(), decl->getParent(), decl, dependent, reason, new EntityFactory<Character>());
    }
}

void WorldRouterintegration::teardown()
{
    delete m_inheritance;
}

void WorldRouterintegration::test_sequence()
{
    database_flag = false;

    WorldRouter * test_world = new WorldRouter(SystemTime());

    LocatedEntity * ent1 = test_world->addNewEntity("__no_such_type__",
                                                    Anonymous());
    assert(ent1 == 0);

    ent1 = test_world->addNewEntity("thing", Anonymous());
    assert(ent1 != 0);

    std::string id;
    long int_id = newId(id);

    Entity * ent2 = new Thing(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_loc = &test_world->m_gameWorld;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);

    Tick tick;
    tick->setFutureSeconds(0);
    tick->setTo(ent2->getId());
    test_world->message(tick, *ent2);

    {
        MapType spawn_data;
        test_world->createSpawnPoint(spawn_data, ent2);
        ASSERT_EQUAL(test_world->m_spawns.size(), 0u);

        spawn_data["name"] = 1;
        test_world->createSpawnPoint(spawn_data, ent2);
        ASSERT_EQUAL(test_world->m_spawns.size(), 0u);

        ASSERT_TRUE(test_world->m_spawns.find("bob") ==
                    test_world->m_spawns.end());

        spawn_data["name"] = "bob";
        test_world->createSpawnPoint(spawn_data, ent2);
        ASSERT_EQUAL(test_world->m_spawns.size(), 1u);
        ASSERT_TRUE(test_world->m_spawns.find("bob") !=
                    test_world->m_spawns.end());

        test_world->createSpawnPoint(spawn_data, ent2);
        ASSERT_EQUAL(test_world->m_spawns.size(), 1u);
        ASSERT_TRUE(test_world->m_spawns.find("bob") !=
                    test_world->m_spawns.end());
    }
    {
        ASSERT_EQUAL(test_world->m_spawns.size(), 1u);
        Atlas::Message::ListType spawn_repr;
        test_world->getSpawnList(spawn_repr);
        assert(!spawn_repr.empty());
        ASSERT_EQUAL(spawn_repr.size(), 1u);
    }

    LocatedEntity * ent3 = test_world->spawnNewEntity("__no_spawn__",
                                                      "character",
                                                      Anonymous());
    assert(ent3 == 0);

    ent3 = test_world->spawnNewEntity("bob",
                                      "character",
                                      Anonymous());
    assert(ent3 == 0);

    {
        MapType spawn_data;
        spawn_data["name"] = "bob";
        MapType entity;
        entity["parent"] = "spiddler";
        MapType entities;
        entities["spiddler"] = entity;
        spawn_data["entities"] = entities;
        test_world->createSpawnPoint(spawn_data, ent2);
    }

    ent3 = test_world->spawnNewEntity("bob",
                                      "spiddler",
                                      Anonymous());
    assert(ent3 == 0);

    {
        MapType spawn_data;
        spawn_data["name"] = "bob";
        MapType entity;
        entity["parent"] = "character";
        MapType entities;
        entities["character"] = entity;
        spawn_data["entities"] = entities;
        test_world->createSpawnPoint(spawn_data, ent2);
    }

    ent3 = test_world->spawnNewEntity("bob",
                                      "character",
                                      Anonymous());
    assert(ent3 != 0);

    //TODO: test creation of archetypes
//    {
//        MapType spawn_data;
//        spawn_data["name"] = "bob";
//        MapType entity;
//        entity["parent"] = "character";
//        MapType entities;
//        entities["character"] = entity;
//        spawn_data["entities"] = entities;
//
//
//        spawn_data["character_types"] = Atlas::Message::ListType(1, "character");
//        spawn_data["contains"] = Atlas::Message::ListType(1, "thing");
//        test_world->createSpawnPoint(spawn_data, ent2);
//    }
//
//    LocatedEntity * ent4 = test_world->spawnNewEntity("bob",
//                                                      "character",
//                                                      Anonymous());
//    assert(ent4 != 0);
//
    test_world->delEntity(&test_world->m_gameWorld);
//    test_world->delEntity(ent4);
//    ent4 = 0;

    delete test_world;
}

int main()
{
    WorldRouterintegration t;

    return t.run();
}

// stubs

#include "server/EntityFactory.h"
#include "server/ArchetypeFactory.h"
#include "server/CorePropertyManager.h"

#include "rulesets/AreaProperty.h"
#include "rulesets/AtlasProperties.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/CalendarProperty.h"
#include "rulesets/EntityProperty.h"
#include "rulesets/ExternalProperty.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/TasksProperty.h"
#include "rulesets/TerrainProperty.h"
#include "rulesets/DomainProperty.h"

#include "rulesets/Character.h"
#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Stackable.h"
#include "rulesets/ExternalMind.h"
#include "rulesets/PythonArithmeticFactory.h"
#include "rulesets/Task.h"

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

#include "stubs/rulesets/stubBBoxProperty.h"
#include "stubs/rulesets/stubTasksProperty.h"
#include "stubs/rulesets/stubTerrainProperty.h"
#include "stubs/rulesets/stubDomainProperty.h"
#include "stubs/rulesets/stubProxyMind.h"
#include "stubs/rulesets/stubBaseMind.h"
#include "stubs/rulesets/stubMemEntity.h"
#include "stubs/rulesets/stubMemMap.h"
#include "stubs/rulesets/stubPropelProperty.h"
#include "stubs/rulesets/stubCreator.h"
#include "stubs/rulesets/stubOutfitProperty.h"
#include "stubs/rulesets/stubPythonClass.h"
#include "stubs/rulesets/stubPedestrian.h"
#include "stubs/rulesets/stubMovement.h"

#include "stubs/common/stubOperationsDispatcher.h"
#include "stubs/common/stubScriptKit.h"
#include "stubs/common/stubRouter.h"
#include "stubs/server/stubConnectableRouter.h"
#include "stubs/server/stubAccount.h"
#include "stubs/server/stubPlayer.h"
#include "stubs/server/stubExternalMindsManager.h"
#include "stubs/server/stubExternalMindsConnection.h"
#include "stubs/server/stubServerRouting.h"
#include "stubs/rulesets/stubEntityProperty.h"
#include "stubs/rulesets/stubPythonScriptFactory.h"

#include <Atlas/Objects/Operation.h>

CorePropertyManager::CorePropertyManager()
{
}

PropertyBase * CorePropertyManager::addProperty(const std::string & name,
                                                int type)
{
    return new Property<float>();
}

int CorePropertyManager::installFactory(const std::string & type_name,
                                        const Atlas::Objects::Root & type_desc,
                                        PropertyKit * factory)
{
    return 0;
}


#define STUB_ArchetypeFactory_newEntity
LocatedEntity* ArchetypeFactory::newEntity(const std::string & id, long intId, const Atlas::Objects::Entity::RootEntity & attributes, LocatedEntity* location)
{
    return new Entity(id, intId);
}

#include "stubs/server/stubArchetypeFactory.h"


class World;


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

ExternalProperty::ExternalProperty(ExternalMind * & data) : m_data(data)
{
}

int ExternalProperty::get(Element & val) const
{
    return 0;
}

void ExternalProperty::set(const Element & val)
{
}

void ExternalProperty::add(const std::string & s,
                         MapType & map) const
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


Task::Task(LocatedEntity & owner) : m_refCount(0), m_serialno(0),
                                    m_obsolete(false),
                                    m_progress(-1), m_rate(-1),
                                    m_owner(owner), m_script(0)
{
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


ContainsProperty::ContainsProperty(LocatedEntitySet & data) :
      PropertyBase(per_ephem), m_data(data)
{
}

int ContainsProperty::get(Element & e) const
{
    return 0;
}

void ContainsProperty::set(const Element & e)
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


ExternalMind::ExternalMind(LocatedEntity & e) : Router(e.getId(), e.getIntId()),
                                         m_link(0),
                                         m_entity(e),
                                         m_lossTime(0.)
{
}

void ExternalMind::externalOperation(const Operation & op, Link &)
{
}

void ExternalMind::linkUp(Link * c)
{
    m_link = c;
}

void ExternalMind::operation(const Operation & op, OpVector & res)
{
}

ArithmeticKit::~ArithmeticKit()
{
}


PythonArithmeticFactory::PythonArithmeticFactory(const std::string & package,
                                                 const std::string & name) :
                                                 PythonClass(package,
                                                             name,
                                                             0)
{
}

PythonArithmeticFactory::~PythonArithmeticFactory()
{
}

int PythonArithmeticFactory::setup()
{
    return 0;
}

ArithmeticScript * PythonArithmeticFactory::newScript(LocatedEntity * owner)
{
    return 0;
}
sigc::signal<void> python_reload_scripts;

#if 0

int timeoffset = 0;

namespace consts {
const char * rootWorldId = "0";
const long rootWorldIntId = 0L;
}

namespace Atlas { namespace Objects { namespace Operation {
int TICK_NO = -1;
}}}

SoftProperty::SoftProperty()
{
}

SoftProperty::SoftProperty(const Element & data) :
              PropertyBase(0), m_data(data)
{
}

int SoftProperty::get(Element & val) const
{
    val = m_data;
    return 0;
}

void SoftProperty::set(const Element & val)
{
}

SoftProperty * SoftProperty::copy() const
{
    return 0;
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
                       MapType & ent) const
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
void Property<int>::set(const Element & e)
{
    if (e.isInt()) {
        this->m_data = e.asInt();
    }
}

template<>
void Property<double>::set(const Element & e)
{
    if (e.isNum()) {
        this->m_data = e.asNum();
    }
}

template<>
void Property<std::string>::set(const Element & e)
{
    if (e.isString()) {
        this->m_data = e.String();
    }
}

template class Property<int>;
template class Property<double>;
template class Property<std::string>;

Router::Router(const std::string & id, long intId) : m_id(id),
                                                             m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(MapType & omap) const
{
}

void Router::addToEntity(const RootEntity & ent) const
{
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

void log(LogLevel lvl, const std::string & msg)
{
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

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
{
}

BaseWorld::~BaseWorld()
{
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    long intId = integerId(id);

    EntityDict::const_iterator I = m_eobjects.find(intId);
    if (I != m_eobjects.end()) {
        assert(I->second != 0);
        return I->second;
    } else {
        return 0;
    }
}

LocatedEntity * BaseWorld::getEntity(long id) const
{
    EntityDict::const_iterator I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second != 0);
        return I->second;
    } else {
        return 0;
    }
}

Inheritance * Inheritance::m_instance = nullptr;

Inheritance::Inheritance() : noClass(0)
{
}

Inheritance & Inheritance::instance()
{
    if (m_instance == nullptr) {
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
    o << m_variable;
}

template class Variable<int>;

Monitors * Monitors::m_instance = nullptr;

Monitors::Monitors()
{
}

Monitors::~Monitors()
{
}

Monitors * Monitors::instance()
{
    if (m_instance == nullptr) {
        m_instance = new Monitors();
    }
    return m_instance;
}

void Monitors::watch(const::std::string & name, VariableBase * monitor)
{
}
#endif // 0
