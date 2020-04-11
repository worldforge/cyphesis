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

#include "../../TestBase.h"
#include "../../TestWorld.h"

#include "rules/simulation/Entity.h"
#include "rules/simulation/GeometryProperty.h"

#include "common/debug.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h>

#include "../../stubs/physics/stubVector3D.h"

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

        void setup() override;

        void teardown() override;

        void test_createShapes();

        void test_createMesh();

        void test_createMeshInvalidData();

        void test_createCompound();
};


GeometryPropertyIntegrationTest::GeometryPropertyIntegrationTest()
{
    ADD_TEST(GeometryPropertyIntegrationTest::test_createCompound);
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


void GeometryPropertyIntegrationTest::test_createCompound()
{
    WFMath::AxisBox<3> aabb(WFMath::Point<3>(-2, -4, -3), WFMath::Point<3>(6, 10, 8));

    btVector3 massOffset;
    {
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type",   "compound"},
                                        {"shapes", ListType{
                                                MapType{
                                                        {"type",   "box"},
                                                        {"points", ListType{
                                                                1.f, 2.f, 3.f,
                                                                6.f, 8.f, 10.f
                                                        }}
                                                },
                                                MapType{
                                                        {"type",   "box"},
                                                        {"points", ListType{
                                                                1.f, 2.f, 3.f,
                                                                6.f, 8.f, 10.f
                                                        }}
                                                }

                                        }}}));
        auto result = g1.createShape(aabb, massOffset, 1.0f);
        btCompoundShape* compoundShape = dynamic_cast<btCompoundShape*>(result.get());
        ASSERT_NOT_NULL(compoundShape);
        ASSERT_EQUAL(2, compoundShape->getNumChildShapes());
        ASSERT_EQUAL(BOX_SHAPE_PROXYTYPE, compoundShape->getChildShape(0)->getShapeType());
        auto* boxChild1 = dynamic_cast<btBoxShape*>(compoundShape->getChildShape(0));
        ASSERT_FUZZY_EQUAL(8.f / 2.f, boxChild1->getImplicitShapeDimensions().x(), 0.1f);
        ASSERT_FUZZY_EQUAL(14.f / 2.f, boxChild1->getImplicitShapeDimensions().y(), 0.1f);
        ASSERT_FUZZY_EQUAL(11.f / 2.f, boxChild1->getImplicitShapeDimensions().z(), 0.1f);
    }

}

