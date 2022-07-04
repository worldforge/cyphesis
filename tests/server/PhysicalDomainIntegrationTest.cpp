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

#include "../TestBaseWithContext.h"
#include "../TestWorld.h"

#include "server/Ruleset.h"

#include "rules/simulation/Entity.h"

#include "common/debug.h"

#include <Atlas/Objects/Anonymous.h>
#include <wfmath/atlasconv.h>

#include <rules/simulation/PhysicalDomain.h>
#include "physics/Convert.h"
#include <rules/simulation/TerrainProperty.h>
#include <Mercator/BasePoint.h>
#include <Mercator/Terrain.h>
#include <rules/simulation/PropelProperty.h>
#include <rules/simulation/AngularFactorProperty.h>
#include <rules/simulation/GeometryProperty.h>
#include "rules/simulation/PhysicalWorld.h"
#include "rules/BBoxProperty.h"
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

#include <rules/simulation/TerrainModProperty.h>
#include <rules/simulation/EntityProperty.h>
#include <rules/simulation/ModeDataProperty.h>
#include <rules/simulation/VisibilityDistanceProperty.h>
#include "../stubs/common/stubMonitors.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

namespace Cyphesis {
    template<>
    int AssertBase::assertFuzzyEqual(const char* l, const WFMath::Point<3>& lval,
                                     const char* r, const WFMath::Point<3>& rval,
                                     const char* e, const WFMath::CoordType& epsilon,
                                     const char* func, const char* file, int line)
    {
        if (!lval.isEqualTo(rval, epsilon)) {
            addFailure(String::compose("%1:%2: %3: Assertion '%4 ~= %5' failed. "
                                       "%6 != %7",
                                       file, line, func, l, r, lval, rval));
            return -1;
        }
        return 0;
    }
}

class TestPhysicalDomain : public PhysicalDomain
{
    public:
        explicit TestPhysicalDomain(LocatedEntity& entity) :
                PhysicalDomain(entity)
        {

        }

        PhysicalWorld* test_getPhysicalWorld() const
        {
            return m_dynamicsWorld.get();
        }


        btRigidBody* test_getRigidBody(long id)
        {
            return btRigidBody::upcast(m_entries.find(id)->second->collisionObject.get());
        }

        void test_childEntityPropertyApplied(const std::string& name, PropertyBase& prop, long id)
        {
            childEntityPropertyApplied(name, prop, *m_entries.find(id)->second);
        }
};

double epsilon = 0.0001;

struct TestContext
{
    long m_id_counter;

    long newId()
    {
        return ++m_id_counter;
    }
};


struct Tested : public Cyphesis::TestBaseWithContext<TestContext>
{
    Tested()
    {
        ADD_TEST(Tested::test_deleteWithPlanted);
        ADD_TEST(Tested::test_scaleBbox);
        ADD_TEST(Tested::test_movePlantedAndResting);
        ADD_TEST(Tested::test_plantedOn);
        ADD_TEST(Tested::test_terrainMods);
        ADD_TEST(Tested::test_lake_rotated);
        ADD_TEST(Tested::test_lake);
        ADD_TEST(Tested::test_ocean);
        ADD_TEST(Tested::test_placement);
        ADD_TEST(Tested::test_convert);
        ADD_TEST(Tested::test_terrainPrecision);
        ADD_TEST(Tested::test_fallToBottom);
        ADD_TEST(Tested::test_standOnFixed);
        ADD_TEST(Tested::test_fallToTerrain);
        ADD_TEST(Tested::test_collision);
        ADD_TEST(Tested::test_mode);
        ADD_TEST(Tested::test_determinism);
        ADD_TEST(Tested::test_zoffset);
        ADD_TEST(Tested::test_zscaledoffset);
        ADD_TEST(Tested::test_visibility);
        ADD_TEST(Tested::test_stairs);
    }


