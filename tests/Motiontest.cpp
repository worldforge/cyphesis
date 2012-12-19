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

#include "TestBase.h"

#include "rulesets/Motion.h"

#include "rulesets/Entity.h"

#include "common/TypeNode.h"

class Motiontest : public Cyphesis::TestBase
{
  protected:
    Entity * tlve;
    Entity * ent;
    Entity * other;
    TypeNode * type;
    Motion * motion;
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
    motion->checkCollisions();
    ASSERT_TRUE(!motion->collision());
}

void Motiontest::test_checkCollision_moving_box()
{
    // Set up our moving entity with a bbox so collisions can be checked for.
    ent->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(1,1,1));

    // No collision yet, as other still has no big box
    motion->checkCollisions();
    assert(!motion->collision());
}

void Motiontest::test_checkCollision_two_boxes()
{
    // Set up our moving entity with a bbox so collisions can be checked for.
    ent->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(1,1,1));

    // Set up the other entity with a bbox so collisions can be checked for.
    other->m_location.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(5,1,1));

    // No collision yet, as other is still too far away
    motion->checkCollisions();
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
    motion->checkCollisions();
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
    motion->checkCollisions();
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
    motion->checkCollisions();
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

    motion->checkCollisions();

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

    motion->checkCollisions();

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

    motion->checkCollisions();

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

    motion->checkCollisions();

    assert(motion->collision());

    inner.m_location.m_loc = 0;
}

int main()
{
    Motiontest t;

    t.run();
}
