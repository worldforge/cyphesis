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

#include "server/StorageManager.h"

#include "server/WorldRouter.h"

#include <cassert>

int main()
{
    {
        WorldRouter world;

        StorageManager store(world);
    }

    {
        WorldRouter world;

        StorageManager store(world);
    }


    return 0;
}

// stubs

#include "server/EntityBuilder.h"

#include "rulesets/Entity.h"

#include "modules/EntityRef.h"
#include "modules/Location.h"

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

WorldRouter::WorldRouter() : BaseWorld(*(Entity*)0),
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

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return 0;
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