    void test_scaleBbox(TestContext& context)
    {
        double tickSize = 1.0 / 15.0;
        double time = 0;

        OpVector res;

        TypeNode rockType("rock");

        Property<double> massProp{};
        massProp.data() = 10000;

        ModeProperty plantedProperty{};
        plantedProperty.set("planted");

        ModeProperty freeProperty{};
        freeProperty.set("free");

        Entity rootEntity(context.newId());
        rootEntity.incRef();
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, 0, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));


        Entity plantedEntity(context.newId());
        plantedEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(plantedProperty.copy()));
        plantedEntity.setType(&rockType);
        plantedEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(0, 0, 0);
        plantedEntity.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();

        auto& bBoxProperty = plantedEntity.requirePropertyClassFixed<BBoxProperty>();
        bBoxProperty.data() = {{-1, 0, -1},
                               {1,  1, 1}};
        plantedEntity.applyProperty(bBoxProperty);


        domain->addEntity(plantedEntity);

        btVector3 from(0, 10, 0);
        btVector3 to(0, -10, 0);

        btCollisionWorld::ClosestRayResultCallback callback(from, to);
        domain->test_getPhysicalWorld()->rayTest(from, to, callback);
        domain->tick(1.0f, res);

        ASSERT_TRUE(callback.hasHit());
        ASSERT_FUZZY_EQUAL(1.0f, callback.m_hitPointWorld.y(), 0.1f);

        //Add a box and let it fall on the planted entity
        Entity freeEntity(context.newId());
        freeEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(freeProperty.copy()));
        freeEntity.setType(&rockType);
        freeEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(0, 10, 0);
        freeEntity.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
        freeEntity.requirePropertyClassFixed<BBoxProperty>().data() = {{-0.5f, 0, -0.5f},
                                                                       {0.5f,  1, 0.5f}};
        freeEntity.setProperty("mass", std::unique_ptr<Property<double>>(massProp.copy()));

        domain->addEntity(freeEntity);

        while (time < 5) {
            time += tickSize;
            domain->tick(tickSize, res);
        }

        ASSERT_FUZZY_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y(), 1.0f, 0.1f);

        //Make the bbox larger and test that it adjust itself against the terrain.
        bBoxProperty.data() = {{-1, 0, -1},
                               {1,  2, 1}};
        bBoxProperty.apply(plantedEntity);

        domain->test_childEntityPropertyApplied("bbox", bBoxProperty, plantedEntity.getIntId());

        domain->tick(1.0f, res);

        callback = btCollisionWorld::ClosestRayResultCallback(from, to);
        domain->test_getPhysicalWorld()->rayTest(from, to, callback);

        ASSERT_TRUE(callback.hasHit());
        ASSERT_FUZZY_EQUAL(2.0f, callback.m_hitPointWorld.y(), 0.1f);

        domain->removeEntity(freeEntity);

        //Test again with the falling box
        freeEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(0, 10, 0);

        domain->addEntity(freeEntity);

        time = 0;
        while (time < 5) {
            time += tickSize;
            domain->tick(tickSize, res);
        }

        ASSERT_FUZZY_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y(), 2.0f, 0.1f);

    }

    void test_deleteWithPlanted(TestContext& context)
    {

        //Place four boxes, all "planted", and all on top of each others.
        //Now first delete the third box from the bottom. The top box should now be placed on the second box.
        //Then delete the first box. The second box should now be placed on the ground, and the top box should be on top of it.

        Entity rootEntity(context.newId());
        rootEntity.incRef();
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = {{-64, 0,  -64},
                                                                       {64,  64, 64}};
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        ModeProperty plantedProperty{};
        plantedProperty.set("planted");
        Property<double> massProp{};
        massProp.data() = 10000;

        OpVector res;

        Entity planted1(context.newId());
        planted1.requirePropertyClassFixed<PositionProperty>().data() = {1, 1, 0};
        planted1.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
        planted1.requirePropertyClassFixed<BBoxProperty>().data() = {{-2, 0, -2},
                                                                     {2,  1, 2}};
        planted1.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(plantedProperty.copy()));

        domain->addEntity(planted1);

        domain->tick(0, res);

        ASSERT_EQUAL(rootEntity.getIntId(), *planted1.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId)

        Entity planted2(context.newId());
        planted2.requirePropertyClassFixed<PositionProperty>().data() = {0, 2, 1};
        planted2.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
        planted2.requirePropertyClassFixed<BBoxProperty>().data() = {{-2, 0, -2},
                                                                     {2,  1, 2}};
        planted2.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(plantedProperty.copy()));

        domain->addEntity(planted2);

        domain->tick(0, res);

        ASSERT_EQUAL(planted1.getIntId(), *planted2.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId)
        ASSERT_FUZZY_EQUAL(1.0f, planted2.requirePropertyClassFixed<PositionProperty>().data().y(), 0.1f);

        Entity planted3(context.newId());
        planted3.requirePropertyClassFixed<PositionProperty>().data() = {0, 4, 1};
        planted3.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
        planted3.requirePropertyClassFixed<BBoxProperty>().data() = {{-2, 0, -2},
                                                                     {2,  1, 2}};
        planted3.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(plantedProperty.copy()));

        domain->addEntity(planted3);

        domain->tick(0, res);

        ASSERT_EQUAL(planted2.getIntId(), *planted3.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId)
        ASSERT_FUZZY_EQUAL(2.0f, planted3.requirePropertyClassFixed<PositionProperty>().data().y(), 0.1f);

        Entity planted4(context.newId());
        planted4.requirePropertyClassFixed<PositionProperty>().data() = {0, 6, 1};
        planted4.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
        planted4.requirePropertyClassFixed<BBoxProperty>().data() = {{-2, 0, -2},
                                                                     {2,  1, 2}};
        planted4.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(plantedProperty.copy()));

        domain->addEntity(planted4);

        domain->tick(0, res);

        ASSERT_EQUAL(planted3.getIntId(), *planted4.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId)
        ASSERT_FUZZY_EQUAL(3.0f, planted4.requirePropertyClassFixed<PositionProperty>().data().y(), 0.1f);

        //Now delete planted3, which should place planted4 on top of planted2

        domain->removeEntity(planted3);
        domain->tick(0, res);

        ASSERT_EQUAL(planted2.getIntId(), *planted4.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId)
        ASSERT_FUZZY_EQUAL(2.0f, planted4.requirePropertyClassFixed<PositionProperty>().data().y(), 0.1f);

        domain->removeEntity(planted1);
        domain->tick(0, res);

        ASSERT_EQUAL(rootEntity.getIntId(), *planted2.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId)
        ASSERT_FUZZY_EQUAL(0.0f, planted2.requirePropertyClassFixed<PositionProperty>().data().y(), 0.1f);
        ASSERT_EQUAL(planted2.getIntId(), *planted4.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId)
        ASSERT_FUZZY_EQUAL(1.0f, planted4.requirePropertyClassFixed<PositionProperty>().data().y(), 0.1f);
    }


    void test_convert(TestContext& context)
    {
        WFMath::AxisBox<3> wfBox(WFMath::Point<3>(-1, -3, -5), WFMath::Point<3>(1, 3, 5));

        auto wfSize = wfBox.highCorner() - wfBox.lowCorner();
        btVector3 btSize = Convert::toBullet(wfSize);

        ASSERT_EQUAL(btSize.x(), wfSize.x());
        ASSERT_EQUAL(btSize.y(), wfSize.y());
        ASSERT_EQUAL(btSize.z(), wfSize.z());

        WFMath::Quaternion wfQuat;
        wfQuat.rotation(1, -WFMath::numeric_constants<float>::pi() / 2.0f);
        btQuaternion btQuat = Convert::toBullet(wfQuat);

        ASSERT_FUZZY_EQUAL(wfQuat.scalar(), btQuat.getW(), epsilon);
        ASSERT_FUZZY_EQUAL(wfQuat.vector().x(), btQuat.getX(), epsilon);
        ASSERT_FUZZY_EQUAL(wfQuat.vector().y(), btQuat.getY(), epsilon);
        ASSERT_FUZZY_EQUAL(wfQuat.vector().z(), btQuat.getZ(), epsilon);


        //Now create a box, rotate it and see that the values match.

        btBoxShape btBox(btSize / 2);

        auto wfHighCorner = wfBox.highCorner();
        wfHighCorner.rotate(wfQuat, WFMath::Point<3>::ZERO());
        auto wfLowCorner = wfBox.lowCorner();
        wfLowCorner.rotate(wfQuat, WFMath::Point<3>::ZERO());

        btTransform transform(btQuat);

        btVector3 minAabb, maxAabb;
        btBox.getAabb(transform, minAabb, maxAabb);

        wfBox.highCorner().x() = std::max(wfHighCorner.x(), wfLowCorner.x());
        wfBox.highCorner().y() = std::max(wfHighCorner.y(), wfLowCorner.y());
        wfBox.highCorner().z() = std::max(wfHighCorner.z(), wfLowCorner.z());

        wfBox.lowCorner().x() = std::min(wfHighCorner.x(), wfLowCorner.x());
        wfBox.lowCorner().y() = std::min(wfHighCorner.y(), wfLowCorner.y());
        wfBox.lowCorner().z() = std::min(wfHighCorner.z(), wfLowCorner.z());

        ASSERT_FUZZY_EQUAL(wfBox.highCorner().x(), maxAabb.x(), 0.01);
        ASSERT_FUZZY_EQUAL(wfBox.highCorner().y(), maxAabb.y(), 0.01);
        ASSERT_FUZZY_EQUAL(wfBox.highCorner().z(), maxAabb.z(), 0.01);

        ASSERT_FUZZY_EQUAL(wfBox.lowCorner().x(), minAabb.x(), 0.01);
        ASSERT_FUZZY_EQUAL(wfBox.lowCorner().y(), minAabb.y(), 0.01);
        ASSERT_FUZZY_EQUAL(wfBox.lowCorner().z(), minAabb.z(), 0.01);

    }

    void test_movePlantedAndResting(TestContext& context)
    {

        //Place a box, "planted". On top of that, place another box, also "planted". And on top of that, place a box which is "free".
        //Then move the first box. The two boxes on top should move along with it.

        Entity rootEntity(context.newId());
        rootEntity.incRef();
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = {{-64, 0,  -64},
                                                                       {64,  64, 64}};
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        ModeProperty fixedProperty{};
        fixedProperty.set("fixed");
        ModeProperty plantedProperty{};
        plantedProperty.set("planted");
        ModeProperty freeProperty{};
        freeProperty.set("free");

        Property<double> massProp{};
        massProp.data() = 10000;

        Entity fixed1(context.newId());
        fixed1.requirePropertyClassFixed<PositionProperty>().data() = {0, 0, 0};
        fixed1.requirePropertyClassFixed<BBoxProperty>().data() = {{-2, -1, -2},
                                                                   {2,  1,  2}};
        fixed1.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
        fixed1.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(fixedProperty.copy()));

        domain->addEntity(fixed1);

        OpVector res;
        domain->tick(0, res);

        ASSERT_EQUAL(0, fixed1.requirePropertyClassFixed<PositionProperty>().data().y());


        Entity planted1(context.newId());
        planted1.requirePropertyClassFixed<PositionProperty>().data() = {1, 1, 0};
        planted1.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
        planted1.requirePropertyClassFixed<BBoxProperty>().data() = {{-2, -1, -2},
                                                                     {2,  1,  2}};
        planted1.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(plantedProperty.copy()));

        domain->addEntity(planted1);

        domain->tick(0, res);

        Entity planted2(context.newId());
        planted2.requirePropertyClassFixed<PositionProperty>().data() = {0, 2, 1};
        planted2.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
        planted2.requirePropertyClassFixed<BBoxProperty>().data() = {{-2, -1, -2},
                                                                     {2,  1,  2}};
        planted2.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(plantedProperty.copy()));

        domain->addEntity(planted2);

        domain->tick(0, res);

        ASSERT_FUZZY_EQUAL(2.0f, planted2.requirePropertyClassFixed<PositionProperty>().data().y(), 0.1f);

        Entity freeEntity(context.newId());
        freeEntity.requirePropertyClassFixed<PositionProperty>().data() = {1, 4, 0};
        freeEntity.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
        freeEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(freeProperty.copy()));
        freeEntity.requirePropertyClassFixed<BBoxProperty>().data() = {{-1, -1, -1},
                                                                       {1,  1,  1}};
        freeEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        domain->addEntity(freeEntity);

        ASSERT_FUZZY_EQUAL(4.0f, freeEntity.requirePropertyClassFixed<PositionProperty>().data().y(), 0.1);
        domain->tick(1, res);

        ASSERT_FUZZY_EQUAL(4.0f, freeEntity.requirePropertyClassFixed<PositionProperty>().data().y(), 0.1);

        //Only change position
        {
            std::set<LocatedEntity*> transformedEntities;

            domain->applyTransform(fixed1, Domain::TransformData{WFMath::Quaternion(), {10, 10, 10}, nullptr, {}}, transformedEntities);
            ASSERT_EQUAL(4u, transformedEntities.size());
            ASSERT_FUZZY_EQUAL(WFMath::Point<3>(11, 11, 10), planted1.requirePropertyClassFixed<PositionProperty>().data(), epsilon);
            ASSERT_FUZZY_EQUAL(WFMath::Point<3>(10, 12, 11), planted2.requirePropertyClassFixed<PositionProperty>().data(), epsilon);
            ASSERT_TRUE(WFMath::Equal(WFMath::Point<3>(11, 14, 10), freeEntity.requirePropertyClassFixed<PositionProperty>().data(), 0.1));
        }
        domain->tick(0, res);


        //Only change orientation
        {
            std::set<LocatedEntity*> transformedEntities;

            domain->applyTransform(fixed1, Domain::TransformData{WFMath::Quaternion(1, WFMath::numeric_constants<float>::pi() / 2), {}, nullptr, {}}, transformedEntities);
            ASSERT_EQUAL(4u, transformedEntities.size());
            ASSERT_FUZZY_EQUAL(WFMath::Point<3>(10, 11, 9), planted1.requirePropertyClassFixed<PositionProperty>().data(), epsilon);
            ASSERT_FUZZY_EQUAL(WFMath::Point<3>(11, 12, 10), planted2.requirePropertyClassFixed<PositionProperty>().data(), epsilon);
            ASSERT_TRUE(WFMath::Equal(WFMath::Point<3>(10, 14, 9), freeEntity.requirePropertyClassFixed<PositionProperty>().data(), 0.1));
        }
        domain->tick(0, res);

        //Move it, and at the same time rotate it 90 degrees around the y axis.
        {
            std::set<LocatedEntity*> transformedEntities;


            domain->applyTransform(fixed1, Domain::TransformData{WFMath::Quaternion(1, WFMath::numeric_constants<float>::pi()), {15, 15, 15}, nullptr, {}}, transformedEntities);
            ASSERT_EQUAL(4u, transformedEntities.size());
            ASSERT_FUZZY_EQUAL(WFMath::Point<3>(14, 16, 15), planted1.requirePropertyClassFixed<PositionProperty>().data(), epsilon);
            ASSERT_FUZZY_EQUAL(WFMath::Point<3>(15, 17, 14), planted2.requirePropertyClassFixed<PositionProperty>().data(), epsilon);
            ASSERT_TRUE(WFMath::Equal(WFMath::Point<3>(14, 19, 15), freeEntity.requirePropertyClassFixed<PositionProperty>().data(), 0.1));
        }
        domain->tick(0, res);

        //Move away the first planted entity, which should also move along the second planted and the free entity, and not affect the fixed entity.
        {
            std::set<LocatedEntity*> transformedEntities;

            domain->applyTransform(planted1, Domain::TransformData{{},
                                                                   {20, 0, 20},
                                                                   nullptr, {}}, transformedEntities);
            ASSERT_EQUAL(3u, transformedEntities.size());
            ASSERT_FUZZY_EQUAL(WFMath::Point<3>(21, 1, 19), planted2.requirePropertyClassFixed<PositionProperty>().data(), epsilon);
            ASSERT_TRUE(WFMath::Equal(WFMath::Point<3>(20, 3, 20), freeEntity.requirePropertyClassFixed<PositionProperty>().data(), 0.1));
            ASSERT_FUZZY_EQUAL(WFMath::Point<3>(15, 15, 15), fixed1.requirePropertyClassFixed<PositionProperty>().data(), epsilon);
        }

        {
            std::set<LocatedEntity*> transformedEntities;
            domain->applyTransform(fixed1, Domain::TransformData{WFMath::Quaternion(), {5, 20, 5}, nullptr, {}}, transformedEntities);
            ASSERT_EQUAL(1u, transformedEntities.size());
        }

        //Remove the second planted entity, making sure that the first planted doesn't keep a reference
        domain->removeEntity(planted2);
        {
            std::set<LocatedEntity*> transformedEntities;

            domain->applyTransform(planted1, Domain::TransformData{WFMath::Quaternion(), {15, 0, 20}, nullptr, {}}, transformedEntities);
            ASSERT_EQUAL(1u, transformedEntities.size());
        }
    }


    void test_plantedOn(TestContext& context)
    {
        std::vector<std::string> shapes{"box", "cylinder-x", "cylinder-y", "cylinder-z", "capsule-x", "capsule-y", "capsule-z"};

        for (auto plantedShape : shapes) {
            for (auto plantedOnTopShape : shapes) {

                auto id = context.newId();
                Entity rootEntity{id};
                rootEntity.incRef();
                TerrainProperty terrainProperty{};
                rootEntity.setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty.copy()));
                Mercator::Terrain& terrain = terrainProperty.getData(rootEntity);
                terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
                terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
                terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
                terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
                rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
                rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = {{-64, -64, -64},
                                                                               {64,  64,  64}};
                TestPhysicalDomain domain{rootEntity};


                Entity planted1(context.newId());
                planted1.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(0, 10, 0);
                planted1.requirePropertyClassFixed<BBoxProperty>().data() = {{-1, -1, -1},
                                                                             {1,  1,  1}};
                {
                    GeometryProperty plantedGeometryProperty{};
                    plantedGeometryProperty.set(MapType{{"type", plantedShape}});
                    ModeProperty modeProperty{};
                    modeProperty.set("planted");

                    planted1.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty.copy()));
                    planted1.setProperty(GeometryProperty::property_name, std::unique_ptr<PropertyBase>(plantedGeometryProperty.copy()));
                }

                domain.addEntity(planted1);

                OpVector res;
                domain.tick(0, res);

                ASSERT_TRUE(planted1.getPropertyClassFixed<ModeDataProperty>())
                ASSERT_TRUE(planted1.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId)
                ASSERT_EQUAL(rootEntity.getIntId(), *planted1.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId)
                ASSERT_FUZZY_EQUAL(10, planted1.requirePropertyClassFixed<PositionProperty>().data().y(), 0.1);
                {
                    auto* planted1RigidBody = domain.test_getRigidBody(planted1.getIntId());
                    btVector3 aabbMin, aabbMax;
                    planted1RigidBody->getAabb(aabbMin, aabbMax);
                    ASSERT_FUZZY_EQUAL_FN(aabbMin.y(), 9, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1'.", plantedShape)); });
                    ASSERT_FUZZY_EQUAL_FN(aabbMax.y(), 11, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1'.", plantedShape)); });

                }

                Entity planted2(context.newId());
                planted2.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(0, 15, 0);
                planted2.requirePropertyClassFixed<BBoxProperty>().data() = {{-1, -1, -1},
                                                                             {1,  1,  1}};
                {
                    GeometryProperty plantedGeometryProperty{};
                    plantedGeometryProperty.set(MapType{{"type", plantedShape}});
                    ModeProperty modeProperty{};
                    modeProperty.set("planted");

                    planted2.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty.copy()));
                    planted2.setProperty(GeometryProperty::property_name, std::unique_ptr<PropertyBase>(plantedGeometryProperty.copy()));
                }

                domain.addEntity(planted2);

                domain.tick(0, res);

                ASSERT_TRUE(planted2.getPropertyClassFixed<ModeDataProperty>());
                ASSERT_TRUE(planted2.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId);
                ASSERT_EQUAL(planted1.getIntId(), *planted2.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId);
                ASSERT_FUZZY_EQUAL(11, planted2.requirePropertyClassFixed<PositionProperty>().data().y(), 0.1);
                {
                    auto* planted2RigidBody = domain.test_getRigidBody(planted2.getIntId());
                    btVector3 aabbMin, aabbMax;
                    planted2RigidBody->getAabb(aabbMin, aabbMax);
                    ASSERT_FUZZY_EQUAL_FN(aabbMin.y(), 10, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1'.", plantedShape)); });
                    ASSERT_FUZZY_EQUAL_FN(aabbMax.y(), 12, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1'.", plantedShape)); });

                }


                Entity plantedOn(context.newId());
                plantedOn.requirePropertyClassFixed<PositionProperty>().data() = {0, 15, 0};
                {
                    ModeProperty modeProperty{};
                    modeProperty.set("planted");
                    plantedOn.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty.copy()));
                }
                plantedOn.requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                                                              {1,  1, 1}};
                ModeDataProperty modeDataProperty{};
                modeDataProperty.setPlantedData({planted1.getIntId()});
                plantedOn.setProperty(ModeDataProperty::property_name, std::unique_ptr<PropertyBase>(modeDataProperty.copy()));

                GeometryProperty geometryProperty{};
                geometryProperty.set(MapType{{"type", plantedOnTopShape}});
                plantedOn.setProperty(GeometryProperty::property_name, std::unique_ptr<PropertyBase>(geometryProperty.copy()));

                domain.addEntity(plantedOn);


                ASSERT_TRUE(plantedOn.getPropertyClassFixed<ModeDataProperty>());
                ASSERT_TRUE(plantedOn.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId);
                ASSERT_EQUAL(planted1.getIntId(), *plantedOn.getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entityId);
                ASSERT_FUZZY_EQUAL_FN(plantedOn.requirePropertyClassFixed<PositionProperty>().data().y(), 11, 0.1,
                                      [&]() { this->addFailure(String::compose("Using shape '%1' on top of '%2'.", plantedOnTopShape, plantedShape)); });
                {
                    auto* plantedOnRigidBody = domain.test_getRigidBody(plantedOn.getIntId());
                    btVector3 aabbMin, aabbMax;
                    plantedOnRigidBody->getAabb(aabbMin, aabbMax);
                    ASSERT_FUZZY_EQUAL_FN(aabbMin.y(), 11, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1' on top of '%2'.", plantedOnTopShape, plantedShape)); });
                    ASSERT_FUZZY_EQUAL_FN(aabbMax.y(), 12, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1' on top of '%2'.", plantedOnTopShape, plantedShape)); });
                }

                domain.removeEntity(planted2);
                domain.removeEntity(planted1);
                domain.removeEntity(plantedOn);

            }
        }
    }

    void test_terrainMods(TestContext& context)
    {

        Entity rootEntity(context.newId());
        rootEntity.incRef();
        TerrainProperty terrainProperty{};
        rootEntity.setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty.copy()));
        Mercator::Terrain& terrain = terrainProperty.getData(rootEntity);
        terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
        terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        ModeProperty modePropertyBase{};
        modePropertyBase.set("planted");

        Entity terrainModEntity(context.newId());
        terrainModEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(32, 10, 32);
        auto modeProperty = terrainModEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePropertyBase.copy()));
        TerrainModProperty terrainModProperty{};

        Atlas::Message::MapType modElement{
                {"heightoffset", -5.0f},
                {"shape",        MapType{
                        {"points", ListType{
                                ListType{-10.f, -10.f},
                                ListType{10.f, -10.f},
                                ListType{10.f, 10.f},
                                ListType{-10.f, 10.f},
                        }
                        },
                        {"type",   "polygon"}
                }
                },
                {"type",         "levelmod"}
        };

        terrainModProperty.set(modElement);
        terrainModProperty.apply(terrainModEntity);
        terrainModEntity.setProperty(TerrainModProperty::property_name, std::unique_ptr<PropertyBase>(terrainModProperty.copy()));

        domain->addEntity(terrainModEntity);

        OpVector res;
        std::set<LocatedEntity*> transformedEntities;

        domain->tick(0, res);


        ASSERT_FUZZY_EQUAL(terrain.get(10, 10), 10.0f, 0.1f);
        ASSERT_TRUE(terrain.hasMod(terrainModEntity.getIntId()));
        ASSERT_FUZZY_EQUAL(terrain.get(32, 32), 5.0f, 0.1f);


        {
            btVector3 rayFrom(32, 32, 32);
            btVector3 rayTo(32, -32, 32);
            btCollisionWorld::ClosestRayResultCallback callback(rayFrom, rayTo);
            domain->test_getPhysicalWorld()->rayTest(rayFrom, rayTo, callback);

            ASSERT_FUZZY_EQUAL(callback.m_hitPointWorld.y(), 5.0f, 0.1f);
        }
        domain->applyTransform(terrainModEntity, Domain::TransformData{WFMath::Quaternion(), WFMath::Point<3>(10, 10, 10), nullptr, {}}, transformedEntities);

        domain->tick(0, res);

        ASSERT_FUZZY_EQUAL(terrain.get(10, 10), 5.0f, 0.1f);
        ASSERT_TRUE(terrain.hasMod(terrainModEntity.getIntId()));
        ASSERT_FUZZY_EQUAL(terrain.get(32, 32), 10.0f, 0.1f);


        {
            btVector3 rayFrom(32, 32, 32);
            btVector3 rayTo(32, -32, 32);
            btCollisionWorld::ClosestRayResultCallback callback(rayFrom, rayTo);
            domain->test_getPhysicalWorld()->rayTest(rayFrom, rayTo, callback);

            ASSERT_FUZZY_EQUAL(callback.m_hitPointWorld.y(), 10.0f, 0.1f);
        }

        //Now change "mode" to "free", which should remove the mod.

        modeProperty->set("free");
        modeProperty->apply(terrainModEntity);
        terrainModEntity.propertyApplied.emit("mode", *modeProperty);

        domain->tick(0, res);

        ASSERT_FUZZY_EQUAL(terrain.get(10, 10), 10.0f, 0.1f);
        ASSERT_FALSE(terrain.hasMod(terrainModEntity.getIntId()));
        ASSERT_FUZZY_EQUAL(terrain.get(32, 32), 10.0f, 0.1f);

        //And back to "planted" which should bring it back
        modeProperty->set("planted");
        modeProperty->apply(terrainModEntity);
        terrainModEntity.propertyApplied.emit("mode", *modeProperty);

        domain->tick(0, res);

        ASSERT_FUZZY_EQUAL(terrain.get(10, 10), 5.0f, 0.1f);
        ASSERT_TRUE(terrain.hasMod(terrainModEntity.getIntId()));
        ASSERT_FUZZY_EQUAL(terrain.get(32, 32), 10.0f, 0.1f);
    }


    void test_lake_rotated(TestContext& context)
    {
        struct TestEntity : public Entity
        {
            explicit TestEntity(long intId) : Entity(intId)
            {
            }

            explicit TestEntity(RouterId id) : Entity(id)
            {
            }

            decltype(LocatedEntity::propertyApplied)& test_propertyApplied()
            {
                return propertyApplied;
            }
        };

        TypeNode rockType("rock");
        TypeNode lakeType("lake");

        Property<double> massProp{};
        massProp.data() = 10000;

        BoolProperty waterBodyProp{};
        waterBodyProp.set(1);

        ModeProperty modeFreeProperty{};
        modeFreeProperty.set("free");
        rockType.injectProperty("mode", std::unique_ptr<PropertyBase>(modeFreeProperty.copy()));

        ModeProperty modeFixedProperty{};
        modeFixedProperty.set("fixed");


        Entity rootEntity(context.newId());
        rootEntity.incRef();
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, 0, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        TestEntity lake(context.newId());
        lake.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeFixedProperty.copy()));
        lake.setType(&lakeType);
        lake.setProperty("water_body", std::unique_ptr<PropertyBase>(waterBodyProp.copy()));
        lake.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(0, -64, 0), WFMath::Point<3>(10, 0, 2));
        lake.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(0, 10, 0);
        //rotate 90 degrees
        lake.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion(1, -WFMath::numeric_constants<float>::pi() / 2.0f);
        domain->addEntity(lake);

        //Should be in water
        Entity freeEntity(RouterId("freeEntity", context.newId()));
        freeEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity.setType(&rockType);
        freeEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(-1, 1, 9);
        freeEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(freeEntity);

        //Should not be in water
        Entity freeEntity2(RouterId("freeEntity2", context.newId()));
        freeEntity2.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity2.setType(&rockType);
        freeEntity2.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(9, 1, 1);
        freeEntity2.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(freeEntity2);

        ModeProperty plantedProp{};
        plantedProp.set("planted");
        ModeDataProperty modeDataProp{};
        modeDataProp.setPlantedData({lake.getIntId()});
        Entity floatingEntity(RouterId("floatingEntity", context.newId()));
        floatingEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(plantedProp.copy()));
        floatingEntity.setProperty(ModeDataProperty::property_name, std::unique_ptr<PropertyBase>(modeDataProp.copy()));

        floatingEntity.setType(&rockType);
        floatingEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(5, 20, 1);
        floatingEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(floatingEntity);

        OpVector res;
        domain->tick(0, res);
        ASSERT_TRUE(freeEntity.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);
        ASSERT_TRUE(freeEntity2.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);
        ASSERT_TRUE(floatingEntity.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Planted);
        ASSERT_EQUAL(WFMath::Point<3>(5, 10, 1), floatingEntity.requirePropertyClassFixed<PositionProperty>().data());

    }


    void test_lake(TestContext& context)
    {
        struct TestEntity : public Entity
        {
            explicit TestEntity(long intId) : Entity(intId)
            {
            }

            explicit TestEntity(RouterId id) : Entity(id)
            {
            }

            decltype(LocatedEntity::propertyApplied)& test_propertyApplied()
            {
                return propertyApplied;
            }
        };

        double tickSize = 1.0 / 15.0;
        double time = 0;

        TypeNode rockType("rock");
        TypeNode lakeType("lake");

        Property<double> massProp{};
        massProp.data() = 10000;

        BoolProperty waterBodyProp{};
        waterBodyProp.set(1);

        ModeProperty modeFreeProperty{};
        modeFreeProperty.set("free");
        rockType.injectProperty("mode", std::unique_ptr<PropertyBase>(modeFreeProperty.copy()));

        ModeProperty modeFixedProperty{};
        modeFixedProperty.set("fixed");


        Entity rootEntity(context.newId());
        rootEntity.incRef();
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        TestEntity lake(context.newId());
        lake.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeFixedProperty.copy()));
        lake.setType(&lakeType);
        lake.setProperty("water_body", std::unique_ptr<PropertyBase>(waterBodyProp.copy()));
        lake.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-5, -64, -5), WFMath::Point<3>(5, 0, 5));
        lake.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(20, 0, 0);
        lake.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
        domain->addEntity(lake);

        Entity freeEntity(context.newId());
        freeEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity.setType(&rockType);
        freeEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(20, 2, 0);
        freeEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(freeEntity);

        //The second entity is placed in water, and should be submerged from the start
        Entity freeEntity2(context.newId());
        freeEntity2.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity2.setType(&rockType);
        freeEntity2.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(20, -2, 2);
        freeEntity2.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(freeEntity2);

        //The third entity is placed outside of the lake, and should never be submerged.
        Entity freeEntity3(context.newId());
        freeEntity3.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity3.setType(&rockType);
        freeEntity3.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(-20, 2, 0);
        freeEntity3.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(freeEntity3);

        OpVector res;
        std::set<LocatedEntity*> transformedEntities;
        domain->tick(0, res);
        while (time < 5) {
            time += tickSize;
            domain->tick(tickSize, res);
        }

        ASSERT_TRUE(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y() < 0);
        ASSERT_TRUE(freeEntity.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);

        ASSERT_TRUE(freeEntity2.requirePropertyClassFixed<PositionProperty>().data().y() < 0);
        ASSERT_TRUE(freeEntity2.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);

        ASSERT_TRUE(freeEntity3.requirePropertyClassFixed<PositionProperty>().data().y() < 0);
        ASSERT_TRUE(freeEntity3.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

        //Move outside
        domain->applyTransform(freeEntity, Domain::TransformData{WFMath::Quaternion::IDENTITY(), WFMath::Point<3>(20, 60, 0), nullptr, {}}, transformedEntities);
        domain->tick(0, res);
        ASSERT_GREATER(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y(), 0);
        ASSERT_TRUE(freeEntity.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

        //Move back in.
        domain->applyTransform(freeEntity, Domain::TransformData{WFMath::Quaternion::IDENTITY(), WFMath::Point<3>(20, -10, 0), nullptr, {}}, transformedEntities);
        domain->tick(0, res);
        ASSERT_LESS(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y(), 0);
        ASSERT_TRUE(freeEntity.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);
        ASSERT_TRUE(freeEntity3.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

        //Move the lake to where freeEntity3 is
        domain->applyTransform(lake, Domain::TransformData{WFMath::Quaternion(), freeEntity3.requirePropertyClassFixed<PositionProperty>().data() + WFMath::Vector<3>(0, 5, 0), nullptr, {}},
                               transformedEntities);
        domain->tick(0, res);
        ASSERT_TRUE(freeEntity3.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);
        ASSERT_TRUE(freeEntity.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

        //Update the bbox of the lake so that it's outside of freeEntity3.
        //To emulate the propertyApplied signal being called in this test we need to do it ourselves.
        auto newBbox = lake.requirePropertyClassFixed<BBoxProperty>().data();
        newBbox.highCorner().x() = 1;
        newBbox.highCorner().y() = 1;
        newBbox.highCorner().z() = 1;
        newBbox.lowCorner().x() = -1;
        newBbox.lowCorner().y() = -1;
        newBbox.lowCorner().z() = -1;

        BBoxProperty bBoxProperty{};
        bBoxProperty.set(newBbox.toAtlas());
        lake.setProperty("bbox", std::unique_ptr<PropertyBase>(bBoxProperty.copy()));
        bBoxProperty.apply(lake);
        lake.test_propertyApplied().emit("bbox", bBoxProperty);

        domain->tick(0, res);
        ASSERT_TRUE(freeEntity3.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);
        ASSERT_TRUE(freeEntity.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

        domain->removeEntity(lake);
        domain->tick(0, res);
        ASSERT_TRUE(freeEntity.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);
        ASSERT_TRUE(freeEntity2.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);
        ASSERT_TRUE(freeEntity3.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

    }

    void test_ocean(TestContext& context)
    {
        double tickSize = 1.0 / 15.0;
        double time = 0;

        TypeNode rockType("rock");
        TypeNode oceanType("ocean");

        Property<double> massProp{};
        massProp.data() = 10000;

        BoolProperty waterBodyProp{};
        waterBodyProp.set(1);

        ModeProperty modeFreeProperty{};
        modeFreeProperty.set("free");
        ModeProperty modeFixedProperty{};
        modeFixedProperty.set("fixed");

        rockType.injectProperty("mode", std::unique_ptr<PropertyBase>(modeFreeProperty.copy()));


        Entity rootEntity(context.newId());
        rootEntity.incRef();
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        Entity ocean(context.newId());
        ocean.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeFixedProperty.copy()));
        ocean.setType(&oceanType);
        ocean.setProperty("water_body", std::unique_ptr<PropertyBase>(waterBodyProp.copy()));
        ocean.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(0, 0, 0);
        ocean.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
        domain->addEntity(ocean);

        Entity freeEntity(context.newId());
        freeEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity.setType(&rockType);
        freeEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(0, 2, 0);
        freeEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(freeEntity);

        //The second entity is placed in water, and should be submerged from the start
        Entity freeEntity2(context.newId());
        freeEntity2.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity2.setType(&rockType);
        freeEntity2.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(10, -10, 0);
        freeEntity2.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(freeEntity2);


        OpVector res;
        std::set<LocatedEntity*> transformedEntities;
        domain->tick(0, res);
        while (time < 5) {
            time += tickSize;
            domain->tick(tickSize, res);
        }

        ASSERT_LESS(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y(), 0)
        ASSERT_TRUE(freeEntity.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);

        ASSERT_LESS(freeEntity2.requirePropertyClassFixed<PositionProperty>().data().y(), 0);
        ASSERT_TRUE(freeEntity2.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);

        //Move outside
        domain->applyTransform(freeEntity, Domain::TransformData{WFMath::Quaternion::IDENTITY(), WFMath::Point<3>(0, 60, 0), nullptr, {}}, transformedEntities);
        domain->tick(0, res);
        ASSERT_TRUE(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y() > 0);
        ASSERT_TRUE(freeEntity.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

        //Move back in.
        domain->applyTransform(freeEntity, Domain::TransformData{WFMath::Quaternion::IDENTITY(), WFMath::Point<3>(0, -10, 0), nullptr, {}}, transformedEntities);
        domain->tick(0, res);
        ASSERT_TRUE(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y() < 0);
        ASSERT_TRUE(freeEntity.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);

        domain->removeEntity(ocean);
        domain->tick(0, res);
        ASSERT_TRUE(freeEntity.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);
        ASSERT_TRUE(freeEntity2.getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);
    }


    void test_placement(TestContext& context)
    {

        TypeNode rockType("rock");

        Property<double> massProp{};
        massProp.data() = 10000;

        ModeProperty modeProperty{};
        modeProperty.set("fixed");


        Entity rootEntity(context.newId());
        rootEntity.incRef();
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        // btDiscreteDynamicsWorld* bulletWorld = domain->test_getBulletWorld();

        auto verifyBboxes = [&](Entity& entity) {
            btRigidBody* rigidBody = domain->test_getRigidBody(entity.getIntId());
            btVector3 aabbMin, aabbMax;
            rigidBody->getAabb(aabbMin, aabbMax);

            //Get the final positions of the entity's bbox

            btVector3 expectedBtAabbMax(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
            btVector3 expectedBtAabbMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

            for (size_t i = 0; i < entity.requirePropertyClassFixed<BBoxProperty>().data().numCorners(); ++i) {
                WFMath::Point<3> point = entity.requirePropertyClassFixed<BBoxProperty>().data().getCorner(i);
                point.rotate(entity.requirePropertyClassFixed<OrientationProperty>().data(), WFMath::Point<3>::ZERO());

                point += WFMath::Vector<3>(entity.requirePropertyClassFixed<PositionProperty>().data());

                btVector3 btPoint = Convert::toBullet(point);

                expectedBtAabbMax.setX(std::max(expectedBtAabbMax.x(), btPoint.x()));
                expectedBtAabbMax.setY(std::max(expectedBtAabbMax.y(), btPoint.y()));
                expectedBtAabbMax.setZ(std::max(expectedBtAabbMax.z(), btPoint.z()));

                expectedBtAabbMin.setX(std::min(expectedBtAabbMin.x(), btPoint.x()));
                expectedBtAabbMin.setY(std::min(expectedBtAabbMin.y(), btPoint.y()));
                expectedBtAabbMin.setZ(std::min(expectedBtAabbMin.z(), btPoint.z()));

            }

            ASSERT_FUZZY_EQUAL(expectedBtAabbMax.x(), aabbMax.x(), 0.001);
            ASSERT_FUZZY_EQUAL(expectedBtAabbMax.y(), aabbMax.y(), 0.001);
            ASSERT_FUZZY_EQUAL(expectedBtAabbMax.z(), aabbMax.z(), 0.001);

            ASSERT_FUZZY_EQUAL(expectedBtAabbMin.x(), aabbMin.x(), 0.001);
            ASSERT_FUZZY_EQUAL(expectedBtAabbMin.y(), aabbMin.y(), 0.001);
            ASSERT_FUZZY_EQUAL(expectedBtAabbMin.z(), aabbMin.z(), 0.001);
        };


        auto performPlacementTests = [&](Entity& entity) {
            verifyBboxes(entity);
            std::set<LocatedEntity*> transformedEntities;

            //Change pos only
            domain->applyTransform(entity, Domain::TransformData{WFMath::Quaternion(), WFMath::Point<3>(20, 30, 1), nullptr, {}}, transformedEntities);

            verifyBboxes(entity);

            //Change orientation only
            domain->applyTransform(entity, Domain::TransformData{WFMath::Quaternion(1, WFMath::numeric_constants<float>::pi() / 3.0f), WFMath::Point<3>(), nullptr, {}},
                                   transformedEntities);

            verifyBboxes(entity);

            //Change pos and orientation
            domain->applyTransform(entity, Domain::TransformData{WFMath::Quaternion(1, WFMath::numeric_constants<float>::pi() / 5.0f), WFMath::Point<3>(10, -25, 6), nullptr, {}},
                                   transformedEntities);

            verifyBboxes(entity);

        };


        //Start with a box centered at origo, with no orientation
        {
            Entity entity(context.newId());
            entity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty.copy()));
            entity.setType(&rockType);
            entity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(10, -20, 1);
            entity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-6, -1, -2), WFMath::Point<3>(6, 1, 2));
            entity.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
            domain->addEntity(entity);

            performPlacementTests(entity);
        }

        //Start with a box centered at origo, with 45 degrees orientation
        {
            Entity entity(context.newId());
            entity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty.copy()));
            entity.setType(&rockType);
            entity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(10, -20, 1);
            entity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-6, -1, -2), WFMath::Point<3>(6, 1, 2));
            WFMath::Quaternion wfQuat;
            wfQuat.rotation(1, -WFMath::numeric_constants<float>::pi() / 4.0f);
            entity.requirePropertyClassFixed<OrientationProperty>().data() = wfQuat;
            domain->addEntity(entity);

            performPlacementTests(entity);
        }

        //A box not centered at origo, with no orientation
        {
            Entity entity(context.newId());
            entity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty.copy()));
            entity.setType(&rockType);
            entity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(10, -20, 1);
            entity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(0, 0, 0), WFMath::Point<3>(6, 1, 2));
            entity.requirePropertyClassFixed<OrientationProperty>().data() = WFMath::Quaternion::IDENTITY();
            domain->addEntity(entity);

            performPlacementTests(entity);
        }

        //A box not centered at origo, with 45 degrees orientation
        {
            Entity entity(context.newId());
            entity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty.copy()));
            entity.setType(&rockType);
            entity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(10, -20, 1);
            entity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(2, 0, 1), WFMath::Point<3>(6, 1, 2));
            WFMath::Quaternion wfQuat;
            wfQuat.rotation(1, -WFMath::numeric_constants<float>::pi() / 4.0f);
            entity.requirePropertyClassFixed<OrientationProperty>().data() = wfQuat;
            domain->addEntity(entity);

            performPlacementTests(entity);
        }
    }

    void test_fallToBottom(TestContext& context)
    {

        double tickSize = 1.0 / 15.0;
        double time = 0;

        Entity rootEntity(context.newId());
        rootEntity.incRef();
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        Property<double> massProp{};
        massProp.data() = 10000;

        TypeNode rockType("rock");


        Entity freeEntity(context.newId());
        freeEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity.setType(&rockType);
        freeEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        freeEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(freeEntity);

        Entity fixedEntity(context.newId());
        fixedEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));

        ModeProperty modeProperty{};
        modeProperty.set("fixed");
        fixedEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty.copy()));
        fixedEntity.setType(&rockType);
        fixedEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(10, 0, 10);
        fixedEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(fixedEntity);

        OpVector res;

        //First tick should not update anything
        domain->tick(0, res);
        ASSERT_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>::ZERO());
        ASSERT_EQUAL(fixedEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(10, 0, 10));

        //Inject enough ticks to move rock to bottom
        while (time < 5) {
            time += tickSize;
            domain->tick(tickSize, res);
        }
        ASSERT_FUZZY_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y(), -64, 0.1);
        //Fixed entity should not move
        ASSERT_EQUAL(fixedEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(10, 0, 10));
    }


    void test_standOnFixed(TestContext& context)
    {

        double tickSize = 1.0 / 15.0;
        double time = 0;
        Entity rootEntity(context.newId());
        rootEntity.incRef();
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        Property<double> massProp{};
        massProp.data() = 10000;

        TypeNode rockType("rock");


        Entity freeEntity(context.newId());
        freeEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity.setType(&rockType);
        freeEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(0, 1, 0);
        freeEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(freeEntity);

        Entity fixedEntity(context.newId());
        fixedEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));

        ModeProperty modeProperty{};
        modeProperty.set("fixed");
        fixedEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty.copy()));
        fixedEntity.setType(&rockType);
        fixedEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        fixedEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(fixedEntity);

        OpVector res;

        //Inject enough ticks to move rock to bottom
        while (time < 5) {
            time += tickSize;
            domain->tick(tickSize, res);
        }
        ASSERT_FUZZY_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(0, 1, 0), epsilon);
    }

    void test_fallToTerrain(TestContext& context)
    {

        double tickSize = 1.0 / 15.0;
        double time = 0;
        Entity rootEntity(context.newId());
        rootEntity.incRef();
        TerrainProperty terrainProperty{};
        rootEntity.setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty.copy()));
        Mercator::Terrain& terrain = terrainProperty.getData(rootEntity);
        terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
        terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        Property<double> massProp{};
        massProp.data() = 10000;

        TypeNode rockType("rock");


        Entity freeEntity(context.newId());
        freeEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity.setType(&rockType);
        freeEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(10, 20, 10);
        freeEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(freeEntity);

        Entity plantedEntity(context.newId());
        plantedEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));

        ModeProperty modeProperty{};
        modeProperty.set("planted");

        plantedEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty.copy()));
        plantedEntity.setType(&rockType);
        plantedEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(20, 20, 20);
        plantedEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(plantedEntity);

        ASSERT_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y(), 20);
        //Planted entity should be placed on the terrain when added to the domain.
        ASSERT_FUZZY_EQUAL(plantedEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(20, 10.0058, 20), epsilon);

        OpVector res;

        //Inject enough ticks to move rock to bottom
        while (time < 5) {
            time += tickSize;
            domain->tick(tickSize, res);
        }
        ASSERT_FUZZY_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y(), 10.0087f, 0.01f);
        //Planted entity should not move
        ASSERT_FUZZY_EQUAL(plantedEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(20, 10.0058, 20), epsilon);

        //If we now change the bbox of the free entity it should not fall through the terrain. The y-position should be the same
        freeEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-0.5, 0, -0.5), WFMath::Point<3>(0.5, 0.5, 0.5));
        PropertyBase* ptr{};
        freeEntity.propertyApplied("bbox", *ptr);
        ASSERT_FUZZY_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y(), 10.0087f, 0.01f);
        //Make sure that everything is correct even after ticking through the simulation
        domain->tick(tickSize, res);
        ASSERT_FUZZY_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data().y(), 10.0087f, 0.01f);

    }


    void test_collision(TestContext& context)
    {
        double tickSize = 1.0 / 15.0;

        Property<double> zeroFrictionProperty{};
        zeroFrictionProperty.data() = 0;
        Property<double> speedGroundProperty{};
        speedGroundProperty.data() = 5.0;


        Entity rootEntity(context.newId());
        rootEntity.incRef();
        TerrainProperty terrainProperty{};
        rootEntity.setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty.copy()));
        Mercator::Terrain& terrain = terrainProperty.getData(rootEntity);
        terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
        terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
        rootEntity.setProperty("friction", std::unique_ptr<PropertyBase>(zeroFrictionProperty.copy()));
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        Property<double> massProp{};
        massProp.data() = 100;

        TypeNode rockType("rock");


        PropelProperty propelProperty{};
        //Move y axis 2 meter per second.
        propelProperty.data() = WFMath::Vector<3>(0, 0, 2.0 / speedGroundProperty.data());

        AngularFactorProperty angularZeroFactorProperty{};
        angularZeroFactorProperty.data() = WFMath::Vector<3>::ZERO();


        Entity freeEntity(context.newId());
        freeEntity.setProperty(PropelProperty::property_name, std::unique_ptr<PropertyBase>(propelProperty.copy()));
        freeEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity.setProperty("friction", std::unique_ptr<PropertyBase>(zeroFrictionProperty.copy()));
        freeEntity.setProperty("speed_ground", std::unique_ptr<PropertyBase>(speedGroundProperty.copy()));
        freeEntity.setProperty(AngularFactorProperty::property_name, std::unique_ptr<PropertyBase>(angularZeroFactorProperty.copy()));
        freeEntity.setType(&rockType);
        freeEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(10, 10, 10);
        freeEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));

        domain->addEntity(freeEntity);

        Entity plantedEntity(context.newId());
        plantedEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));

        ModeProperty modeProperty{};
        modeProperty.set("planted");

        plantedEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty.copy()));
        plantedEntity.setType(&rockType);
        plantedEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(10, 10, 15);
        plantedEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(plantedEntity);
        const WFMath::Point<3> plantedPos = plantedEntity.requirePropertyClassFixed<PositionProperty>().data();

        OpVector res;

        domain->tick(tickSize, res);

        //Should have moved 2/15 meters
        ASSERT_FUZZY_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data().z(), 10 + (2.0 / 15.0), 0.1f);

        //Inject ticks for one second
        domain->tick(14.0 / 15.0, res);

        //Should have moved 2 meters in y axis
        ASSERT_FUZZY_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data().z(), 12, 0.1f);

        //Inject ticks for three seconds
        for (int i = 0; i < (3 * 15); ++i) {
            domain->tick(tickSize, res);
        }

        //Should have stopped at planted entity
        ASSERT_FUZZY_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data().z(), 13, 0.1f);
        ASSERT_EQUAL(plantedEntity.requirePropertyClassFixed<PositionProperty>().data(), plantedPos);

        domain->removeEntity(plantedEntity);
        domain->tick(1.0, res);

        //Should have moved two more meters as planted entity was removed.
        ASSERT_FUZZY_EQUAL(freeEntity.requirePropertyClassFixed<PositionProperty>().data().z(), 15, 0.1f);

    }


    void test_mode(TestContext& context)
    {

        double tickSize = 1.0 / 15.0;

        ModeProperty modePlantedProperty{};
        modePlantedProperty.set("planted");
        ModeProperty modeFixedProperty{};
        modeFixedProperty.set("fixed");
        ModeProperty modeFreeProperty{};
        modeFreeProperty.set("");
        TypeNode rockType("rock");

        Entity rootEntity(context.newId());
        rootEntity.incRef();
        TerrainProperty terrainProperty{};
        rootEntity.setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty.copy()));
        Mercator::Terrain& terrain = terrainProperty.getData(rootEntity);
        terrain.setBasePoint(0, 0, Mercator::BasePoint(40));
        terrain.setBasePoint(0, 1, Mercator::BasePoint(40));
        terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        Property<double> massProp{};
        massProp.data() = 100;

        Entity freeEntity1(RouterId("free1", context.newId()));
        freeEntity1.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity1.setType(&rockType);
        freeEntity1.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(10, 30, 10);
        freeEntity1.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(freeEntity1);

        ASSERT_EQUAL(freeEntity1.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(10, 30, 10));

        //The other free entity is placed below the terrain; it's expected to then be clamped to the terrain
        Entity freeEntity2(context.newId());
        freeEntity2.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        freeEntity2.setType(&rockType);
        freeEntity2.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(20, -10, 20);
        freeEntity2.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(freeEntity2);
        ASSERT_FUZZY_EQUAL(freeEntity2.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(20, 22.6006, 20), epsilon);

        Entity plantedEntity(context.newId());
        plantedEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        plantedEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty.copy()));
        plantedEntity.setType(&rockType);
        plantedEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(30, 10, 30);
        plantedEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(plantedEntity);
        ASSERT_FUZZY_EQUAL(plantedEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(30, 18.4325, 30), epsilon);


        Entity fixedEntity(context.newId());
        fixedEntity.setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
        fixedEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeFixedProperty.copy()));
        fixedEntity.setType(&rockType);
        fixedEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(40, 50, 40);
        fixedEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1));
        domain->addEntity(fixedEntity);
        ASSERT_FUZZY_EQUAL(fixedEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(40, 50, 40), epsilon);


        OpVector res;

        //Inject ticks for two seconds
        for (int i = 0; i < 30; ++i) {
            domain->tick(tickSize, res);
        }

        ASSERT_NOT_EQUAL(freeEntity1.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(10, 30, 10));
        ASSERT_NOT_EQUAL(freeEntity2.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(20, 22.6006, 20));
        ASSERT_FUZZY_EQUAL(plantedEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(30, 18.4325, 30), epsilon);
        ASSERT_FUZZY_EQUAL(fixedEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(40, 50, 40), epsilon);
    }


    void test_static_entities_no_move(TestContext& context)
    {}

    void test_determinism(TestContext& context)
    {

        double tickSize = 1.0 / 15.0;

        TypeNode rockType("rock");

        Entity rootEntity(context.newId());
        rootEntity.incRef();
        TerrainProperty terrainProperty{};
        rootEntity.setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty.copy()));
        Mercator::Terrain& terrain = terrainProperty.getData(rootEntity);
        terrain.setBasePoint(0, 0, Mercator::BasePoint(40));
        terrain.setBasePoint(0, 1, Mercator::BasePoint(40));
        terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(0, 0, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        Property<double> massProp{};
        massProp.data() = 100;

        std::vector<Ref<Entity>> entities;

        for (size_t i = 0; i < 10; ++i) {
            for (size_t j = 0; j < 10; ++j) {
                Ref<Entity> freeEntity(new Entity(context.newId()));
                freeEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
                freeEntity->setType(&rockType);
                freeEntity->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(i, j, i + j);
                freeEntity->requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-0.25f, 0, -0.25f), WFMath::Point<3>(-0.25f, 0.5f, -0.25f));
                domain->addEntity(*freeEntity);
                entities.push_back(freeEntity);
            }
        }

        OpVector res;

        //First tick is setup, so we'll exclude that from time measurement
        domain->tick(tickSize, res);
        //Inject ticks for two seconds
        for (int i = 0; i < 29; ++i) {
            domain->tick(tickSize, res);
        }

        //Sample a couple of selected entities
        //Note: this perhaps differs depending on version of Bullet and machine setup?
