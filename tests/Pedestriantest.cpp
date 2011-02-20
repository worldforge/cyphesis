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

#include "rulesets/Pedestrian.h"

#include "rulesets/Entity.h"
#include "rulesets/Domain.h"
#include "rulesets/TerrainProperty.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

class TestWorld : public BaseWorld {
  public:
    explicit TestWorld(Entity & w) : BaseWorld(w) {
    }

    virtual bool idle(int, int) { return false; }
    virtual Entity * addEntity(Entity * ent) { 
        return 0;
    }
    virtual Entity * addNewEntity(const std::string &,
                                  const Atlas::Objects::Entity::RootEntity &) {
        return 0;
    }
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         Entity *) { return 0; }
    int getSpawnList(Atlas::Message::ListType & data) { return 0; }
    Entity * spawnNewEntity(const std::string & name,
                            const std::string & type,
                            const Atlas::Objects::Entity::RootEntity & desc) {
        return addNewEntity(type, desc);
    }
    virtual Task * newTask(const std::string &, Character &) { return 0; }
    virtual Task * activateTask(const std::string &, const std::string &,
                                const std::string &, Character &) { return 0; }
    virtual ArithmeticScript * newArithmetic(const std::string &, Entity *) {
        return 0;
    }
    virtual void message(const Atlas::Objects::Operation::RootOperation & op,
                         Entity & ent) { }
    virtual Entity * findByName(const std::string & name) { return 0; }
    virtual Entity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(Entity *) { }
};

int main()
{
    Entity * e = new Entity("1", 1);
    Entity * wrld = new Entity("0", 0);

    e->m_location.m_loc = wrld;
    e->m_location.m_loc->makeContainer();
    assert(e->m_location.m_loc->m_contains != 0);
    e->m_location.m_loc->m_contains->insert(e);

    TestWorld test_world(*wrld);

    Pedestrian * p = new Pedestrian(*e);

    p->getTickAddition(Point3D(0,0,0), Vector3D(1,0,0));

    p->setTarget(Point3D(2,0,0));

    p->getTickAddition(Point3D(0,0,0), Vector3D(1,0,0));

    Location loc;
    p->getUpdatedLocation(loc);

    e->m_location.m_velocity = Vector3D(1,0,0);
    p->getUpdatedLocation(loc);

    e->m_location.m_pos = Point3D(1,0,0);
    p->getUpdatedLocation(loc);

    e->m_location.m_pos = Point3D(2,0,0);
    p->getUpdatedLocation(loc);

    e->m_location.m_pos = Point3D(3,0,0);
    p->getUpdatedLocation(loc);

    p->generateMove(loc);

    delete p;
    return 0;
}

// stubs

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

void Entity::onContainered()
{
}

void Entity::onUpdated()
{
}

Domain * Entity::getMovementDomain()
{
    return Domain::instance();
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

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
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

Domain * Domain::m_instance = new Domain();

Domain::Domain() : m_refCount(0)
{
}

Domain::~Domain()
{
}

float Domain::constrainHeight(LocatedEntity * parent,
                              const Point3D & pos,
                              const std::string & mode)
{
    return 8.f;
}

BaseWorld * BaseWorld::m_instance = 0;

/// \brief BaseWorld constructor.
///
/// Protected as BaseWorld is a base class.
/// This constructor registers the instance created as the singleton, and
/// in debug mode ensures that an instance has not already been created.
/// @param gw the top level in-game entity in the world.
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

Entity * BaseWorld::getEntity(long id) const
{
    return 0;
}

Location::Location() : m_loc(0)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

float squareDistance(const Point3D & u, const Point3D & v)
{
    return 1.f;
}
