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

#include "TestBase.h"

#include "rulesets/Motion.h"

#include "rulesets/Entity.h"
#include "rulesets/PhysicalDomain.h"
#include "rulesets/OutfitProperty.h"

#include "common/TypeNode.h"

class Motiontest : public Cyphesis::TestBase
{
  protected:
    Entity * tlve;
    Entity * ent;
    Entity * other;
    TypeNode * type;
    Motion * motion;
    Domain* domain;
  public:
    Motiontest();

    void setup();
    void teardown();

    void test_adjustPosition();
    void test_genUpdateOperation();
    void test_genMoveOperation();
    void test_checkCollision_no_box();
    void test_checkCollision_moving_box();
    void test_checkCollision_two_boxes();
    void test_checkCollision_close();
    void test_checkCollision_broken();
    void test_checkCollision_velocity();
    void test_checkCollision_inner1();
    void test_checkCollision_inner2();
    void test_checkCollision_inner3();
    void test_checkCollision_inner4();
};

void Motiontest::setup()
{
    type = new TypeNode("test_type");

    tlve = new Entity("0", 0);
    domain = new PhysicalDomain(*tlve);
    ent = new Entity("1", 1);
    other = new Entity("2", 2);

    ent->m_location.m_loc = tlve;
    ent->m_location.m_pos = Point3D(1, 1, 0);
    ent->m_location.m_velocity = Vector3D(1,0,0);
    ent->setType(type);

    // Set up another entity to test collisions with.
    other->m_location.m_loc = tlve;
    other->m_location.m_pos = Point3D(10, 0, 0);
    other->setType(type);

    tlve->m_contains = new LocatedEntitySet;
    tlve->m_contains->insert(ent);
    tlve->m_contains->insert(other);
    tlve->setType(type);
    tlve->incRef();
    tlve->incRef();

    motion = new Motion(*ent);

    std::string example_mode("walking");

    motion->setMode(example_mode);
    assert(motion->mode() == example_mode);
}

Motiontest::Motiontest()
{
    ADD_TEST(Motiontest::test_adjustPosition);
    ADD_TEST(Motiontest::test_genUpdateOperation);
    ADD_TEST(Motiontest::test_genMoveOperation);
    ADD_TEST(Motiontest::test_checkCollision_no_box);
    ADD_TEST(Motiontest::test_checkCollision_moving_box);
    ADD_TEST(Motiontest::test_checkCollision_two_boxes);
    ADD_TEST(Motiontest::test_checkCollision_close);
    ADD_TEST(Motiontest::test_checkCollision_broken);
    ADD_TEST(Motiontest::test_checkCollision_velocity);
    ADD_TEST(Motiontest::test_checkCollision_inner1);
    ADD_TEST(Motiontest::test_checkCollision_inner2);
    ADD_TEST(Motiontest::test_checkCollision_inner3);
    ADD_TEST(Motiontest::test_checkCollision_inner4);
}

void Motiontest::teardown()
{
    ent->m_location.m_loc = 0;
    other->m_location.m_loc = 0;

    delete motion;
    delete tlve;
    delete ent;
    delete other;
    delete type;
}

void Motiontest::test_adjustPosition()
{
    motion->adjustPostion();
}

void Motiontest::test_genUpdateOperation()
{
    motion->genUpdateOperation();
}

void Motiontest::test_genMoveOperation()
{
    motion->genMoveOperation();
}

void Motiontest::test_checkCollision_no_box()
{
    // No collisions yet
    motion->checkCollisions(*domain);
    ASSERT_TRUE(!motion->collision());
}

void Motiontest::test_checkCollision_moving_box()
{
    // Set up our moving entity with a bbox so collisions can be checked for.
    ent->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(1,1,1));

    // No collision yet, as other still has no big box
    motion->checkCollisions(*domain);
    assert(!motion->collision());
}

void Motiontest::test_checkCollision_two_boxes()
{
    // Set up our moving entity with a bbox so collisions can be checked for.
    ent->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(1,1,1));

    // Set up the other entity with a bbox so collisions can be checked for.
    other->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(5,1,1));

    // No collision yet, as other is still too far away
    motion->checkCollisions(*domain);
    assert(!motion->collision());
}

void Motiontest::test_checkCollision_close()
{
    // Set up our moving entity with a bbox so collisions can be checked for.
    ent->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(1,1,1));

    // Set up the other entity with a bbox so collisions can be checked for.
    other->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(5,1,1));

    // Move it closer
    other->m_location.m_pos = Point3D(3, 0, 0);

    // Now it can collide
    motion->checkCollisions(*domain);
    assert(motion->collision());
    motion->resolveCollision();
}

void Motiontest::test_checkCollision_broken()
{
    // Set up our moving entity with a bbox so collisions can be checked for.
    ent->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(1,1,1));

    // Set up the other entity with a bbox so collisions can be checked for.
    other->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(5,1,1));

    // Move it closer
    other->m_location.m_pos = Point3D(3, 0, 0);

    // Set up the collision again
    motion->checkCollisions(*domain);
    assert(motion->collision());
    // But this time break the hierarchy to hit the error message
    other->m_location.m_loc = ent;

    motion->resolveCollision();
}

void Motiontest::test_checkCollision_velocity()
{
    // Set up our moving entity with a bbox so collisions can be checked for.
    ent->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(1,1,1));

    // Set up the other entity with a bbox so collisions can be checked for.
    other->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(5,1,1));

    // Move it closer
    other->m_location.m_pos = Point3D(3, 0, 0);

    // Set up the collision again
    motion->checkCollisions(*domain);
    assert(motion->collision());

    // Re-align the velocity so some is preserved by the collision normal
    ent->m_location.m_velocity = Vector3D(1,1,0);

    motion->resolveCollision();
}

