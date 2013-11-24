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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/StorageManager.h"

#include "server/WorldRouter.h"
#include "server/MindInspector.h"

#include "rulesets/Entity.h"
#include "rulesets/Character.h"
#include "rulesets/MindProperty.h"

#include "common/SystemTime.h"

#include <cassert>
using Atlas::Message::Element;

class TestStorageManager : public StorageManager
{
  public:
    TestStorageManager(WorldRouter&w) : StorageManager(w) { }

    
    void test_entityInserted(LocatedEntity * e) {
        entityInserted(e);
    }
    void test_entityUpdated(LocatedEntity * e) {
        entityUpdated(e);
    }

    void test_encodeProperty(PropertyBase * p, std::string & s) {
        encodeProperty(p, s);
    }
    void test_restoreProperties(LocatedEntity * e) {
        restoreProperties(e);
    }

    void test_insertEntity(LocatedEntity * e) {
        insertEntity(e);
    }
    void test_updateEntity(LocatedEntity * e) {
        updateEntity(e);
    }
    void test_restoreChildren(LocatedEntity * e) {
        restoreChildren(e);
    }


};

int main()
{
    {
        SystemTime time;
        WorldRouter world(time);

        StorageManager store(world);
    }

    {
        SystemTime time;
        WorldRouter world(time);

        StorageManager store(world);

        store.initWorld();
    }

    {
        SystemTime time;
        WorldRouter world(time);

        StorageManager store(world);

        store.restoreWorld();
    }

    {
        SystemTime time;
        WorldRouter world(time);

        StorageManager store(world);

        store.tick();
    }

    {
        SystemTime time;
        WorldRouter world(time);

        TestStorageManager store(world);

        store.test_entityInserted(new Entity("1", 1));
    }

    {
        SystemTime time;
        WorldRouter world(time);

        TestStorageManager store(world);

        store.test_entityUpdated(new Entity("1", 1));
    }

    {
        SystemTime time;
        WorldRouter world(time);

        TestStorageManager store(world);

        std::string val;

        // store.test_encodeProperty(0, val);
    }

    {
        SystemTime time;
        WorldRouter world(time);

        TestStorageManager store(world);

        store.test_restoreProperties(new Entity("1", 1));
    }

    {
        SystemTime time;
        WorldRouter world(time);

        TestStorageManager store(world);

        store.test_insertEntity(new Entity("1", 1));
    }

    {
        SystemTime time;
        WorldRouter world(time);

        TestStorageManager store(world);

        store.test_updateEntity(new Entity("1", 1));
    }

    {
        SystemTime time;
        WorldRouter world(time);

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
#include "common/SystemTime.h"
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

WorldRouter::WorldRouter(const SystemTime &) :
      BaseWorld(*new Entity(consts::rootWorldId, consts::rootWorldIntId)),
      m_entityCount(1)
          
{
}

WorldRouter::~WorldRouter()
{
}

LocatedEntity * WorldRouter::addEntity(LocatedEntity * ent)
{
    return 0;
}

LocatedEntity * WorldRouter::addNewEntity(const std::string & typestr,
                                          const RootEntity & attrs)
{
    return 0;
}

void WorldRouter::delEntity(LocatedEntity * obj)
{
}

int WorldRouter::createSpawnPoint(const MapType & data, LocatedEntity * ent)
{
    return 0;
}

int WorldRouter::removeSpawnPoint(LocatedEntity *)
{
    return 0;
}

int WorldRouter::getSpawnList(Atlas::Message::ListType & data)
{
    return 0;
}

LocatedEntity * WorldRouter::spawnNewEntity(const std::string & name,
                                            const std::string & type,
                                            const RootEntity & desc)
{
    return 0;
}

int WorldRouter::moveToSpawn(const std::string & name,
                        Location& location)
{
    return 0;
}


Task * WorldRouter::newTask(const std::string & name, LocatedEntity & owner)
{
    return 0;
}

Task * WorldRouter::activateTask(const std::string & tool,
                                 const std::string & op,
                                 LocatedEntity * target,
                                 LocatedEntity & owner)
{
    return 0;
}

void WorldRouter::message(const Operation & op, LocatedEntity & ent)
{
}

bool WorldRouter::idle(const SystemTime &)
{
    return false;
}

LocatedEntity * WorldRouter::findByName(const std::string & name)
{
    return 0;
}

LocatedEntity * WorldRouter::findByType(const std::string & type)
{
    return 0;
}

ArithmeticScript * WorldRouter::newArithmetic(const std::string & name,
                                              LocatedEntity * owner)
{
    return 0;
}

void WorldRouter::addPerceptive(LocatedEntity * perceptive)
{
}

void WorldRouter::resumeWorld()
{
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
                                         const BaseWorld &) const
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

void Character::RelayOperation(const Operation & op, OpVector &)
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
    return 0;
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

void Entity::onContainered(const LocatedEntity*)
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

void LocatedEntity::onContainered(const LocatedEntity*)
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

int PropertyManager::installFactory(const std::string & type_name,
                                    const Atlas::Objects::Root & type_desc,
                                    PropertyKit * factory)
{
    return 0;
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

int Database::encodeObject(const MapType & o,
                           std::string & data)
{
    return 0;
}

int Database::decodeMessage(const std::string & data,
                            MapType &o)
{
    return 0;
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
                           const std::string & location_data,
                           const std::string & location)
{
    return 0;
}

int Database::updateEntityWithoutLoc(const std::string & id,
                           int seq,
                           const std::string & location_data)
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

int Database::registerThoughtsTable()
{
    return 0;
}
const DatabaseResult Database::selectThoughts(const std::string & loc)
{
    return DatabaseResult(0);
}
int Database::replaceThoughts(const std::string & id,
                     const std::vector<std::string>& thoughts)
{
    return 0;
}

int Database::launchNewQuery()
{
    return 0;
}
int Database::clearPendingQuery()
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

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

MindInspector::MindInspector() :
        m_serial(0)
{
}

MindInspector::~MindInspector()
{
}

void MindInspector::queryEntityForThoughts(const std::string& entityId)
{
}

void MindInspector::relayResponseReceived(const Operation& op,
        const std::string& entityId)
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

template <typename T>
Property<T>::Property(unsigned int flags) :
                      PropertyBase(flags)
{
}

template <typename T>
int Property<T>::get(Atlas::Message::Element & e) const
{
    return 0;
}

// The following two are obsolete.
template <typename T>
void Property<T>::add(const std::string & s,
                               Atlas::Message::MapType & ent) const
{
}

template <typename T>
void Property<T>::add(const std::string & s,
                               const Atlas::Objects::Entity::RootEntity & ent) const
{
}

template <typename T>
void Property<T>::set(const Atlas::Message::Element & e)
{
}

template <typename T>
Property<T> * Property<T>::copy() const
{
    return new Property<T>(*this);
}


template class Property<MapType>;


bool MindProperty::isMindEnabled() const {
    return false;
}

MindProperty::MindProperty() : m_factory(0)
{
}

MindProperty::~MindProperty()
{
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

namespace Atlas { namespace Objects { namespace Operation {
int THINK_NO = -1;
} } }
