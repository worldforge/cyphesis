// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2017 Erik Ogenvik
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
#include "TestWorld.h"

#include "server/Ruleset.h"
#include "server/ServerRouting.h"

#include "rulesets/Entity.h"

#include "common/compose.hpp"
#include "common/debug.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>
#include <rulesets/GeometryProperty.h>
#include <LinearMath/btVector3.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h>

#include "stubs/common/stubLog.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using String::compose;


class GeometryPropertyIntegrationTest : public Cyphesis::TestBase
{

    public:
        GeometryPropertyIntegrationTest();

        void setup();

        void teardown();

        void test_createShapes();

        void test_createMesh();

        void test_createMeshInvalidData();
};


GeometryPropertyIntegrationTest::GeometryPropertyIntegrationTest()
{
    ADD_TEST(GeometryPropertyIntegrationTest::test_createShapes);
    ADD_TEST(GeometryPropertyIntegrationTest::test_createMesh);
    ADD_TEST(GeometryPropertyIntegrationTest::test_createMeshInvalidData);
}


void GeometryPropertyIntegrationTest::setup()
{
}

void GeometryPropertyIntegrationTest::teardown()
{

}

void GeometryPropertyIntegrationTest::test_createShapes()
{
    WFMath::AxisBox<3> aabb(WFMath::Point<3>(-2, -4, -3), WFMath::Point<3>(6, 10, 8));

    btVector3 massOffset;
    {
        GeometryProperty g1;
        btCollisionShape* shape = g1.createShape(aabb, massOffset).first;
        ASSERT_EQUAL(btVector3(-2, -3, -2.5), massOffset);
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape);
        ASSERT_NOT_NULL(box);
        ASSERT_EQUAL(btVector3(4, 7, 5.5), box->getHalfExtentsWithMargin());
    }
    {
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"shape", "box"}}));
        btCollisionShape* shape = g1.createShape(aabb, massOffset).first;
        ASSERT_EQUAL(btVector3(-2, -3, -2.5), massOffset);
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape);
        ASSERT_NOT_NULL(box);
        ASSERT_EQUAL(btVector3(4, 7, 5.5), box->getHalfExtentsWithMargin());
    }
    {
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"shape", "sphere"}}));
        btCollisionShape* shape = g1.createShape(aabb, massOffset).first;
        ASSERT_EQUAL(btVector3(-2, 0, -1), massOffset);
        btSphereShape* sphere = dynamic_cast<btSphereShape*>(shape);
        ASSERT_NOT_NULL(sphere);
        //Min radius is used
        ASSERT_EQUAL(4, sphere->getRadius());
    }
    {
        WFMath::AxisBox<3> characterAabb(WFMath::Point<3>(-2, -4, -3), WFMath::Point<3>(2, 10, 3));
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"shape", "capsule-y"}}));
        btCollisionShape* shape = g1.createShape(characterAabb, massOffset).first;
        ASSERT_EQUAL(btVector3(0, -3, 0), massOffset);
        btCapsuleShape* capsule = dynamic_cast<btCapsuleShape*>(shape);
        ASSERT_NOT_NULL(capsule);
        //Min radius is used
        ASSERT_EQUAL(2, capsule->getRadius());
    }
    {
        WFMath::AxisBox<3> characterAabb(WFMath::Point<3>(-10, -4, -3), WFMath::Point<3>(2, 2, 3));
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"shape", "capsule-x"}}));
        btCollisionShape* shape = g1.createShape(characterAabb, massOffset).first;
        ASSERT_EQUAL(btVector3(4, 1, 0), massOffset);
        btCapsuleShapeX* capsule = dynamic_cast<btCapsuleShapeX*>(shape);
        ASSERT_NOT_NULL(capsule);
        //Min radius is used
        ASSERT_EQUAL(3, capsule->getRadius());
    }
    {
        WFMath::AxisBox<3> characterAabb(WFMath::Point<3>(-3, -4, -10), WFMath::Point<3>(3, 2, 2));
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"shape", "capsule-z"}}));
        btCollisionShape* shape = g1.createShape(characterAabb, massOffset).first;
        ASSERT_EQUAL(btVector3(0, 1, 4), massOffset);
        btCapsuleShapeZ* capsule = dynamic_cast<btCapsuleShapeZ*>(shape);
        ASSERT_NOT_NULL(capsule);
        //Min radius is used
        ASSERT_EQUAL(3, capsule->getRadius());
    }

    {
        WFMath::AxisBox<3> characterAabb(WFMath::Point<3>(-2, -4, -3), WFMath::Point<3>(2, 10, 3));
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"shape", "cylinder-y"}}));
        btCollisionShape* shape = g1.createShape(characterAabb, massOffset).first;
        ASSERT_EQUAL(btVector3(0, -3, 0), massOffset);
        btCylinderShape* cylinder = dynamic_cast<btCylinderShape*>(shape);
        ASSERT_NOT_NULL(cylinder);
        //Min radius is used
        ASSERT_EQUAL(2, cylinder->getRadius());
    }
    {
        WFMath::AxisBox<3> characterAabb(WFMath::Point<3>(-10, -4, -3), WFMath::Point<3>(2, 2, 3));
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"shape", "cylinder-x"}}));
        btCollisionShape* shape = g1.createShape(characterAabb, massOffset).first;
        ASSERT_EQUAL(btVector3(4, 1, 0), massOffset);
        btCylinderShapeX* cylinder = dynamic_cast<btCylinderShapeX*>(shape);
        ASSERT_NOT_NULL(cylinder);
        //Min radius is used
        ASSERT_EQUAL(3, cylinder->getRadius());
    }
    {
        WFMath::AxisBox<3> characterAabb(WFMath::Point<3>(-3, -4, -10), WFMath::Point<3>(3, 2, 2));
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"shape", "cylinder-z"}}));
        btCollisionShape* shape = g1.createShape(characterAabb, massOffset).first;
        ASSERT_EQUAL(btVector3(0, 1, 4), massOffset);
        btCylinderShapeZ* cylinder = dynamic_cast<btCylinderShapeZ*>(shape);
        ASSERT_NOT_NULL(cylinder);
        //Min radius is used
        ASSERT_EQUAL(3, cylinder->getRadius());
    }
}

