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

#include "rulesets/MemMap.h"

#include "rulesets/MemEntity.h"
#include "rulesets/Script.h"

#include "common/Inheritance.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cstdlib>

#include <cassert>

using Atlas::Objects::Entity::Anonymous;

class MemMaptest : public Cyphesis::TestBase
{
  private:
    Script * m_script;
    MemMap * m_memMap;
  public:
    MemMaptest();

    void setup();
    void teardown();

    void test_addId();
    void test_sendLooks();
    void test_del();
    void test_addEntity();
    void test_readEntity();
};

MemMaptest::MemMaptest()
{
    ADD_TEST(MemMaptest::test_addId);
    ADD_TEST(MemMaptest::test_sendLooks);
    ADD_TEST(MemMaptest::test_del);
    ADD_TEST(MemMaptest::test_addEntity);
    ADD_TEST(MemMaptest::test_readEntity);
}

void MemMaptest::setup()
{
    m_script = 0;
    m_memMap = new MemMap(m_script);
}

void MemMaptest::teardown()
{
    delete m_memMap;
}

void MemMaptest::test_addId()
{
    m_memMap->addId("2", 2);
}

void MemMaptest::test_sendLooks()
{
    OpVector res;
    m_memMap->sendLooks(res);
}

void MemMaptest::test_del()
{
    m_memMap->del("2");
}

void MemMaptest::test_addEntity()
{
    const std::string new_id("3");
    ASSERT_NULL(m_memMap->get(new_id));

    MemEntity * ent = new MemEntity(new_id, 3);
    ent->setType(MemMap::m_entity_type);
    m_memMap->addEntity(ent);

    ASSERT_NOT_NULL(m_memMap->get(new_id));
}

void MemMaptest::test_readEntity()
{
    const std::string new_id("3");

    Anonymous data;
    data->setParents(std::list<std::string>(1, "sample_type"));

    MemEntity * ent = new MemEntity(new_id, 3);
    ent->setType(MemMap::m_entity_type);

    m_memMap->readEntity(ent, data);
}

int main()
{
    MemMaptest t;

    return t.run();
}

// stubs

MemEntity::MemEntity(const std::string & id, long intId) :
           LocatedEntity(id, intId), m_lastSeen(0.)
{
}

MemEntity::~MemEntity()
{
}

void MemEntity::externalOperation(const Operation & op, Link &)
{
}

void MemEntity::operation(const Operation &, OpVector &)
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

void LocatedEntity::merge(const Atlas::Message::MapType & ent)
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

Location::Location() : m_loc(0)
{
}

int Location::readFromEntity(const Atlas::Objects::Entity::RootEntity & ent)
{
    return 0;
}

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance() : noClass(0)
{
}

const TypeNode * Inheritance::getType(const std::string & parent)
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

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
    }
    return *m_instance;
}

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

float squareDistance(const Point3D & u, const Point3D & v)
{
    return 1.f;
}

void log(LogLevel lvl, const std::string & msg)
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


