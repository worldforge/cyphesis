// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

#include "IGEntityExerciser.h"
#include "TestBase.h"
#include "allOperations.h"

#include "rulesets/Character.h"

#include "rulesets/AtlasProperties.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/Domain.h"
#include "rulesets/EntityProperty.h"
#include "rulesets/ExternalMind.h"
#include "rulesets/ExternalProperty.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/Pedestrian.h"
#include "rulesets/Script.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/Task.h"
#include "rulesets/TasksProperty.h"

#include "common/const.h"
#include "common/id.h"
#include "common/log.h"
#include "common/Link.h"
#include "common/Property_impl.h"
#include "common/TypeNode.h"

#include "stubs/rulesets/stubThing.h"
#include "stubs/rulesets/stubExternalProperty.h"
#include "stubs/rulesets/stubEntityProperty.h"
#include "stubs/rulesets/stubOutfitProperty.h"
#include "stubs/rulesets/stubTasksProperty.h"
#include "stubs/rulesets/stubTask.h"
#include "stubs/common/stubProperty.h"
#include "stubs/rulesets/stubSoftProperty.h"
#include "stubs/rulesets/stubContainsProperty.h"
#include "stubs/rulesets/stubStatusProperty.h"
#include "stubs/rulesets/stubBBoxProperty.h"
#include "stubs/rulesets/stubProxyMind.h"
#include "stubs/rulesets/stubBaseMind.h"
#include "stubs/rulesets/stubMemEntity.h"
#include "stubs/rulesets/stubMemMap.h"
#include "stubs/common/stubPropertyManager.h"
#include "stubs/rulesets/stubContainsProperty.h"

#include "stubs/common/stubLink.h"
#include "stubs/common/stubTypeNode.h"
#include "stubs/common/stubCustom.h"
#include "stubs/modules/stubLocation.h"
#include "stubs/modules/stubWorldTime.h"
#include "stubs/modules/stubDateTime.h"
#include "stubs/rulesets/stubScript.h"
#include "stubs/common/stubRouter.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/rulesets/stubMovement.h"
#include "stubs/rulesets/stubDomain.h"

#include <cstdlib>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

class TestLink : public Link
{
  public:
    TestLink(CommSocket & s, const std::string & id, long iid) :
        Link(s, id, iid)
    {
    }

    void externalOperation(const Operation & op, Link &)
    {
    }

    void operation(const Operation &, OpVector &)
    {
    }
};

class Charactertest : public Cyphesis::TestBase
{
  private:
    static Operation m_BaseWorld_message_called;
    static LocatedEntity * m_BaseWorld_message_called_from;

    Character * m_character;
    TypeNode * m_type;
  public:
    Charactertest();

    void setup();
    void teardown();

    void test_exerciser();
    void test_linkExternal();
    void test_linkExternal_mind();
    void test_linkExternal_linked();
    void test_linkExternal_linked_other();
    void test_unlinkExternal();
    void test_unlinkExternal_linked_other();
    void test_unlinkExternal_unlinked();
    void test_unlinkExternal_nomind();
    void test_filterExternalOperation();

    static void BaseWorld_message_called(const Operation & op, LocatedEntity &);
};

Operation Charactertest::m_BaseWorld_message_called(0);
LocatedEntity * Charactertest::m_BaseWorld_message_called_from(0);

void Charactertest::BaseWorld_message_called(const Operation & op,
                                             LocatedEntity & ent)
{
    m_BaseWorld_message_called = op;
    m_BaseWorld_message_called_from = &ent;
}

Charactertest::Charactertest()
{
    ADD_TEST(Charactertest::test_exerciser);
    ADD_TEST(Charactertest::test_linkExternal);
    ADD_TEST(Charactertest::test_linkExternal_mind);
    ADD_TEST(Charactertest::test_linkExternal_linked);
    ADD_TEST(Charactertest::test_linkExternal_linked_other);
    ADD_TEST(Charactertest::test_unlinkExternal);
    ADD_TEST(Charactertest::test_unlinkExternal_linked_other);
    ADD_TEST(Charactertest::test_unlinkExternal_unlinked);
    ADD_TEST(Charactertest::test_unlinkExternal_nomind);
    ADD_TEST(Charactertest::test_filterExternalOperation);
}

void Charactertest::setup()
{
    m_character = new Character("1", 1);
    m_type = new TypeNode("character");
    m_character->setType(m_type);

    m_BaseWorld_message_called = 0;
    m_BaseWorld_message_called_from = 0;
}

void Charactertest::teardown()
{
    delete m_character;
    delete m_type;
}

void Charactertest::test_exerciser()
{
    IGEntityExerciser ee(*m_character);

    // Throw an op of every type at the entity
    ee.runOperations();

    // Subscribe the entity to every class of op
    std::set<std::string> opNames;
    ee.addAllOperations(opNames);

    // Throw an op of every type at the entity again now it is subscribed
    ee.runOperations();
}

