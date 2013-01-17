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

#include "server/WorldRouter.h"

#include "server/ArithmeticBuilder.h"
#include "server/EntityBuilder.h"
#include "server/SpawnEntity.h"

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

using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Tick;

static bool stub_deny_newid = false;

class WorldRoutertest : public Cyphesis::TestBase
{
    WorldRouter * test_world;
  public:
    WorldRoutertest();

    void setup();
    void teardown();

    void test_constructor();
    void test_addNewEntity_unknown();
    void test_addNewEntity_thing();
    void test_addNewEntity_idfail();
    void test_addEntity();
    void test_getOperationFromQueue();
    void test_addEntity_tick();
    void test_addEntity_tick_get();
    void test_spawnNewEntity_unknown();
    void test_spawnNewEntity_thing();
    void test_createSpawnPoint();
    void test_delEntity();
    void test_delEntity_world();
};

WorldRoutertest::WorldRoutertest()
{
    ADD_TEST(WorldRoutertest::test_constructor);
    ADD_TEST(WorldRoutertest::test_addNewEntity_unknown);
    ADD_TEST(WorldRoutertest::test_addNewEntity_thing);
    ADD_TEST(WorldRoutertest::test_addNewEntity_idfail);
    ADD_TEST(WorldRoutertest::test_addEntity);
    ADD_TEST(WorldRoutertest::test_getOperationFromQueue);
    ADD_TEST(WorldRoutertest::test_addEntity_tick);
    ADD_TEST(WorldRoutertest::test_addEntity_tick_get);
    ADD_TEST(WorldRoutertest::test_spawnNewEntity_unknown);
    ADD_TEST(WorldRoutertest::test_spawnNewEntity_thing);
    ADD_TEST(WorldRoutertest::test_createSpawnPoint);
    ADD_TEST(WorldRoutertest::test_delEntity);
    ADD_TEST(WorldRoutertest::test_delEntity_world);
}

void WorldRoutertest::setup()
{
    test_world = new WorldRouter(SystemTime());
}

void WorldRoutertest::teardown()
{
    delete test_world;

    EntityBuilder::del();
}

void WorldRoutertest::test_constructor()
{
}

void WorldRoutertest::test_addNewEntity_unknown()
{

    LocatedEntity * ent1 = test_world->addNewEntity("__no_such_type__",
                                                    Anonymous());
    assert(ent1 == 0);
}

void WorldRoutertest::test_addNewEntity_thing()
{
    LocatedEntity * ent1 = test_world->addNewEntity("thing", Anonymous());
    assert(ent1 != 0);
}


void WorldRoutertest::test_addNewEntity_idfail()
{
    stub_deny_newid = true;

    LocatedEntity * ent1 = test_world->addNewEntity("thing", Anonymous());
    assert(ent1 == 0);

    stub_deny_newid = false;
}

void WorldRoutertest::test_addEntity()
{
    std::string id;
    long int_id = newId(id);

    Entity * ent2 = new Entity(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_loc = &test_world->m_gameWorld;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);
}

void WorldRoutertest::test_getOperationFromQueue()
{
    test_world->getOperationFromQueue();
}

void WorldRoutertest::test_addEntity_tick()
{
    std::string id;
    long int_id = newId(id);

    Entity * ent2 = new Entity(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_loc = &test_world->m_gameWorld;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);

    Tick tick;
    tick->setFutureSeconds(0);
    tick->setTo(ent2->getId());
    test_world->message(tick, *ent2);
}


void WorldRoutertest::test_addEntity_tick_get()
{
    std::string id;
    long int_id = newId(id);

    Entity * ent2 = new Entity(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_loc = &test_world->m_gameWorld;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);

    Tick tick;
    tick->setFutureSeconds(0);
    tick->setTo(ent2->getId());
    test_world->message(tick, *ent2);

    test_world->getOperationFromQueue();
}

