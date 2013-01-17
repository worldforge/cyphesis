// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

#include "allOperations.h"
#include "TestBase.h"

#include "rulesets/Thing.h"

#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Update;

class testThing : public Thing {
  public:
    testThing(const std::string & id, long intId) :
        Thing(id, intId) { }
    using Thing::updateProperties;
};

static const std::string testName("bob");
static const std::string testNewName("fred");

class ThingupdatePropertiestest : public Cyphesis::TestBase
{
  protected:
    Thing * m_thing;
    Property<std::string> * m_name;

  public:
    ThingupdatePropertiestest();

    void setup();
    void teardown();

    void test_update();
};

ThingupdatePropertiestest::ThingupdatePropertiestest()
{
    ADD_TEST(ThingupdatePropertiestest::test_update);
}

void ThingupdatePropertiestest::setup()
{
    m_name = new Property<std::string>(flag_unsent);
    m_name->data() = testName;

    m_thing = new Thing("1", 1);
    m_thing->setProperty("name", m_name);
}

void ThingupdatePropertiestest::teardown()
{
    delete m_thing;
}

void ThingupdatePropertiestest::test_update()
{
    ASSERT_EQUAL(m_name->flags() & flag_unsent, flag_unsent);

    Update u;
    OpVector res;
   
    m_thing->updateProperties(u, res);

    // The flag marking the property has been cleared
    ASSERT_EQUAL(m_name->flags() & flag_unsent, 0u);

    // The update operation should not have actually changed the name
    // at all
    ASSERT_EQUAL(m_name->data(), testName);

    // The result should be a Sight op
    ASSERT_EQUAL(res.size(), 1u);

    const Operation & result = res.front();

    ASSERT_EQUAL(result->getClassNo(), Atlas::Objects::Operation::SIGHT_NO);

    // The result argument should be a Set op
    ASSERT_EQUAL(result->getArgs().size(), 1u);

    const Operation & result_inner = smart_dynamic_cast<Operation>(result->getArgs().front());

    ASSERT_TRUE(result_inner.isValid());

    ASSERT_EQUAL(result_inner->getClassNo(), Atlas::Objects::Operation::SET_NO);
    ASSERT_EQUAL(result_inner->getArgs().size(), 1u);

    auto set_arg = smart_dynamic_cast<RootEntity>(result_inner->getArgs().front());

    // Make sure the name on the sight set argument has been set to the
    // name of the entity, not the name in the Update op.
    ASSERT_TRUE(!set_arg->isDefaultName());
    ASSERT_EQUAL(set_arg->getName(), testName);
}

int main()
{
    ThingupdatePropertiestest t;

    return t.run();
}

// stubs

#include "rulesets/Domain.h"
#include "rulesets/Motion.h"

#include "common/BaseWorld.h"
#include "common/const.h"
#include "common/log.h"

namespace Atlas { namespace Objects { namespace Operation {
int ACTUATE_NO = -1;
int ATTACK_NO = -1;
int DROP_NO = -1;
int EAT_NO = -1;
int NOURISH_NO = -1;
int PICKUP_NO = -1;
int SETUP_NO = -1;
int TICK_NO = -1;
int UPDATE_NO = -1;
} } }

Motion::Motion(Entity & body) : m_entity(body), m_serialno(0),
                                m_collision(false)
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
    return true;
}

void Motion::setMode(const std::string & mode)
{
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

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return m_properties[name] = prop;
}

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

void Entity::installHandler(int class_no, Handler handler)
{
}

void Entity::installDelegate(int class_no, const std::string & delegate)
{
}

void Entity::sendWorld(const Operation & op)
{
}

void Entity::onContainered()
{
}

void Entity::onUpdated()
{
}

Domain * Entity::getMovementDomain()
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

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
}

void LocatedEntity::changeContainer(LocatedEntity * new_loc)
{
    assert(m_location.m_loc != 0);
    assert(m_location.m_loc->m_contains != 0);
    m_location.m_loc->m_contains->erase(this);
    if (m_location.m_loc->m_contains->empty()) {
        m_location.m_loc->onUpdated();
    }
    new_loc->makeContainer();
    bool was_empty = new_loc->m_contains->empty();
    new_loc->m_contains->insert(this);
    if (was_empty) {
        new_loc->onUpdated();
    }
    assert(m_location.m_loc->checkRef() > 0);
    m_location.m_loc->decRef();
    m_location.m_loc = new_loc;
    m_location.m_loc->incRef();
    assert(m_location.m_loc->checkRef() > 0);

    onContainered();
}

void LocatedEntity::merge(const MapType & ent)
{
}

float Domain::constrainHeight(LocatedEntity * parent,
                              const Point3D & pos,
                              const std::string & mode)
{
    return 0.f;
}

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
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

Location::Location() : m_loc(0)
{
}

Location::Location(LocatedEntity * rf, const Point3D & pos)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Location::addToMessage(MapType & omap) const
{
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

template <typename FloatT>
int fromStdVector(Vector3D & v, const std::vector<FloatT> & vf)
{
    if (vf.size() != 3) {
        return -1;
    }
    v[0] = vf[0];
    v[1] = vf[1];
    v[2] = vf[2];
    v.setValid();
    return 0;
}

template int fromStdVector<double>(Point3D & p, const std::vector<double> & vf);
template int fromStdVector<double>(Vector3D & v, const std::vector<double> & vf);

float squareDistance(const Location & self, const Location & other)
{
    return 1.f;
}

float squareDistance(const Point3D & u, const Point3D & v)
{
    return 1.f;
}
