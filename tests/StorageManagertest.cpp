// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "server/StorageManager.h"

#include "server/WorldRouter.h"

#include "rulesets/Entity.h"

#include <cassert>

class TestStorageManager : public StorageManager
{
  public:
    TestStorageManager(WorldRouter&w) : StorageManager(w) { }

    
    void test_entityInserted(Entity * e) {
        entityInserted(e);
    }
    void test_entityUpdated(Entity * e) {
        entityUpdated(e);
    }

    void test_encodeProperty(PropertyBase * p, std::string & s) {
        encodeProperty(p, s);
    }
    void test_restoreProperties(Entity * e) {
        restoreProperties(e);
    }

    void test_insertEntity(Entity * e) {
        insertEntity(e);
    }
    void test_updateEntity(Entity * e) {
        updateEntity(e);
    }
    void test_restoreChildren(Entity * e) {
        restoreChildren(e);
    }


};

int main()
{
    {
        WorldRouter world;

        StorageManager store(world);
    }

    {
        WorldRouter world;

        StorageManager store(world);

        store.initWorld();
    }

    {
        WorldRouter world;

        StorageManager store(world);

        store.restoreWorld();
    }

    {
        WorldRouter world;

        StorageManager store(world);

        store.tick();
    }

    {
        WorldRouter world;

        TestStorageManager store(world);

        store.test_entityInserted(new Entity("1", 1));
    }

    {
        WorldRouter world;

        TestStorageManager store(world);

        store.test_entityUpdated(new Entity("1", 1));
    }

    {
        WorldRouter world;

        TestStorageManager store(world);

        std::string val;

        // store.test_encodeProperty(0, val);
    }

    {
        WorldRouter world;

        TestStorageManager store(world);

        store.test_restoreProperties(new Entity("1", 1));
    }

    {
        WorldRouter world;

        TestStorageManager store(world);

        store.test_insertEntity(new Entity("1", 1));
    }

    {
        WorldRouter world;

        TestStorageManager store(world);

        store.test_updateEntity(new Entity("1", 1));
    }

    {
        WorldRouter world;

        TestStorageManager store(world);

        store.test_restoreChildren(new Entity("1", 1));
    }



    return 0;
}

// stubs

#include "server/EntityBuilder.h"

#include "rulesets/Script.h"

#include "modules/EntityRef.h"
#include "modules/Location.h"

#include "common/const.h"
#include "common/Database.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/PropertyManager.h"
#include "common/Variable.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cstdlib>

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

struct OpQueEntry {
    Operation op;
    Entity & from;

    explicit OpQueEntry(const Operation & o, Entity & f);
    OpQueEntry(const OpQueEntry & o);
    ~OpQueEntry();

    const Operation & operator*() const {
        return op;
    }

    Atlas::Objects::Operation::RootOperationData * operator->() const {
        return op.get();
    }
};

OpQueEntry::OpQueEntry(const Operation & o, Entity & f) : op(o), from(f)
{
    from.incRef();
}

OpQueEntry::OpQueEntry(const OpQueEntry & o) : op(o.op), from(o.from)
{
    from.incRef();
}

OpQueEntry::~OpQueEntry()
{
    from.decRef();
}

WorldRouter::WorldRouter() : BaseWorld(*new Entity(consts::rootWorldId,
                                                   consts::rootWorldIntId)),
                             m_entityCount(1)
          
{
}

WorldRouter::~WorldRouter()
{
}

Entity * WorldRouter::addEntity(Entity * ent)
{
    return 0;
}

Entity * WorldRouter::addNewEntity(const std::string & typestr,
                                   const RootEntity & attrs)
{
    return 0;
}

int WorldRouter::createSpawnPoint(const MapType & data, Entity * ent)
{
    return 0;
}

int WorldRouter::getSpawnList(Atlas::Message::ListType & data)
{
    return 0;
}

Entity * WorldRouter::spawnNewEntity(const std::string & name,
                                     const std::string & type,
                                     const RootEntity & desc)
{
    return 0;
}

Task * WorldRouter::newTask(const std::string & name, Character & owner)
{
    return 0;
}

Task * WorldRouter::activateTask(const std::string & tool,
                                 const std::string & op,
                                 const std::string & target,
                                 Character & owner)
{
    return 0;
}

void WorldRouter::message(const Operation & op, Entity & ent)
{
}

bool WorldRouter::idle(int sec, int usec)
{
    return false;
}

Entity * WorldRouter::findByName(const std::string & name)
{
    return 0;
}

Entity * WorldRouter::findByType(const std::string & type)
{
    return 0;
}

ArithmeticScript * WorldRouter::newArithmetic(const std::string & name,
                                              Entity * owner)
{
    return 0;
}

void WorldRouter::addPerceptive(Entity * perceptive)
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
    return 0;
}

Script noScript;

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

void Entity::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
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

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
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
               m_script(&noScript), m_type(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
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

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
}

void LocatedEntity::merge(const MapType & ent)
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

void Location::addToMessage(MapType & omap) const
{
}

Location::Location() : m_loc(0)
{
}

Location::Location(LocatedEntity * rf, const Point3D & pos)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

Script::Script()
{
}

/// \brief Script destructor
Script::~Script()
{
}

bool Script::operation(const std::string & opname,
                       const Atlas::Objects::Operation::RootOperation & op,
                       OpVector & res)
{
   return false;
}

void Script::hook(const std::string & function, LocatedEntity * entity)
{
}

EntityRef::EntityRef(Entity* e) : m_inner(e)
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

int Location::readFromMessage(const MapType & msg)
{
    return 0;
}

Database * Database::m_instance = NULL;

Database * Database::instance()
{
    if (m_instance == NULL) {
        m_instance = new Database();
    }
    return m_instance;
}

Database::Database() : m_rule_db("rules"),
                       m_queryInProgress(false),
                       m_connection(NULL)
{
}

const DatabaseResult Database::selectProperties(const std::string & id)
{
    return DatabaseResult(0);
}

const DatabaseResult Database::selectEntities(const std::string & loc)
{
    return DatabaseResult(0);
}

bool Database::encodeObject(const MapType & o,
                            std::string & data)
{
    return true;
}

bool Database::decodeMessage(const std::string & data,
                             MapType &o)
{
    return true;
}

int Database::insertEntity(const std::string & id,
                           const std::string & loc,
                           const std::string & type,
                           int seq,
                           const std::string & value)
{
    return 0;
}

int Database::updateEntity(const std::string & id,
                           int seq,
                           const std::string & value)
{
    return 0;
}

int Database::dropEntity(long id)
{
    return 0;
}

int Database::insertProperties(const std::string & id,
                               const KeyValues & tuples)
{
    return 0;
}

int Database::updateProperties(const std::string & id,
                               const KeyValues & tuples)
{
    return 0;
}

const char * DatabaseResult::const_iterator::column(const char * column) const
{
    return "";
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
template class Variable<const char *>;
template class Variable<std::string>;

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

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(Entity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

long forceIntegerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        abort();
    }

    return intId;
}

void log(LogLevel lvl, const std::string & msg)
{
}

bool database_flag = true;

namespace consts {

  // Id of root world entity
  const char * rootWorldId = "0";
  // Integer id of root world entity
  const long rootWorldIntId = 0L;

}