void WorldRoutertest::test_spawnNewEntity_unknown()
{
    LocatedEntity * ent3 = test_world->spawnNewEntity("__no_spawn__",
                                                      "thing",
                                                      Anonymous());
    assert(ent3 == 0);
}

void WorldRoutertest::test_spawnNewEntity_thing()
{
    LocatedEntity * ent3 = test_world->spawnNewEntity("bob",
                                                      "thing",
                                                      Anonymous());
    assert(ent3 == 0);
}

void WorldRoutertest::test_createSpawnPoint()
{
    std::string id;
    long int_id = newId(id);

    Entity * ent2 = new Entity(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_loc = &test_world->m_gameWorld;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);

    {
        int ret;

        Atlas::Message::MapType spawn_data;
        ret = test_world->createSpawnPoint(spawn_data, ent2);
        assert(ret == -1);

        spawn_data["name"] = 1;
        ret = test_world->createSpawnPoint(spawn_data, ent2);
        assert(ret == -1);

        spawn_data["name"] = "bob";
        ret = test_world->createSpawnPoint(spawn_data, ent2);
        assert(ret == 0);

        ret = test_world->createSpawnPoint(spawn_data, ent2);
        assert(ret == 0);
    }

    {
        Atlas::Message::ListType spawn_repr;
        test_world->getSpawnList(spawn_repr);
        assert(!spawn_repr.empty());
        assert(spawn_repr.size() == 1u);
    }

    LocatedEntity * ent3 = test_world->spawnNewEntity("bob",
                                                      "permitted_non_existant",
                                                      Anonymous());
    assert(ent3 == 0);

    ent3 = test_world->spawnNewEntity("bob",
                                      "thing",
                                      Anonymous());
    assert(ent3 != 0);
}

void WorldRoutertest::test_delEntity()
{
    std::string id;
    long int_id = newId(id);

    Entity * ent2 = new Entity(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_loc = &test_world->m_gameWorld;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);

    test_world->delEntity(ent2);
    test_world->delEntity(&test_world->m_gameWorld);
}

void WorldRoutertest::test_delEntity_world()
{
    test_world->delEntity(&test_world->m_gameWorld);
}

int main()
{
    WorldRoutertest t;

    return t.run();
}

// Stubs

int timeoffset = 0;

namespace consts {
const char * rootWorldId = "0";
const long rootWorldIntId = 0L;
}

namespace Atlas { namespace Objects { namespace Operation {
int TICK_NO = -1;
}}}

World::World(const std::string & id, long intId) : Thing(id, intId)
{
}

World::~World()
{
}

void World::EatOperation(const Operation & op, OpVector & res)
{
}

void World::LookOperation(const Operation & op, OpVector & res)
{
}

void World::MoveOperation(const Operation & op, OpVector & res)
{
}

