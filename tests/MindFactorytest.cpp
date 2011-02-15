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

#include "rulesets/MindFactory.h"

#include "rulesets/BaseMind.h"

#include "common/TypeNode.h"

#include <cassert>

static const char * etype = "settler";

int main()
{
    TypeNode * tn = new TypeNode();
    tn->name() = etype;

    MindFactory * mf = MindFactory::instance();

    mf->addMindType(etype, "testmod");

    mf->newMind("1", 1, tn);

    tn->name() = etype;

    mf->newMind("2", 2, tn);

    return 0;
}

// stubs

MemMap::MemMap(Script *& s) : m_script(s)
{
}

BaseMind::BaseMind(const std::string & id, long intId) :
          MemEntity(id, intId), m_map(m_script)
{
}

BaseMind::~BaseMind()
{
}

void BaseMind::SightOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::SoundOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::AppearanceOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::DisappearanceOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::UnseenOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::operation(const Operation & op, OpVector & res)
{
}

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

Location::Location() : m_loc(0)
{
}

void WorldTime::initTimeInfo()
{
}

DateTime::DateTime(int t)
{
}

void Create_PyMind(BaseMind * mind, const std::string & package,
                                    const std::string & type)
{
}

TypeNode::TypeNode() : m_parent(0)
{
}

TypeNode::~TypeNode()
{
}