//    ASSERT_EQUAL(entities[0]->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(0.495543, -0, 19.949));
//    ASSERT_EQUAL(entities[10]->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(1, -0, 18.0217));
//    ASSERT_EQUAL(entities[15]->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(2.61429, 0.0884429, 26.4489));
//    ASSERT_EQUAL(entities[16]->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(0.948305, 0.105805, 18.7352));
//    ASSERT_EQUAL(entities[55]->requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(6.30361, -1.19749f, 28.0569));

    }


    void test_zoffset(TestContext& context)
    {


        TypeNode rockType("rock");
        ModeProperty modePlantedProperty{};
        modePlantedProperty.set("planted");

        Property<double> plantedOffsetBase{};
        plantedOffsetBase.data() = -2;

        Entity rootEntity(context.newId());
        rootEntity.incRef();
        TerrainProperty terrainProperty{};
        rootEntity.setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty.copy()));
        Mercator::Terrain& terrain = terrainProperty.getData(rootEntity);
        terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
        terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(0, -64, 0), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        TestWorld testWorld(&rootEntity);


        Entity plantedEntity(RouterId("planted", context.newId()));
        plantedEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty.copy()));
        plantedEntity.setType(&rockType);
        plantedEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(30, 10, 30);
        plantedEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 10, 1));
        auto plantedOffset = (Property<double>*) plantedEntity.setProperty("planted_offset", std::unique_ptr<PropertyBase>(plantedOffsetBase.copy()));
        domain->addEntity(plantedEntity);
        ASSERT_FUZZY_EQUAL(plantedEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(30, 8.01695, 30), epsilon);

        plantedOffset->data() = -3;
        plantedOffset->apply(plantedEntity);
        plantedEntity.propertyApplied.emit("planted_offset", *plantedOffset);
        ASSERT_FUZZY_EQUAL(plantedEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(30, 7.01695, 30), epsilon);

    }


    void test_zscaledoffset(TestContext& context)
    {
        TypeNode rockType("rock");
        ModeProperty modePlantedProperty{};
        modePlantedProperty.set("planted");

        Property<double> plantedScaledOffsetBase{};
        plantedScaledOffsetBase.data() = -0.2;

        Entity rootEntity(context.newId());
        rootEntity.incRef();
        TerrainProperty terrainProperty{};
        rootEntity.setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty.copy()));
        Mercator::Terrain& terrain = terrainProperty.getData(rootEntity);
        terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
        terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(0, -64, 0), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        TestWorld testWorld(&rootEntity);


        Entity plantedEntity(RouterId("planted", context.newId()));
        plantedEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty.copy()));
        plantedEntity.setType(&rockType);
        plantedEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(30, 10, 30);
        plantedEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 10, 10));
        auto plantedScaledOffset = (Property<double>*) plantedEntity.setProperty("planted_scaled_offset", std::unique_ptr<PropertyBase>(plantedScaledOffsetBase.copy()));
        domain->addEntity(plantedEntity);
        ASSERT_FUZZY_EQUAL(plantedEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(30, 8.01695, 30), epsilon);

        plantedScaledOffset->data() = -0.3;
        plantedScaledOffset->apply(plantedEntity);
        plantedEntity.propertyApplied.emit("planted_offset", *plantedScaledOffset);
        ASSERT_FUZZY_EQUAL(plantedEntity.requirePropertyClassFixed<PositionProperty>().data(), WFMath::Point<3>(30, 7.01695, 30), epsilon);

    }


    void test_visibility(TestContext& context)
    {
        TypeNode rockType("rock");
        TypeNode humanType("human");
        ModeProperty modePlantedProperty{};
        modePlantedProperty.set("planted");

        VisibilityDistanceProperty visibilityProperty{};
        visibilityProperty.set(1000.f);


        Entity rootEntity(context.newId());
        rootEntity.incRef();
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, 0, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        TestWorld testWorld(&rootEntity);

        Entity smallEntity1(RouterId("small1", context.newId()));
        smallEntity1.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty.copy()));
        smallEntity1.setType(&rockType);
        smallEntity1.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(30, 0, 30);
        smallEntity1.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, 0, -0.2f), WFMath::Point<3>(0.2, 0.4, 0.2));
        domain->addEntity(smallEntity1);

        Entity smallEntity2(RouterId("small2", context.newId()));
        smallEntity2.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty.copy()));
        smallEntity2.setType(&rockType);
        smallEntity2.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(-31, 0, -31);
        smallEntity2.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, 0, -0.2f), WFMath::Point<3>(0.2, 0.4, 0.2));
        domain->addEntity(smallEntity2);

        //This entity should always be seen, as "visibility" is specified.
        Entity smallVisibleEntity(RouterId("smallVisible", context.newId()));
        smallVisibleEntity.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty.copy()));
        smallVisibleEntity.setType(&rockType);
        smallVisibleEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(-63, 0, -63);
        smallVisibleEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, 0, -0.2f), WFMath::Point<3>(0.2, 0.4, 0.2));
        smallVisibleEntity.setProperty(VisibilityDistanceProperty::property_name, std::unique_ptr<PropertyBase>(visibilityProperty.copy()));
        domain->addEntity(smallVisibleEntity);

        Entity largeEntity1(RouterId("large1", context.newId()));
        largeEntity1.setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty.copy()));
        largeEntity1.setType(&rockType);
        largeEntity1.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(0, 0, 0);
        largeEntity1.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-10.f, 0, -10.f), WFMath::Point<3>(10, 20, 10));
        domain->addEntity(largeEntity1);

        Entity observerEntity(RouterId("observer", context.newId()));
        observerEntity.setType(&humanType);
        observerEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(-30, 0, -30);
        observerEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, 0, -0.2f), WFMath::Point<3>(0.2, 2, 0.2));
        observerEntity.addFlags(entity_perceptive);
        domain->addEntity(observerEntity);

        OpVector res;
        std::set<LocatedEntity*> transformedEntities;
        domain->tick(0.1, res);

        ASSERT_TRUE(domain->isEntityVisibleFor(observerEntity, observerEntity));

        {
            ASSERT_TRUE(domain->isEntityVisibleFor(observerEntity, smallVisibleEntity));
            ASSERT_TRUE(domain->isEntityVisibleFor(observerEntity, smallEntity2));
            ASSERT_TRUE(domain->isEntityVisibleFor(observerEntity, largeEntity1));
            ASSERT_FALSE(domain->isEntityVisibleFor(observerEntity, smallEntity1));

            std::list<LocatedEntity*> observedList;
            domain->getVisibleEntitiesFor(observerEntity, observedList);

            ASSERT_EQUAL(5u, observedList.size());
            ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "small2"; }) != observedList.end());
            ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "smallVisible"; }) != observedList.end());
            ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "large1"; }) != observedList.end());
            ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "observer"; }) != observedList.end());

        }
        //Now move the observer to "small1"
        domain->applyTransform(observerEntity, Domain::TransformData{WFMath::Quaternion(), WFMath::Point<3>(30, 0, 30), nullptr, {}}, transformedEntities);
        //Force visibility updates
        domain->tick(2, res);
        {
            ASSERT_TRUE(domain->isEntityVisibleFor(observerEntity, smallVisibleEntity));
            ASSERT_TRUE(domain->isEntityVisibleFor(observerEntity, smallEntity1));
            ASSERT_TRUE(domain->isEntityVisibleFor(observerEntity, largeEntity1));
            ASSERT_FALSE(domain->isEntityVisibleFor(observerEntity, smallEntity2));

            std::list<LocatedEntity*> observedList;

            domain->getVisibleEntitiesFor(observerEntity, observedList);

            ASSERT_EQUAL(5u, observedList.size());
            ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "small1"; }) != observedList.end());
            ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "smallVisible"; }) != observedList.end());
            ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "large1"; }) != observedList.end());
            ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "observer"; }) != observedList.end());

        }
    }


    void test_visibilityPerformance(TestContext& context);

    void test_stairs(TestContext& context)
    {
        TypeNode rockType("rock");
        TypeNode humanType("human");
        ModeProperty modePlantedProperty{};
        modePlantedProperty.set("planted");
        Property<double> massProp{};
        massProp.data() = 100;
        Property<double> speedGroundProperty{};
        speedGroundProperty.data() = 5.0;
        PropelProperty propelProperty{};
        propelProperty.data() = WFMath::Vector<3>(0, 0, 1.0 / speedGroundProperty.data());
        AngularFactorProperty angularZeroFactorProperty;
        angularZeroFactorProperty.data() = WFMath::Vector<3>::ZERO();
        GeometryProperty capsuleProperty;
        capsuleProperty.set(Atlas::Message::MapType({{"type", "capsule-y"}}));
//    Property<double>* stepFactorProp = new Property<double>();
//    stepFactorProp->data() = 0.3;

        humanType.injectProperty("speed_ground", std::unique_ptr<PropertyBase>(speedGroundProperty.copy()));


        Entity rootEntity(context.newId());
        rootEntity.incRef();
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, 0, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));

        TestWorld testWorld(&rootEntity);


        std::vector<Ref<Entity>> stepElements;
        //Create 10 entities at increasing height, forming a stair.
        for (int i = 0; i < 10; ++i) {
            Ref<Entity> stepElement(new Entity(context.newId()));
            stepElement->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty.copy()));
            float height = 0.1f + (i * 0.1f);
            float zPos = i * 0.2f;
            WFMath::Point<3> pos(0, 0, zPos);
            WFMath::AxisBox<3> bbox(WFMath::Point<3>(-0.4f, 0, -0.1f), WFMath::Point<3>(0.4f, height, 0.1f));
            stepElement->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            stepElement->requirePropertyClassFixed<PositionProperty>().data() = pos;
            stepElement->setType(&rockType);


            domain->addEntity(*stepElement);
            stepElements.emplace_back(stepElement);
        }

        //First with an entity which doesn't step; it should collide and be kept in place
        {
            Ref<Entity> human = new Entity(RouterId{"human", context.newId()});
            human->setProperty(AngularFactorProperty::property_name, std::unique_ptr<PropertyBase>(angularZeroFactorProperty.copy()));
            human->setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
            human->setProperty(PropelProperty::property_name, std::unique_ptr<PropertyBase>(propelProperty.copy()));
            human->setType(&humanType);
            human->requirePropertyClassFixed<PositionProperty>().data() = {0, 0, -1};
            human->requirePropertyClassFixed<BBoxProperty>().data() = {{-0.4f, 0,   -0.4f},
                                                                       {0.4,   1.8, 0.4}};
            domain->addEntity(*human);

            OpVector res;
            domain->tick(2, res);

            ASSERT_FUZZY_EQUAL(-0.5f, human->requirePropertyClassFixed<PositionProperty>().data().z(), 0.1f);
            domain->removeEntity(*human);
        }

        //Then with an entity with a capsule geometry, it should step
        {
            Ref<Entity> human = new Entity(RouterId{"human", context.newId()});
            //human->setProperty("step_factor", stepFactorProp));
            human->setProperty(AngularFactorProperty::property_name, std::unique_ptr<PropertyBase>(angularZeroFactorProperty.copy()));
            human->setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
            human->setProperty(PropelProperty::property_name, std::unique_ptr<PropertyBase>(propelProperty.copy()));
            human->setProperty(GeometryProperty::property_name, std::unique_ptr<PropertyBase>(capsuleProperty.copy()));
            human->setType(&humanType);
            human->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(0, 0, -1);
            human->requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-0.4f, 0, -0.4f), WFMath::Point<3>(0.4, 1.8, 0.4));
            domain->addEntity(*human);

            OpVector res;
            domain->tick(2, res);

            ASSERT_FUZZY_EQUAL(0.5, human->requirePropertyClassFixed<PositionProperty>().data().z(), 0.3f);
            domain->removeEntity(*human);
        }

        //Also place a tilted square entity, which is too tilted to allow for stepping on
        //The human entity shouldn't step up on the tilted entity
        {

            Ref<Entity> stepElement = new Entity(RouterId{"tilted", context.newId()});
            stepElement->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty.copy()));
            WFMath::Point<3> pos(20, 0, 0);
            WFMath::AxisBox<3> bbox(WFMath::Point<3>(-0.4f, 0.f, 0), WFMath::Point<3>(0.4f, 1, 0.4f));
            stepElement->requirePropertyClassFixed<OrientationProperty>().data().rotate(WFMath::Quaternion(0, WFMath::numeric_constants<float>::pi() * 0.2f));
            stepElement->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            stepElement->requirePropertyClassFixed<PositionProperty>().data() = pos;
            stepElement->setType(&rockType);


            domain->addEntity(*stepElement);

            Ref<Entity> human = new Entity(RouterId{"human", context.newId()});
            //human->setProperty("step_factor", stepFactorProp));
            human->setProperty(AngularFactorProperty::property_name, std::unique_ptr<PropertyBase>(angularZeroFactorProperty.copy()));
            human->setProperty("mass", std::unique_ptr<PropertyBase>(massProp.copy()));
            human->setProperty(PropelProperty::property_name, std::unique_ptr<PropertyBase>(propelProperty.copy()));
            human->setProperty(GeometryProperty::property_name, std::unique_ptr<PropertyBase>(capsuleProperty.copy()));
            human->setType(&humanType);
            human->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(20, 0, -1);
            human->requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-0.4f, 0, -0.4f), WFMath::Point<3>(0.4, 1.8, 0.4));
            domain->addEntity(*human);

            OpVector res;
            domain->tick(2, res);

            ASSERT_FUZZY_EQUAL(0, human->requirePropertyClassFixed<PositionProperty>().data().y(), 0.01f);
            ASSERT_FUZZY_EQUAL(-0.4f, human->requirePropertyClassFixed<PositionProperty>().data().z(), 0.1f);
        }


    }

    void test_terrainPrecision(TestContext& context)
    {

        Entity rootEntity(context.newId());
        rootEntity.incRef();
        TerrainProperty terrainProperty{};
        rootEntity.setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty.copy()));
        Mercator::Terrain& terrain = terrainProperty.getData(rootEntity);
        terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
        terrain.setBasePoint(0, 1, Mercator::BasePoint(15));
        terrain.setBasePoint(0, -1, Mercator::BasePoint(15));
        terrain.setBasePoint(1, 0, Mercator::BasePoint(20));
        terrain.setBasePoint(1, 1, Mercator::BasePoint(25));
        terrain.setBasePoint(1, -1, Mercator::BasePoint(30));
        terrain.setBasePoint(-1, 0, Mercator::BasePoint(35));
        terrain.setBasePoint(-1, 1, Mercator::BasePoint(40));
        terrain.setBasePoint(-1, -1, Mercator::BasePoint(45));
        rootEntity.requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
        rootEntity.requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64));
        std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(rootEntity));


        auto checkHeightFunc = [&](float x, float z) {
            PhysicalWorld* physicalWorld = domain->test_getPhysicalWorld();

            float mercatorHeight;
            WFMath::Vector<3> normal;
            terrain.getHeightAndNormal(x, z, mercatorHeight, normal);

            btVector3 from(x, 63, z);
            btVector3 to(x, -63, z);
            btCollisionWorld::ClosestRayResultCallback callback(from, to);

            physicalWorld->rayTest(from, to, callback);
            ASSERT_FUZZY_EQUAL(mercatorHeight, callback.m_hitPointWorld.y(), 0.1);
/*
        ASSERT_FUZZY_EQUAL(normal.x(), callback.m_hitNormalWorld.x(), 0.1);
        ASSERT_FUZZY_EQUAL(normal.y(), callback.m_hitNormalWorld.y(), 0.1);
        ASSERT_FUZZY_EQUAL(normal.z(), callback.m_hitNormalWorld.z(), 0.1);
        */
        };

        checkHeightFunc(1, 1);
        checkHeightFunc(10, 10);
        checkHeightFunc(15, 15);
        checkHeightFunc(-15, 15);
        checkHeightFunc(-15, -15);
        checkHeightFunc(15, -15);
    }
};


int main()
{
    Tested t;

    return t.run();
}