void GeometryPropertyIntegrationTest::test_createMesh()
{

    btVector3 massOffset;

    GeometryProperty g1;
    std::vector<Atlas::Message::Element> vertices;

    vertices.push_back(-1.f);
    vertices.push_back(1.f);
    vertices.push_back(-1.f);

    vertices.push_back(1.f);
    vertices.push_back(1.f);
    vertices.push_back(-1.f);

    vertices.push_back(1.f);
    vertices.push_back(-1.f);
    vertices.push_back(-1.f);

    vertices.push_back(-1.f);
    vertices.push_back(-1.f);
    vertices.push_back(-1.f);

    vertices.push_back(-1.f);
    vertices.push_back(1.f);
    vertices.push_back(1.f);

    vertices.push_back(-1.f);
    vertices.push_back(-1.f);
    vertices.push_back(1.f);

    vertices.push_back(1.f);
    vertices.push_back(-1.f);
    vertices.push_back(1.f);

    vertices.push_back(1.f);
    vertices.push_back(1.f);
    vertices.push_back(1.f);

    vertices.push_back(-1.f);
    vertices.push_back(1.f);
    vertices.push_back(-1.f);

    vertices.push_back(-1.f);
    vertices.push_back(1.f);
    vertices.push_back(1.f);

    vertices.push_back(1.f);
    vertices.push_back(1.f);
    vertices.push_back(1.f);

    vertices.push_back(1.f);
    vertices.push_back(1.f);
    vertices.push_back(-1.f);

    vertices.push_back(1.f);
    vertices.push_back(1.f);
    vertices.push_back(-1.f);

    vertices.push_back(1.f);
    vertices.push_back(1.f);
    vertices.push_back(1.f);

    vertices.push_back(1.f);
    vertices.push_back(-1.f);
    vertices.push_back(1.f);

    vertices.push_back(1.f);
    vertices.push_back(-1.f);
    vertices.push_back(-1.f);

    vertices.push_back(1.f);
    vertices.push_back(-1.f);
    vertices.push_back(-1.f);

    vertices.push_back(1.f);
    vertices.push_back(-1.f);
    vertices.push_back(1.f);

    vertices.push_back(-1.f);
    vertices.push_back(-1.f);
    vertices.push_back(1.f);

    vertices.push_back(-1.f);
    vertices.push_back(-1.f);
    vertices.push_back(-1.f);

    vertices.push_back(-1.f);
    vertices.push_back(1.f);
    vertices.push_back(1.f);

    vertices.push_back(-1.f);
    vertices.push_back(1.f);
    vertices.push_back(-1.f);

    vertices.push_back(-1.f);
    vertices.push_back(-1.f);
    vertices.push_back(-1.f);

    vertices.push_back(-1.f);
    vertices.push_back(-1.f);
    vertices.push_back(1.f);


    std::vector<Atlas::Message::Element> indices;

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(6);
    indices.push_back(7);
    indices.push_back(4);
    indices.push_back(8);
    indices.push_back(9);
    indices.push_back(10);
    indices.push_back(10);
    indices.push_back(11);
    indices.push_back(8);
    indices.push_back(12);
    indices.push_back(13);
    indices.push_back(14);
    indices.push_back(14);
    indices.push_back(15);
    indices.push_back(12);
    indices.push_back(16);
    indices.push_back(17);
    indices.push_back(18);
    indices.push_back(18);
    indices.push_back(19);
    indices.push_back(16);
    indices.push_back(20);
    indices.push_back(21);
    indices.push_back(22);
    indices.push_back(22);
    indices.push_back(23);
    indices.push_back(20);


    g1.set(Atlas::Message::MapType({{"shape",    "mesh"},
                                    {"vertices", vertices},
                                    {"indices",  indices}
                                   }));

    btBvhTriangleMeshShape* childShape1;

    {
        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        btCollisionShape* shape = g1.createShape(aabb, massOffset).first;
        ASSERT_EQUAL(btVector3(0, 0, 0), massOffset);
        btScaledBvhTriangleMeshShape* mesh = dynamic_cast<btScaledBvhTriangleMeshShape*>(shape);
        ASSERT_NOT_NULL(mesh);
        ASSERT_EQUAL(btVector3(1, 1, 1), mesh->getLocalScaling());
        childShape1 = mesh->getChildShape();
    }

    {
        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 3, 1));

        btCollisionShape* shape = g1.createShape(aabb, massOffset).first;
        ASSERT_EQUAL(btVector3(0, 0, 0), massOffset);
        btScaledBvhTriangleMeshShape* mesh = dynamic_cast<btScaledBvhTriangleMeshShape*>(shape);
        ASSERT_NOT_NULL(mesh);
        ASSERT_EQUAL(btVector3(1, 2, 1), mesh->getLocalScaling());
        ASSERT_EQUAL(childShape1, mesh->getChildShape());
    }
}


