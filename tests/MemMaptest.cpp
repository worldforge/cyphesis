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

#include "rulesets/MemMap.h"

#include "rulesets/MemEntity.h"
#include "rulesets/Script.h"

#include "common/Inheritance.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cstdlib>

#include <cassert>

class TestMemMap : public MemMap {
  public:
    TestMemMap(Script * & s) : MemMap(s) { }

    MemEntity * test_addId(const std::string & id, long int_id) {
        return addId(id, int_id);
    }
};

int main()
{
    Script * s = &noScript;
    TestMemMap * mm = new TestMemMap(s);

    mm->test_addId("2", 2);

    OpVector res;
    mm->sendLooks(res);

    mm->del("2");

    delete mm;
    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}

// stubs

Script noScript;

MemEntity::MemEntity(const std::string & id, long intId) :
           LocatedEntity(id, intId), m_visible(false), m_lastSeen(0.)
{
}

MemEntity::~MemEntity()
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

Location::Location()
{
}

int Location::readFromEntity(const Atlas::Objects::Entity::RootEntity & ent)
{
    return 0;
}

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance()
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

TypeNode::TypeNode() : m_parent(0)
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


