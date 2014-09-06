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
        restorePropertiesRecursively(e);
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

#include "stubs/server/stubWorldRouter.h"
#include "stubs/modules/stubLocation.h"
#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubCharacter.h"
#include "stubs/rulesets/stubThing.h"
#include "stubs/common/stubBaseWorld.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cstdlib>

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

struct OpQueEntry {
    Operation op;
    LocatedEntity* from;

    explicit OpQueEntry(const Operation & o, LocatedEntity & f);
    ~OpQueEntry();

    const Operation & operator*() const {
        return op;
    }

    Atlas::Objects::Operation::RootOperationData * operator->() const {
        return op.get();
    }

    bool operator<(const OpQueEntry& right) const {
        return op->getSeconds() < right->getSeconds();
    }

    bool operator>(const OpQueEntry& right) const {
        return op->getSeconds() > right->getSeconds();
    }
};


OpQueEntry::OpQueEntry(const Operation & o, LocatedEntity & f) : op(o), from(&f)
{
    from->incRef();
}

OpQueEntry::~OpQueEntry()
{
    from->decRef();
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

void LocatedEntity::removeDelegate(int class_no, const std::string & delegate)
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
