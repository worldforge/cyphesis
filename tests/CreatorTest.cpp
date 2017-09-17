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
#include "TestWorld.h"

#include "rulesets/Creator.h"
#include "rulesets/Python_API.h"
#include "rulesets/Script.h"

#include "modules/EntityRef.h"

#include "common/id.h"
#include "common/log.h"
#include "common/Property_impl.h"
#include "common/TypeNode.h"


#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubThing.h"
#include "stubs/common/stubCustom.h"
#include "stubs/common/stubTypeNode.h"
#include "stubs/modules/stubLocation.h"

#include "stubs/common/stubRouter.h"
#include "stubs/common/stubBaseWorld.h"
#include "stubs/common/stubProperty.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/common/stubPropertyManager.h"
#include "stubs/rulesets/stubDomain.h"

#include <cstdlib>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::RootOperation;

class Creatortest : public Cyphesis::TestBase
{
  private:
    static Operation m_Character_filterExternalOperation_called;

    Creator * m_creator;
    TypeNode * m_type;
  public:
    Creatortest();

    void setup();
    void teardown();

    void test_exerciser();
    void test_externalOperation_normal();

    static void Character_filterExternalOperation_called(const Operation & op);
};

Operation Creatortest::m_Character_filterExternalOperation_called(0);

void Creatortest::Character_filterExternalOperation_called(const Operation & op)
{
    m_Character_filterExternalOperation_called = op;
}

Creatortest::Creatortest()
{
    ADD_TEST(Creatortest::test_exerciser);
    ADD_TEST(Creatortest::test_externalOperation_normal);
}

void Creatortest::setup()
{
    m_creator = new Creator("1", 1);
    m_type = new TypeNode("character");
    m_creator->setType(m_type);

    m_Character_filterExternalOperation_called = 0;
}

void Creatortest::teardown()
{
    delete m_creator;
    delete m_type;
}

void Creatortest::test_exerciser()
{
    IGEntityExerciser ee(*m_creator);

    // Throw an op of every type at the entity
    ee.runOperations();

    // Subscribe the entity to every class of op
    std::set<std::string> opNames;
    ee.addAllOperations(opNames);

    // Throw an op of every type at the entity again now it is subscribed
    ee.runOperations();
}

void Creatortest::test_externalOperation_normal()
{
    RootOperation op;
    op->setFrom(m_creator->getId());

    m_creator->externalOperation(op, *(Link*)0);

    ASSERT_TRUE(m_Character_filterExternalOperation_called.isValid());
    ASSERT_EQUAL(m_Character_filterExternalOperation_called->getClassNo(),
                 Atlas::Objects::Operation::ROOT_OPERATION_NO);
}

int main(int argc, char ** argv)
{
    Creatortest t;

    return t.run();
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

Character::Character(const std::string & id, long intId) :
           Thing(id, intId),
               m_movement(*(Movement*)0),
               m_externalMind(0)
{
}

Character::~Character()
{
}

void Character::operation(const Operation & op, OpVector &)
{
}

void Character::externalOperation(const Operation & op, Link &)
{
    filterExternalOperation(op);
}

void Character::filterExternalOperation(const Operation & op)
{
    Creatortest::Character_filterExternalOperation_called(op);
}

void Character::ImaginaryOperation(const Operation & op, OpVector &)
{
}

void Character::InfoOperation(const Operation & op, OpVector &)
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

void Character::ActuateOperation(const Operation & op, OpVector &)
{
}

void Character::RelayOperation(const Operation & op, OpVector &)
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

void Character::mindThinkOperation(const Operation & op, OpVector & res)
{
}


void Character::sendMind(const Operation & op, OpVector & res)
{
}

std::vector<Atlas::Objects::Root> Character::getThoughts() const
{
    return std::vector<Atlas::Objects::Root>();
}


long int Character::s_serialNumberNext = 0L;




void log(LogLevel lvl, const std::string & msg)
{
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

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
}