// The common case as a link up with a new ExternalMind object
void Charactertest::test_linkExternal()
{
    ASSERT_NULL(m_character->m_externalMind);

    Link * l = new TestLink(*(CommSocket*)0, "2", 2);
    
    int ret = m_character->linkExternal(l);
    ASSERT_EQUAL(ret, 0);

    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_TRUE(m_character->m_externalMind->isLinked())
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(l))
}

// An existing, non-linked ExternalMind should be left in place, and used
// for the link up.
void Charactertest::test_linkExternal_mind()
{
    ExternalMind * existing_mind = m_character->m_externalMind =
                                   new ExternalMind(*m_character);

    ASSERT_NOT_NULL(m_character->m_externalMind);
    ASSERT_TRUE(!m_character->m_externalMind->isLinked());

    Link * l = new TestLink(*(CommSocket*)0, "2", 2);
    
    int ret = m_character->linkExternal(l);
    ASSERT_EQUAL(ret, 0);

    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_EQUAL(m_character->m_externalMind, existing_mind);
    ASSERT_TRUE(m_character->m_externalMind->isLinked())
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(l))
}

// A link already done should be unaffected
void Charactertest::test_linkExternal_linked()
{
    ExternalMind * existing_mind = m_character->m_externalMind =
                                   new ExternalMind(*m_character);

    Link * l = new TestLink(*(CommSocket*)0, "2", 2);
    existing_mind->linkUp(l);

    ASSERT_NOT_NULL(m_character->m_externalMind);
    ASSERT_TRUE(m_character->m_externalMind->isLinked());
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(l));

    
    int ret = m_character->linkExternal(l);
    ASSERT_EQUAL(ret, -1);

    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_EQUAL(m_character->m_externalMind, existing_mind);
    ASSERT_TRUE(m_character->m_externalMind->isLinked())
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(l))
}

// An existing link should be unaffected, and linkup should fail
void Charactertest::test_linkExternal_linked_other()
{
    ExternalMind * existing_mind = m_character->m_externalMind =
                                   new ExternalMind(*m_character);
    Link * existing_link = new TestLink(*(CommSocket*)0, "2", 2);
    existing_mind->linkUp(existing_link);

    ASSERT_NOT_NULL(m_character->m_externalMind);
    ASSERT_TRUE(m_character->m_externalMind->isLinked());
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(existing_link));

    Link * l = new TestLink(*(CommSocket*)0, "2", 2);
    
    int ret = m_character->linkExternal(l);
    ASSERT_EQUAL(ret, -1);

    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_EQUAL(m_character->m_externalMind, existing_mind);
    ASSERT_TRUE(m_character->m_externalMind->isLinked())
    ASSERT_TRUE(!m_character->m_externalMind->isLinkedTo(l))
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(existing_link));
}

// Common case. Character has a link, and this removes it.
void Charactertest::test_unlinkExternal()
{
    ExternalMind * existing_mind = m_character->m_externalMind =
                                   new ExternalMind(*m_character);
    Link * existing_link = new TestLink(*(CommSocket*)0, "2", 2);
    existing_mind->linkUp(existing_link);

    ASSERT_NOT_NULL(m_character->m_externalMind);
    ASSERT_TRUE(m_character->m_externalMind->isLinked());
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(existing_link));
    
    int ret = m_character->unlinkExternal(existing_link);
    ASSERT_EQUAL(ret, 0);

    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_TRUE(!m_character->m_externalMind->isLinked())
    ASSERT_TRUE(m_BaseWorld_message_called.isValid());
    ASSERT_EQUAL(m_BaseWorld_message_called_from, m_character);
    ASSERT_EQUAL(m_BaseWorld_message_called->getClassNo(),
                 Atlas::Objects::Operation::MOVE_NO);
}

// Character has a link to another connection.
void Charactertest::test_unlinkExternal_linked_other()
{
    ExternalMind * existing_mind = m_character->m_externalMind =
                                   new ExternalMind(*m_character);
    Link * existing_link = new TestLink(*(CommSocket*)0, "2", 2);
    existing_mind->linkUp(existing_link);

    ASSERT_NOT_NULL(m_character->m_externalMind);
    ASSERT_TRUE(m_character->m_externalMind->isLinked());
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(existing_link));
    
    Link * l = new TestLink(*(CommSocket*)0, "2", 2);

    int ret = m_character->unlinkExternal(l);
    ASSERT_EQUAL(ret, -2);

    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_TRUE(m_character->m_externalMind->isLinked())
    ASSERT_TRUE(m_character->m_externalMind->isLinkedTo(existing_link))
}

