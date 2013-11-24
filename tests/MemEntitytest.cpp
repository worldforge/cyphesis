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

#include "rulesets/MemEntity.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/SolidProperty.h"
#include "rulesets/InternalProperties.h"

#include <cassert>

int main()
{
    MemEntity * me = new MemEntity("1", 1);

    delete me;
    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}

// stubs

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

PropertyBase::PropertyBase(unsigned int){}
PropertyBase::~PropertyBase(){}
BBoxProperty::BBoxProperty(){}

int BBoxProperty::get(Atlas::Message::Element & ent) const
{
    return 0;
}

void BBoxProperty::set(const Atlas::Message::Element & ent)
{
}

BBoxProperty * BBoxProperty::copy() const
{
    return 0;
}

void BBoxProperty::apply(LocatedEntity * owner)
{
}

void BBoxProperty::add(const std::string & key,
                       Atlas::Message::MapType & map) const
{
}

void BBoxProperty::add(const std::string & key,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
}

SimpleProperty::SimpleProperty(){}

int SimpleProperty::get(Atlas::Message::Element & ent) const
{
    return 0;
}

void SimpleProperty::set(const Atlas::Message::Element & ent)
{
}

SimpleProperty * SimpleProperty::copy() const
{
    return 0;
}

void SimpleProperty::apply(LocatedEntity * owner)
{
}

SolidProperty::SolidProperty(){}

int SolidProperty::get(Atlas::Message::Element & ent) const
{
    return 0;
}

void SolidProperty::set(const Atlas::Message::Element & ent)
{
}

SolidProperty * SolidProperty::copy() const
{
    return 0;
}

void SolidProperty::apply(LocatedEntity * owner)
{
}


SoftProperty::SoftProperty(Atlas::Message::Element const&){}

int SoftProperty::get(Atlas::Message::Element & ent) const
{
    return 0;
}

void SoftProperty::set(const Atlas::Message::Element & ent)
{
}

SoftProperty * SoftProperty::copy() const
{
    return 0;
}

void PropertyBase::install(LocatedEntity *, const std::string & name)
{
}

void PropertyBase::remove(LocatedEntity *)
{
}

void PropertyBase::apply(LocatedEntity *)
{
}

void PropertyBase::add(const std::string & s,
                       Atlas::Message::MapType & ent) const
{
}

void PropertyBase::add(const std::string & s,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
}

HandlerResult PropertyBase::operation(LocatedEntity *,
                                      const Operation &,
                                      OpVector & res)
{
    return OPERATION_IGNORED;
}