void World::DeleteOperation(const Operation & op, OpVector & res)
{
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

Domain * Domain::m_instance = new Domain();

Domain::Domain() : m_refCount(0)
{
}

Domain::~Domain()
{
}

Domain * Entity::getMovementDomain()
{
    return Domain::instance();
}

float Domain::constrainHeight(LocatedEntity * parent,
                              const Point3D & pos,
                              const std::string & mode)
{
    return 0.f;
}

void Domain::tick(double t)
{
    
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

void Entity::addToEntity(const RootEntity & ent) const
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
    return 0;
}

void Entity::installHandler(int class_no, Handler handler)
{
}

void Entity::installDelegate(int class_no, const std::string & delegate)
{
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

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
    // Necessary to avoid memory leaks
    if (m_location.m_loc != 0) {
        m_location.m_loc->decRef();
    }
    delete m_contains;
}

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
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

Location::Location() : m_loc(0)
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

static inline float sqr(float x)
{
    return x * x;
}

float squareDistance(const Point3D & u, const Point3D & v)
{
    return (sqr(u.x() - v.x()) + sqr(u.y() - v.y()) + sqr(u.z() - v.z()));
}

static bool distanceFromAncestor(const Location & self,
                                 const Location & other, Point3D & c)
{
    if (&self == &other) {
        return true;
    }

    if (other.m_loc == NULL) {
        return false;
    }

    if (other.orientation().isValid()) {
        c = c.toParentCoords(other.m_pos, other.orientation());
    } else {
        static const Quaternion identity(1, 0, 0, 0);
        c = c.toParentCoords(other.m_pos, identity);
    }

    return distanceFromAncestor(self, other.m_loc->m_location, c);
}

static bool distanceToAncestor(const Location & self,
                               const Location & other, Point3D & c)
{
    c.setToOrigin();
    if (distanceFromAncestor(self, other, c)) {
        return true;
    } else if ((self.m_loc != 0) &&
               distanceToAncestor(self.m_loc->m_location, other, c)) {
        if (self.orientation().isValid()) {
            c = c.toLocalCoords(self.m_pos, self.orientation());
        } else {
            static const Quaternion identity(1, 0, 0, 0);
            c = c.toLocalCoords(self.m_pos, identity);
        }
        return true;
    }
    log(ERROR, "Broken entity hierarchy doing distance calculation");
    if (self.m_loc != 0) {
        std::cerr << "Self(" << self.m_loc->getId() << "," << self.m_loc << ")"
                  << std::endl << std::flush;
    }
    if (other.m_loc != 0) {
        std::cerr << "Other(" << other.m_loc->getId() << "," << other.m_loc << ")"
                  << std::endl << std::flush;
    }
     
    return false;
}

float sqrMag(const Point3D & p)
{
    return p.x() * p.x() + p.y() * p.y() + p.z() * p.z();
}

float squareDistance(const Location & self, const Location & other)
{
    Point3D dist;
    distanceToAncestor(self, other, dist);
    return sqrMag(dist);
}

static long idGenerator = 2;

long newId(std::string & id)
{
    if (stub_deny_newid) {
        return -1;
    }
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

void Monitors::watch(const::std::string & name, VariableBase * monitor)
{
    delete monitor;
}

ArithmeticBuilder * ArithmeticBuilder::m_instance = 0;

ArithmeticBuilder * ArithmeticBuilder::instance()
{
    return 0;
}

ArithmeticScript * ArithmeticBuilder::newArithmetic(const std::string &,
                                                    LocatedEntity *)
{
    return 0;
}

EntityBuilder * EntityBuilder::m_instance = NULL;

EntityBuilder::EntityBuilder()
{
}

EntityBuilder::~EntityBuilder()
{
}

LocatedEntity * EntityBuilder::newEntity(const std::string & id, long intId,
                                         const std::string & type,
                                         const RootEntity & attributes,
                                         const BaseWorld & world) const
{
    if (type == "thing") {
        Entity * e = new Entity(id, intId);
        e->m_location.m_loc = &world.m_gameWorld;
        e->m_location.m_pos = Point3D(0,0,0);
        return e;
    }
    return 0;
}

Task * EntityBuilder::newTask(const std::string & name, LocatedEntity & owner) const
{
    return 0;
}

Task * EntityBuilder::activateTask(const std::string & tool,
                                   const std::string & op,
                                   LocatedEntity * target,
                                   LocatedEntity & owner) const
{
    return 0;
}

SpawnEntity::SpawnEntity(LocatedEntity *)
{
}

int SpawnEntity::setup(const MapType &)
{
    return 0;
}

int SpawnEntity::spawnEntity(const std::string & type,
                             const RootEntity & dsc)
{
    if (type == "thing" || type == "permitted_non_existant") {
        return 0;
    }
    return -1;
}

int SpawnEntity::populateEntity(LocatedEntity * ent,
                                const RootEntity & dsc,
                                OpVector & res)
{
    return -1;
}

int SpawnEntity::addToMessage(MapType & msg) const
{
    return -1;
}

Spawn::~Spawn()
{
}