void Motiontest::test_checkCollision_inner1()
{
    // Set up our moving entity with a bbox so collisions can be checked for.
    ent->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(1,1,1));

    // Set up the other entity with a bbox so collisions can be checked for.
    other->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(5,1,1));

    // Move it closer
    other->m_location.m_pos = Point3D(3, 0, 0);

    // Add another entity inside other
    Entity inner("3", 3);

    inner.m_location.m_loc = other;
    inner.m_location.m_pos = Point3D(0,0,0);

    other->m_contains = new LocatedEntitySet;
    other->m_contains->insert(&inner);
    // Make other non-simple so that collision checks go inside
    other->m_location.setSimple(false);

    motion->checkCollisions(*domain);

    assert(motion->collision());

    inner.m_location.m_loc = 0;
}

void Motiontest::test_checkCollision_inner2()
{
    // Set up our moving entity with a bbox so collisions can be checked for.
    ent->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(1,1,1));

    // Set up the other entity with a bbox so collisions can be checked for.
    other->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(5,1,1));

    // Move it closer
    other->m_location.m_pos = Point3D(3, 0, 0);

    // Add another entity inside other
    Entity inner("3", 3);

    inner.m_location.m_loc = other;
    inner.m_location.m_pos = Point3D(0,0,0);

    other->m_contains = new LocatedEntitySet;
    other->m_contains->insert(&inner);
    // Make other non-simple so that collision checks go inside
    other->m_location.setSimple(false);

    other->m_location.m_orientation = Quaternion(1,0,0,0);

    motion->checkCollisions(*domain);

    assert(motion->collision());

    inner.m_location.m_loc = 0;
}

void Motiontest::test_checkCollision_inner3()
{
    // Set up our moving entity with a bbox so collisions can be checked for.
    ent->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(1,1,1));

    // Set up the other entity with a bbox so collisions can be checked for.
    other->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(5,1,1));

    // Move it closer
    other->m_location.m_pos = Point3D(3, 0, 0);

    // Add another entity inside other
    Entity inner("3", 3);

    inner.m_location.m_loc = other;
    inner.m_location.m_pos = Point3D(0,0,0);

    other->m_contains = new LocatedEntitySet;
    other->m_contains->insert(&inner);
    // Make other non-simple so that collision checks go inside
    other->m_location.setSimple(false);


    other->m_location.m_orientation = Quaternion(1,0,0,0);

    inner.m_location.m_bBox = BBox(Point3D(-0.1,-0.1,-0.1),
                                   Point3D(0.1,0.1,0.1));

    motion->checkCollisions(*domain);

    assert(motion->collision());

    inner.m_location.m_loc = 0;
}

void Motiontest::test_checkCollision_inner4()
{
    // Set up our moving entity with a bbox so collisions can be checked for.
    ent->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(1,1,1));

    // Set up the other entity with a bbox so collisions can be checked for.
    other->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(5,1,1));

    // Move it closer
    other->m_location.m_pos = Point3D(3, 0, 0);

    // Add another entity inside other
    Entity inner("3", 3);

    inner.m_location.m_loc = other;
    inner.m_location.m_pos = Point3D(0,0,0);

    other->m_contains = new LocatedEntitySet;
    other->m_contains->insert(&inner);
    // Make other non-simple so that collision checks go inside
    other->m_location.setSimple(false);

    other->m_location.m_orientation = Quaternion(1,0,0,0);

    inner.m_location.m_bBox = BBox(Point3D(-0.1,-0.1,-0.1),
                                   Point3D(0.1,0.1,0.1));

    // Move the inner entity too far away for collision this interval
    inner.m_location.m_pos = Point3D(3,0,0);

    motion->checkCollisions(*domain);

    assert(motion->collision());
    ASSERT_EQUAL(motion->m_collEntity, other);

    inner.m_location.m_loc = 0;
}

int main()
{
    Motiontest t;

    t.run();
}

// stubs

#include "common/const.h"
#include "common/log.h"
#include "common/Property_impl.h"

#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubDomain.h"
#include "stubs/rulesets/stubTerrainProperty.h"
#include "stubs/rulesets/stubOutfitProperty.h"
#include "stubs/common/stubCustom.h"


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

void LocatedEntity::removeDelegate(int class_no, const std::string & delegate)
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
    LocatedEntity* oldLoc = m_location.m_loc;
    m_location.m_loc = new_loc;
    m_location.m_loc->incRef();
    assert(m_location.m_loc->checkRef() > 0);

    onContainered(oldLoc);
    oldLoc->decRef();
}

void LocatedEntity::merge(const Atlas::Message::MapType & ent)
{
}

void LocatedEntity::addChild(LocatedEntity& childEntity)
{

}

void LocatedEntity::removeChild(LocatedEntity& childEntity)
{

}

void LocatedEntity::setType(const TypeNode* t)
{
    m_type = t;

}

#include "stubs/common/stubRouter.h"
#include "stubs/modules/stubLocation.h"
#include "stubs/common/stubTypeNode.h"
#include "stubs/common/stubProperty.h"
#include "rulesets/EntityProperty.h"
#include "stubs/rulesets/stubEntityProperty.h"

void log(LogLevel lvl, const std::string & msg)
{
}

WFMath::CoordType squareDistance(const Point3D & u, const Point3D & v)
{
    return 0.0f;
}