// Check the case where the Character has no link
void Charactertest::test_unlinkExternal_unlinked()
{
    m_character->m_externalMind = new ExternalMind(*m_character);
    ASSERT_NOT_NULL(m_character->m_externalMind);
    ASSERT_TRUE(!m_character->m_externalMind->isLinked())

    Link * l = new TestLink(*(CommSocket*)0, "2", 2);
    
    int ret = m_character->unlinkExternal(l);
    ASSERT_EQUAL(ret, -1);

    ASSERT_NOT_NULL(m_character->m_externalMind)
    ASSERT_TRUE(!m_character->m_externalMind->isLinked())
}

void Charactertest::test_unlinkExternal_nomind()
{
    ASSERT_NULL(m_character->m_externalMind);

    Link * l = new TestLink(*(CommSocket*)0, "2", 2);
    
    int ret = m_character->unlinkExternal(l);
    ASSERT_EQUAL(ret, -1);

    ASSERT_NULL(m_character->m_externalMind)
}

void Charactertest::test_filterExternalOperation()
{
    // Dispatching a Talk external op from the character should result in
    // it being passed on to the world.

    Atlas::Objects::Operation::Talk op;
    op->setFrom(m_character->getId());

    m_character->filterExternalOperation(op);

    // BaseWorld::message should have been called from Enitty::sendWorld
    // with the Talk operation, modified to have TO set to the character.
    ASSERT_TRUE(m_BaseWorld_message_called.isValid());
    ASSERT_EQUAL(m_BaseWorld_message_called->getClassNo(),
                 Atlas::Objects::Operation::TALK_NO);
    ASSERT_TRUE(!m_BaseWorld_message_called->isDefaultTo());
    ASSERT_EQUAL(m_BaseWorld_message_called->getTo(), m_character->getId());
    ASSERT_NOT_NULL(m_BaseWorld_message_called_from);
    ASSERT_EQUAL(m_BaseWorld_message_called_from, m_character);
}

int main(int argc, char ** argv)
{
    Charactertest t;

    return t.run();
}

// stubs

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
    Charactertest::BaseWorld_message_called(op, ent);
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}


ExternalMind::ExternalMind(LocatedEntity & e) : Router(e.getId(), e.getIntId()),
                                         m_external(0),
                                         m_entity(e),
                                         m_lossTime(0.)
{
}

ExternalMind::~ExternalMind()
{
}

void ExternalMind::externalOperation(const Operation & op, Link &)
{
}

void ExternalMind::linkUp(Link * c)
{
    m_external = c;
}

void ExternalMind::operation(const Operation & op, OpVector & res)
{
}

Pedestrian::Pedestrian(LocatedEntity & body) : Movement(body)
{
}

Pedestrian::~Pedestrian()
{
}

double Pedestrian::getTickAddition(const Point3D & coordinates,
                                   const Vector3D & velocity) const
{
    return consts::basic_tick;
}

int Pedestrian::getUpdatedLocation(Location & return_location)
{
    return 1;
}

Operation Pedestrian::generateMove(const Location & new_location)
{
    Atlas::Objects::Operation::Move moveOp;
    return moveOp;
}


void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId)
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

void Entity::RelayOperation(const Operation &, OpVector &)
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

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return 0;
}

void Entity::installDelegate(int class_no, const std::string & delegate)
{
}

void Entity::removeDelegate(int class_no, const std::string & delegate)
{
}

Domain * Entity::getDomain()
{
    return 0;
}

const Domain * Entity::getDomain() const
{
    return 0;
}

void Entity::sendWorld(const Operation & op)
{
    BaseWorld::instance().message(op, *this);
}

void Entity::onContainered(const LocatedEntity*)
{
}

void Entity::onUpdated()
{
}

void Entity::setType(const TypeNode* t)
{
    m_type = t;
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

LocatedEntity& BaseWorld::getDefaultLocation() {
    return m_gameWorld;
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    long intId = integerId(id);

    EntityDict::const_iterator I = m_eobjects.find(intId);
    if (I != m_eobjects.end()) {
        assert(I->second != 0);
        return I->second;
    } else {
        return 0;
    }
}

LocatedEntity * BaseWorld::getEntity(long id) const
{
    EntityDict::const_iterator I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second != 0);
        return I->second;
    } else {
        return 0;
    }
}

double BaseWorld::getTime() const {
   return 0;
}




void log(LogLevel lvl, const std::string & msg)
{
}

void logEvent(LogEvent lev, const std::string & msg)
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

void addToEntity(const Vector3D & v, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = v[0];
    vd[1] = v[1];
    vd[2] = v[2];
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


template<class V>
const Quaternion quaternionFromTo(const V & from, const V & to)
{
    return Quaternion(1.f, 0.f, 0.f, 0.f);
}

template
const Quaternion quaternionFromTo<Vector3D>(const Vector3D &, const Vector3D&);
