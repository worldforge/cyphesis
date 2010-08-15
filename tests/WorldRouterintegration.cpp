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

#include "server/WorldRouter.h"

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

class TestWorldRouter : public WorldRouter
{
  public:
    Operation test_getOperationFromQueue() {
        return getOperationFromQueue();
    }

    void test_delEntity(Entity * e) {
        delEntity(e);
    }
};

int main()
{
    database_flag = false;

    new Domain;

    TestWorldRouter * test_world = new TestWorldRouter;

    Entity * ent1 = test_world->addNewEntity("__no_such_type__", Anonymous());
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

    test_world->test_getOperationFromQueue();

    Tick tick;
    tick->setFutureSeconds(0);
    tick->setTo(ent2->getId());
    test_world->message(tick, *ent2);

    test_world->test_getOperationFromQueue();

    {
        Atlas::Message::MapType spawn_data;
        test_world->createSpawnPoint(spawn_data, ent2);

        spawn_data["name"] = 1;
        test_world->createSpawnPoint(spawn_data, ent2);

        spawn_data["name"] = "bob";
        test_world->createSpawnPoint(spawn_data, ent2);

        test_world->createSpawnPoint(spawn_data, ent2);
    }
    {
        Atlas::Message::ListType spawn_repr;
        test_world->getSpawnList(spawn_repr);
        assert(!spawn_repr.empty());
    }

    Entity * ent3 = test_world->spawnNewEntity("__no_spawn__",
                                               "character",
                                               Anonymous());
    assert(ent3 == 0);

    ent3 = test_world->spawnNewEntity("bob",
                                      "character",
                                      Anonymous());
    assert(ent3 == 0);

    {
        Atlas::Message::MapType spawn_data;
        spawn_data["name"] = "bob";
        spawn_data["character_types"] = Atlas::Message::ListType(1, "spiddler");
        test_world->createSpawnPoint(spawn_data, ent2);
    }

    ent3 = test_world->spawnNewEntity("bob",
                                      "spiddler",
                                      Anonymous());
    assert(ent3 == 0);

    {
        Atlas::Message::MapType spawn_data;
        spawn_data["name"] = "bob";
        spawn_data["character_types"] = Atlas::Message::ListType(1, "character");
        test_world->createSpawnPoint(spawn_data, ent2);
    }

    ent3 = test_world->spawnNewEntity("bob",
                                      "character",
                                      Anonymous());
    assert(ent3 != 0);

    {
        Atlas::Message::MapType spawn_data;
        spawn_data["name"] = "bob";
        spawn_data["character_types"] = Atlas::Message::ListType(1, "character");
        spawn_data["contains"] = Atlas::Message::ListType(1, "thing");
        test_world->createSpawnPoint(spawn_data, ent2);
    }

    Entity * ent4 = test_world->spawnNewEntity("bob",
                                               "character",
                                               Anonymous());
    assert(ent4 != 0);

    test_world->test_delEntity(&test_world->m_gameWorld);
    test_world->test_delEntity(ent4);
    ent4 = 0;

    delete test_world;

    return 0;
}

#if 0
// Stubs

int timeoffset = 0;

namespace consts {
const char * rootWorldId = "0";
const long rootWorldIntId = 0L;
}

namespace Atlas { namespace Objects { namespace Operation {
int TICK_NO = -1;
}}}

World::World(const std::string & id, long intId) : World_parent(id, intId)
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
       Thing_parent(id, intId)
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
        LocatedEntity(id, intId), m_motion(0), m_flags(0)
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

void Entity::externalOperation(const Operation & op)
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

void Entity::setAttr(const std::string & name,
                     const Atlas::Message::Element & attr)
{
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    return 0;
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
               m_script(0), m_type(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
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

bool LocatedEntity::getAttr(const std::string & name, Atlas::Message::Element & attr) const
{
    return false;
}

bool LocatedEntity::getAttrType(const std::string & name,
                                Atlas::Message::Element & attr,
                                int type) const
{
    return false;
}

void LocatedEntity::setAttr(const std::string & name, const Atlas::Message::Element & attr)
{
    return;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

void LocatedEntity::onContainered()
{
}

void LocatedEntity::onUpdated()
{
}

Location::Location()
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

const float squareDistance(const Location & self, const Location & other)
{
    Point3D dist;
    distanceToAncestor(self, other, dist);
    return sqrMag(dist);
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

BaseWorld::BaseWorld(Entity & gw) : m_gameWorld(gw)
{
}

BaseWorld::~BaseWorld()
{
}

Entity * BaseWorld::getEntity(const std::string & id) const
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

Entity * BaseWorld::getEntity(long id) const
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

Inheritance::Inheritance()
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
}

EntityBuilder * EntityBuilder::m_instance = NULL;

EntityBuilder::EntityBuilder(BaseWorld & w) : m_world(w)
{
}

EntityBuilder::~EntityBuilder()
{
}

Entity * EntityBuilder::newEntity(const std::string & id, long intId,
                                  const std::string & type,
                                  const RootEntity & attributes) const
{
    if (type == "thing") {
        Entity * e = new Entity(id, intId);
        e->m_location.m_loc = &m_world.m_gameWorld;
        e->m_location.m_pos = Point3D(0,0,0);
        return e;
    }
    return 0;
}

Task * EntityBuilder::newTask(const std::string & name, Character & owner) const
{
    return 0;
}

Task * EntityBuilder::activateTask(const std::string & tool,
                                   const std::string & op,
                                   const std::string & target,
                                   Character & owner) const
{
    return 0;
}

SpawnEntity::SpawnEntity(Entity * e, const MapType & data)
{
}

int SpawnEntity::spawnEntity(const std::string & type,
                             const RootEntity & dsc)
{
    return -1;
}

int SpawnEntity::populateEntity(Entity * ent,
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
#endif // 0
