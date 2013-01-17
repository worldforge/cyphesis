// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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

#include "TestWorld.h"
#include "TestPropertyManager.h"

#include "common/Property.h"

#include "rulesets/World.h"
#include "rulesets/Python_API.h"

#include "server/EntityBuilder.h"

int main(int argc, char ** argv)
{
    TestPropertyManager tpm;
    // database_flag = false;

    Entity * thing = new Thing("1", 1);

    // Check no flags are set
    assert(thing->getFlags() == 0);
    // Check test attribute is not there
    assert(!thing->hasAttr("test_attr1"));
    // Check test property not there
    assert(thing->getProperty("test_attr1") == 0);
    assert(thing->modProperty("test_attr1") == 0);
    // Set the flag that this entity is clean
    thing->setFlags(entity_clean);
    // Check the flags are no longer clear
    assert(thing->getFlags() != 0);
    // Check the entity_clean flag is set
    assert(thing->getFlags() & entity_clean);
    // Check entity_clean is the only flag set
    assert((thing->getFlags() & ~entity_clean) == 0);
    // Set the flag that this entity is queued
    thing->setFlags(entity_queued);
    // Check the entity_queued flag is set
    assert(thing->getFlags() & entity_queued);
    // Check the entity_clean flag is still set
    assert(thing->getFlags() & entity_clean);
    
    thing->setAttr("test_attr1", 1);

    // Check entity_clean is no longer set
    assert((thing->getFlags() & entity_clean) == 0);
    // Check the attribute is there
    assert(thing->hasAttr("test_attr1"));
    // Check it is there as a property
    assert(thing->getProperty("test_attr1") != 0);
    assert(thing->modProperty("test_attr1") != 0);
    // Check it is there as an integer property
    assert(thing->modPropertyType<int>("test_attr1") != 0);
    // Check it is not available as any other type.
    assert(thing->modPropertyType<double>("test_attr1") == 0);
    assert(thing->modPropertyType<std::string>("test_attr1") == 0);

    Property<int> * int_prop = thing->modPropertyType<int>("test_attr1");
    assert(int_prop != 0);
    // assert(int_prop->flags() & flag_unsent);
    // TODO(alriddoch) make sure flags behave fully correctly, and signals
    // are emitted as required.

    // TODO(alriddoch) add coverage for calling requirePropertyClass() when
    // a property of the wrong type already exists.
}

// stubs

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}

#include "rulesets/AtlasProperties.h"
#include "rulesets/Domain.h"
#include "rulesets/Motion.h"

#include "common/const.h"
#include "common/log.h"

using Atlas::Objects::Entity::RootEntity;

namespace Atlas { namespace Objects { namespace Operation {

int ACTUATE_NO = -1;
int ATTACK_NO = -1;
int DROP_NO = -1;
int EAT_NO = -1;
int NOURISH_NO = -1;
int PICKUP_NO = -1;
int TICK_NO = -1;
int UPDATE_NO = -1;

} } }

ContainsProperty::ContainsProperty(LocatedEntitySet & data) :
      PropertyBase(per_ephem), m_data(data)
{
}

int ContainsProperty::get(Atlas::Message::Element & e) const
{
    return 0;
}

void ContainsProperty::set(const Atlas::Message::Element & e)
{
}

void ContainsProperty::add(const std::string & s, const RootEntity & ent) const
{
}

ContainsProperty * ContainsProperty::copy() const
{
    return 0;
}

IdProperty::IdProperty(const std::string & data) : PropertyBase(per_ephem),
                                                   m_data(data)
{
}

int IdProperty::get(Atlas::Message::Element & e) const
{
    return 0;
}

void IdProperty::set(const Atlas::Message::Element & e)
{
}

void IdProperty::add(const std::string & key,
                     Atlas::Message::MapType & ent) const
{
}

void IdProperty::add(const std::string & key, const RootEntity & ent) const
{
}

IdProperty * IdProperty::copy() const
{
    return 0;
}

Motion::Motion(Entity & body) : m_entity(body), m_serialno(0),
                                m_collision(false), m_collEntity(0),
                                m_collisionTime(0.f)
{
}

Motion::~Motion()
{
}

float Motion::checkCollisions()
{
    return consts::move_tick;
}

bool Motion::resolveCollision()
{
    return false;
}

void Motion::setMode(const std::string & mode)
{
    m_mode = mode;
    // FIXME Re-configure stuff, and possible schedule an update?
}

void Motion::adjustPostion()
{
}

Operation * Motion::genUpdateOperation()
{
    return 0;
}

Operation * Motion::genMoveOperation()
{
    return 0;
}

Domain * Domain::m_instance = 0;

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

Location::Location() :
    m_simple(true), m_solid(true),
    m_boxSize(consts::minBoxSize),
    m_squareBoxSize(consts::minSqrBoxSize),
    m_loc(0)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Location::addToMessage(Atlas::Message::MapType & omap) const
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

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    return 0;
}

LocatedEntity * BaseWorld::getEntity(long id) const
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

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
}

WFMath::CoordType squareDistance(const Point3D & u, const Point3D & v)
{
    return 1.f;
}

template <typename FloatT>
int fromStdVector(Point3D & p, const std::vector<FloatT> & vf)
{
    if (vf.size() != 3) {
        return -1;
    }
    p[0] = vf[0];
    p[1] = vf[1];
    p[2] = vf[2];
    p.setValid();
    return 0;
}

template <>
int fromStdVector<double>(Point3D & p, const std::vector<double> & vf)
{
    return 0;
}

template <>
int fromStdVector<double>(Vector3D & v, const std::vector<double> & vf)
{
    return 0;
}

void log(LogLevel lvl, const std::string & msg)
{
}