void GeometryPropertyIntegrationTest::test_createMeshInvalidData()
{

    btVector3 massOffset;

    {
        GeometryProperty g1;
        std::vector<Atlas::Message::Element> vertices;

        vertices.push_back(-1.f);
        vertices.push_back(1.f);

        std::vector<Atlas::Message::Element> indices;

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);

        g1.set(Atlas::Message::MapType({{"shape",    "mesh"},
                                        {"vertices", vertices},
                                        {"indices",  indices}
                                       }));

        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        btCollisionShape* shape = g1.createShape(aabb, massOffset).first;
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape);
        ASSERT_NOT_NULL(box);
    }

    {
        GeometryProperty g1;
        std::vector<Atlas::Message::Element> vertices;

        vertices.push_back(-1.f);
        vertices.push_back(1.f);
        vertices.push_back(1.f);

        std::vector<Atlas::Message::Element> indices;

        indices.push_back(0);
        indices.push_back(1);

        g1.set(Atlas::Message::MapType({{"shape",    "mesh"},
                                        {"vertices", vertices},
                                        {"indices",  indices}
                                       }));

        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        btCollisionShape* shape = g1.createShape(aabb, massOffset).first;
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape);
        ASSERT_NOT_NULL(box);
    }

    {
        GeometryProperty g1;
        std::vector<Atlas::Message::Element> vertices;

        vertices.push_back(-1.f);
        vertices.push_back(1.f);
        vertices.push_back(1.f);

        std::vector<Atlas::Message::Element> indices;

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);

        g1.set(Atlas::Message::MapType({{"shape",    "mesh"},
                                        {"vertices", vertices},
                                        {"indices",  indices}
                                       }));

        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        btCollisionShape* shape = g1.createShape(aabb, massOffset).first;
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape);
        ASSERT_NOT_NULL(box);
    }
    {
        GeometryProperty g1;
        std::vector<Atlas::Message::Element> vertices;

        vertices.push_back(-1.f);
        vertices.push_back(1.f);
        vertices.push_back(1.f);
        vertices.push_back("a");

        std::vector<Atlas::Message::Element> indices;

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);

        g1.set(Atlas::Message::MapType({{"shape",    "mesh"},
                                        {"vertices", vertices},
                                        {"indices",  indices}
                                       }));

        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        btCollisionShape* shape = g1.createShape(aabb, massOffset).first;
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape);
        ASSERT_NOT_NULL(box);
    }

    {
        GeometryProperty g1;
        std::vector<Atlas::Message::Element> vertices;

        vertices.push_back(-1.f);
        vertices.push_back(1.f);
        vertices.push_back(1.f);

        std::vector<Atlas::Message::Element> indices;

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back("a");

        g1.set(Atlas::Message::MapType({{"shape",    "mesh"},
                                        {"vertices", vertices},
                                        {"indices",  indices}
                                       }));

        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        btCollisionShape* shape = g1.createShape(aabb, massOffset).first;
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape);
        ASSERT_NOT_NULL(box);
    }
}

int main()
{
    GeometryPropertyIntegrationTest t;

    return t.run();
}

