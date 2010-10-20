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

#include "PropertyCoverage.h"
#include "TestWorld.h"

#include "rulesets/Entity.h"
#include "rulesets/Character.h"

#include "common/log.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootOperation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Entity::Anonymous;

PropertyCoverage::PropertyCoverage(PropertyBase * pb) :
    m_prop(pb),
    m_tlve(new Entity("0", 0)),
    m_wrld(new TestWorld(*m_tlve)),
    m_ent(new Entity("1", 1))
{
    m_ent->m_location.m_loc = m_tlve;
    m_ent->m_location.m_pos = Point3D(1,0,0);

    m_tlve->m_contains = new LocatedEntitySet;
    m_tlve->m_contains->insert(m_ent);

    m_testData.push_back(23);
    m_testData.push_back(23.);
    m_testData.push_back("twenty_three");
    m_testData.push_back(ListType(1, 23));
    m_testData.push_back(ListType(1, 23.));
    m_testData.push_back(ListType(1, "twenty_three"));

    MapType map;
    map["one"] = 23;
    map["two"] = 23.;
    map["three"] = "twenty_three";
    map["four"] = ListType(1, 23);
    map["five"] = ListType(1, 23.);
    map["six"] = ListType(1, "twenty_three");

    m_testData.push_back(map);
}

PropertyCoverage::~PropertyCoverage()
{
    m_ent->m_location.m_loc = 0;
    delete m_ent;
    delete m_tlve;
    delete m_prop;
    delete m_wrld;
}

void PropertyCoverage::basicCoverage()
{
    Element val;

    // assert(pb->flags() == 4);

    m_prop->get(val);

    // FIXME cover all types inc. map
    ListType::const_iterator I = m_testData.begin();
    ListType::const_iterator Iend = m_testData.end();
    for (; I != Iend; ++I) {
        m_prop->set(*I);
    }

    m_prop->get(val);

    m_prop->install(m_ent);

    I = m_testData.begin();
    for (; I != Iend; ++I) {
        m_prop->set(*I);
        m_prop->apply(m_ent);
    }

    MapType map;
    m_prop->add("test_name", map);
    Anonymous ent;
    m_prop->add("test_name", ent);
}

Character * PropertyCoverage::createCharacterEntity()
{
    m_ent->m_location.m_loc = 0;
    delete m_ent;
    m_tlve->m_contains->clear();

    Character * chr = new Character("2", 2);
    m_ent = chr;
    m_ent->m_location.m_loc = m_tlve;
    m_ent->m_location.m_pos = Point3D(1,0,0);
    m_tlve->m_contains->insert(m_ent);

    return chr;
}

void PropertyCoverage::testDataAppend(const Element & o)
{
    m_testData.push_back(o);
}

// stubs

Character::Character(const std::string & id, long intId) :
           Character_parent(id, intId),
               m_movement(*(Movement*)0),
               m_task(0), m_mind(0), m_externalMind(0)
{
}

Character::~Character()
{
}

void Character::operation(const Operation & op, OpVector &)
{
}

void Character::externalOperation(const Operation & op)
{
}


void Character::ImaginaryOperation(const Operation & op, OpVector &)
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
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second;
    }
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

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(Entity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

Entity * BaseWorld::getEntity(const std::string & id) const
{
    return 0;
}

Location::Location() : m_loc(0)
{
}

void log(LogLevel lvl, const std::string & msg)
{
}