void GeometryPropertyIntegrationTest::test_createShapes()
{
    WFMath::AxisBox<3> aabb(WFMath::Point<3>(-2, -4, -3), WFMath::Point<3>(6, 10, 8));

    btVector3 massOffset;
    {
        GeometryProperty g1;
        auto shape = g1.createShape(aabb, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(-2, -3, -2.5f), massOffset);
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape.get());
        ASSERT_NOT_NULL(box);
        ASSERT_EQUAL(btVector3(4, 7, 5.5), box->getHalfExtentsWithMargin());
    }
    {
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type", "box"}}));
        auto shape = g1.createShape(aabb, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(-2, -3, -2.5f), massOffset);
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape.get());
        ASSERT_NOT_NULL(box);
        ASSERT_EQUAL(btVector3(4, 7, 5.5), box->getHalfExtentsWithMargin());
    }
    {
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type", "sphere"}}));
        auto shape = g1.createShape(aabb, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(-2, -3, -2.5f), massOffset);
        btSphereShape* sphere = dynamic_cast<btSphereShape*>(shape.get());
        ASSERT_NOT_NULL(sphere);
        //Min radius is used
        ASSERT_EQUAL(4, sphere->getRadius());
    }
    {
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type", "sphere"}}));
        auto shape = g1.createShape({{-1, 0, -1},
                                     {1,  1, 1}}, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(0, -0.5f, 0), massOffset);
        btSphereShape* sphere = dynamic_cast<btSphereShape*>(shape.get());
        //Min radius is used
        ASSERT_EQUAL(0.5, sphere->getRadius());
    }
    {
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type", "sphere"}, {"scaler", "x"}}));
        auto shape = g1.createShape({{-1, 0, -1},
                                     {1,  1, 1}}, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(0, -0.5f, 0), massOffset);
        btSphereShape* sphere = dynamic_cast<btSphereShape*>(shape.get());
        //X radius is used
        ASSERT_EQUAL(1, sphere->getRadius());
    }
    {
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type", "sphere"}, {"scaler", "y"}}));
        auto shape = g1.createShape({{-1, 0, -1},
                                     {1,  1, 1}}, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(0, -0.5f, 0), massOffset);
        btSphereShape* sphere = dynamic_cast<btSphereShape*>(shape.get());
        //Y radius is used
        ASSERT_EQUAL(0.5, sphere->getRadius());
    }
    {
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type", "sphere"}, {"scaler", "z"}}));
        auto shape = g1.createShape({{-1, 0, -1},
                                     {1,  1, 1}}, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(0, -0.5f, 0), massOffset);
        btSphereShape* sphere = dynamic_cast<btSphereShape*>(shape.get());
        //Z radius is used
        ASSERT_EQUAL(1, sphere->getRadius());
    }
    {
        WFMath::AxisBox<3> characterAabb(WFMath::Point<3>(-2, -4, -3), WFMath::Point<3>(2, 10, 3));
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type", "capsule-y"}}));
        auto shape = g1.createShape(characterAabb, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(0, -3, 0), massOffset);
        btCapsuleShape* capsule = dynamic_cast<btCapsuleShape*>(shape.get());
        ASSERT_NOT_NULL(capsule);
        //Min radius is used
        ASSERT_EQUAL(2, capsule->getRadius());
    }
    {
        WFMath::AxisBox<3> characterAabb(WFMath::Point<3>(-10, -4, -3), WFMath::Point<3>(2, 2, 3));
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type", "capsule-x"}}));
        auto shape = g1.createShape(characterAabb, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(4, 1, 0), massOffset);
        btCapsuleShapeX* capsule = dynamic_cast<btCapsuleShapeX*>(shape.get());
        ASSERT_NOT_NULL(capsule);
        //Min radius is used
        ASSERT_EQUAL(3, capsule->getRadius());
    }
    {
        WFMath::AxisBox<3> characterAabb(WFMath::Point<3>(-3, -4, -10), WFMath::Point<3>(3, 2, 2));
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type", "capsule-z"}}));
        auto shape = g1.createShape(characterAabb, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(0, 1, 4), massOffset);
        btCapsuleShapeZ* capsule = dynamic_cast<btCapsuleShapeZ*>(shape.get());
        ASSERT_NOT_NULL(capsule);
        //Min radius is used
        ASSERT_EQUAL(3, capsule->getRadius());
    }

    {
        WFMath::AxisBox<3> characterAabb(WFMath::Point<3>(-2, -4, -3), WFMath::Point<3>(2, 10, 3));
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type", "cylinder-y"}}));
        auto shape = g1.createShape(characterAabb, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(0, -3, 0), massOffset);
        btCylinderShape* cylinder = dynamic_cast<btCylinderShape*>(shape.get());
        ASSERT_NOT_NULL(cylinder);
        //Min radius is used
        ASSERT_EQUAL(2, cylinder->getRadius());
    }
    {
        WFMath::AxisBox<3> characterAabb(WFMath::Point<3>(-10, -4, -3), WFMath::Point<3>(2, 2, 3));
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type", "cylinder-x"}}));
        auto shape = g1.createShape(characterAabb, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(4, 1, 0), massOffset);
        btCylinderShapeX* cylinder = dynamic_cast<btCylinderShapeX*>(shape.get());
        ASSERT_NOT_NULL(cylinder);
        //Min radius is used
        ASSERT_EQUAL(3, cylinder->getRadius());
    }
    {
        WFMath::AxisBox<3> characterAabb(WFMath::Point<3>(-3, -4, -10), WFMath::Point<3>(3, 2, 2));
        GeometryProperty g1;
        g1.set(Atlas::Message::MapType({{"type", "cylinder-z"}}));
        auto shape = g1.createShape(characterAabb, massOffset, 1.0f);
        ASSERT_EQUAL(btVector3(0, 1, 4), massOffset);
        btCylinderShapeZ* cylinder = dynamic_cast<btCylinderShapeZ*>(shape.get());
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

    vertices.emplace_back(-1.f);
    vertices.emplace_back(1.f);
    vertices.emplace_back(-1.f);

    vertices.emplace_back(1.f);
    vertices.emplace_back(1.f);
    vertices.emplace_back(-1.f);

    vertices.emplace_back(1.f);
    vertices.emplace_back(-1.f);
    vertices.emplace_back(-1.f);

    vertices.emplace_back(-1.f);
    vertices.emplace_back(-1.f);
    vertices.emplace_back(-1.f);

    vertices.emplace_back(-1.f);
    vertices.emplace_back(1.f);
    vertices.emplace_back(1.f);

    vertices.emplace_back(-1.f);
    vertices.emplace_back(-1.f);
    vertices.emplace_back(1.f);

    vertices.emplace_back(1.f);
    vertices.emplace_back(-1.f);
    vertices.emplace_back(1.f);

    vertices.emplace_back(1.f);
    vertices.emplace_back(1.f);
    vertices.emplace_back(1.f);

    vertices.emplace_back(-1.f);
    vertices.emplace_back(1.f);
    vertices.emplace_back(-1.f);

    vertices.emplace_back(-1.f);
    vertices.emplace_back(1.f);
    vertices.emplace_back(1.f);

    vertices.emplace_back(1.f);
    vertices.emplace_back(1.f);
    vertices.emplace_back(1.f);

    vertices.emplace_back(1.f);
    vertices.emplace_back(1.f);
    vertices.emplace_back(-1.f);

    vertices.emplace_back(1.f);
    vertices.emplace_back(1.f);
    vertices.emplace_back(-1.f);

    vertices.emplace_back(1.f);
    vertices.emplace_back(1.f);
    vertices.emplace_back(1.f);

    vertices.emplace_back(1.f);
    vertices.emplace_back(-1.f);
    vertices.emplace_back(1.f);

    vertices.emplace_back(1.f);
    vertices.emplace_back(-1.f);
    vertices.emplace_back(-1.f);

    vertices.emplace_back(1.f);
    vertices.emplace_back(-1.f);
    vertices.emplace_back(-1.f);

    vertices.emplace_back(1.f);
    vertices.emplace_back(-1.f);
    vertices.emplace_back(1.f);

    vertices.emplace_back(-1.f);
    vertices.emplace_back(-1.f);
    vertices.emplace_back(1.f);

    vertices.emplace_back(-1.f);
    vertices.emplace_back(-1.f);
    vertices.emplace_back(-1.f);

    vertices.emplace_back(-1.f);
    vertices.emplace_back(1.f);
    vertices.emplace_back(1.f);

    vertices.emplace_back(-1.f);
    vertices.emplace_back(1.f);
    vertices.emplace_back(-1.f);

    vertices.emplace_back(-1.f);
    vertices.emplace_back(-1.f);
    vertices.emplace_back(-1.f);

    vertices.emplace_back(-1.f);
    vertices.emplace_back(-1.f);
    vertices.emplace_back(1.f);


    std::vector<Atlas::Message::Element> indices;

    indices.emplace_back(0);
    indices.emplace_back(1);
    indices.emplace_back(2);
    indices.emplace_back(2);
    indices.emplace_back(3);
    indices.emplace_back(0);
    indices.emplace_back(4);
    indices.emplace_back(5);
    indices.emplace_back(6);
    indices.emplace_back(6);
    indices.emplace_back(7);
    indices.emplace_back(4);
    indices.emplace_back(8);
    indices.emplace_back(9);
    indices.emplace_back(10);
    indices.emplace_back(10);
    indices.emplace_back(11);
    indices.emplace_back(8);
    indices.emplace_back(12);
    indices.emplace_back(13);
    indices.emplace_back(14);
    indices.emplace_back(14);
    indices.emplace_back(15);
    indices.emplace_back(12);
    indices.emplace_back(16);
    indices.emplace_back(17);
    indices.emplace_back(18);
    indices.emplace_back(18);
    indices.emplace_back(19);
    indices.emplace_back(16);
    indices.emplace_back(20);
    indices.emplace_back(21);
    indices.emplace_back(22);
    indices.emplace_back(22);
    indices.emplace_back(23);
    indices.emplace_back(20);


    g1.set(Atlas::Message::MapType({{"type",     "mesh"},
                                    {"vertices", vertices},
                                    {"indices",  indices}
                                   }));


    //Creating mesh shape with no mass should result in a btScaledBvhTriangleMeshShape, with mass a btGImpactMeshShape
    {
        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        auto shape = g1.createShape(aabb, massOffset, 1.0);
        ASSERT_EQUAL(btVector3(0, 0, 0), massOffset);
        auto mesh = dynamic_cast<btConvexTriangleMeshShape*>(shape.get());
        ASSERT_NOT_NULL(mesh);
        ASSERT_EQUAL(btVector3(1, 1, 1), mesh->getLocalScaling());
    }

    //Make sure that we reuse the same child shape.
    btBvhTriangleMeshShape* childShape1;
    {
        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        auto shape = g1.createShape(aabb, massOffset, 0);
        ASSERT_EQUAL(btVector3(0, 0, 0), massOffset);
        btScaledBvhTriangleMeshShape* mesh = dynamic_cast<btScaledBvhTriangleMeshShape*>(shape.get());
        ASSERT_NOT_NULL(mesh);
        ASSERT_EQUAL(btVector3(1, 1, 1), mesh->getLocalScaling());
        childShape1 = mesh->getChildShape();
    }

    {
        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 3, 1));

        auto shape = g1.createShape(aabb, massOffset, 0);
        ASSERT_EQUAL(btVector3(0, 0, 0), massOffset);
        btScaledBvhTriangleMeshShape* mesh = dynamic_cast<btScaledBvhTriangleMeshShape*>(shape.get());
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

        vertices.emplace_back(-1.f);
        vertices.emplace_back(1.f);

        std::vector<Atlas::Message::Element> indices;

        indices.emplace_back(0);
        indices.emplace_back(1);
        indices.emplace_back(2);

        g1.set(Atlas::Message::MapType({{"type",     "mesh"},
                                        {"vertices", vertices},
                                        {"indices",  indices}
                                       }));

        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        auto shape = g1.createShape(aabb, massOffset, 0);
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape.get());
        ASSERT_NOT_NULL(box);
    }

    {
        GeometryProperty g1;
        std::vector<Atlas::Message::Element> vertices;

        vertices.emplace_back(-1.f);
        vertices.emplace_back(1.f);
        vertices.emplace_back(1.f);

        std::vector<Atlas::Message::Element> indices;

        indices.emplace_back(0);
        indices.emplace_back(1);

        g1.set(Atlas::Message::MapType({{"type",     "mesh"},
                                        {"vertices", vertices},
                                        {"indices",  indices}
                                       }));

        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        auto shape = g1.createShape(aabb, massOffset, 0);
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape.get());
        ASSERT_NOT_NULL(box);
    }

    {
        GeometryProperty g1;
        std::vector<Atlas::Message::Element> vertices;

        vertices.emplace_back(-1.f);
        vertices.emplace_back(1.f);
        vertices.emplace_back(1.f);

        std::vector<Atlas::Message::Element> indices;

        indices.emplace_back(0);
        indices.emplace_back(1);
        indices.emplace_back(2);

        g1.set(Atlas::Message::MapType({{"type",     "mesh"},
                                        {"vertices", vertices},
                                        {"indices",  indices}
                                       }));

        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        auto shape = g1.createShape(aabb, massOffset, 0);
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape.get());
        ASSERT_NOT_NULL(box);
    }
    {
        GeometryProperty g1;
        std::vector<Atlas::Message::Element> vertices;

        vertices.emplace_back(-1.f);
        vertices.emplace_back(1.f);
        vertices.emplace_back(1.f);
        vertices.emplace_back("a");

        std::vector<Atlas::Message::Element> indices;

        indices.emplace_back(0);
        indices.emplace_back(1);
        indices.emplace_back(2);

        g1.set(Atlas::Message::MapType({{"type",     "mesh"},
                                        {"vertices", vertices},
                                        {"indices",  indices}
                                       }));

        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        auto shape = g1.createShape(aabb, massOffset, 0);
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape.get());
        ASSERT_NOT_NULL(box);
    }

    {
        GeometryProperty g1;
        std::vector<Atlas::Message::Element> vertices;

        vertices.emplace_back(-1.f);
        vertices.emplace_back(1.f);
        vertices.emplace_back(1.f);

        std::vector<Atlas::Message::Element> indices;

        indices.emplace_back(0);
        indices.emplace_back(1);
        indices.emplace_back(2);
        indices.emplace_back("a");

        g1.set(Atlas::Message::MapType({{"type",     "mesh"},
                                        {"vertices", vertices},
                                        {"indices",  indices}
                                       }));

        WFMath::AxisBox<3> aabb(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));

        auto shape = g1.createShape(aabb, massOffset, 0);
        btBoxShape* box = dynamic_cast<btBoxShape*>(shape.get());
        ASSERT_NOT_NULL(box);
    }
}

int main()
{
    GeometryPropertyIntegrationTest t;

    return t.run();
}

