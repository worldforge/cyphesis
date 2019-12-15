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

#include "rules/simulation/Entity.h"

#include "common/debug.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <wfmath/atlasconv.h>

#include <rules/simulation/PhysicalDomain.h>
#include "physics/Convert.h"
#include <rules/simulation/TerrainProperty.h>
#include <Mercator/BasePoint.h>
#include <Mercator/Terrain.h>
#include <rules/simulation/PropelProperty.h>
#include <rules/simulation/AngularFactorProperty.h>
#include <rules/simulation/VisibilityProperty.h>
#include <rules/simulation/GeometryProperty.h>
#include "rules/simulation/PhysicalWorld.h"
#include "rules/BBoxProperty.h"
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

#include <rules/simulation/TerrainModProperty.h>
#include <rules/simulation/EntityProperty.h>
#include <rules/simulation/ModeDataProperty.h>
#include "stubs/common/stubMonitors.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

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
            childEntityPropertyApplied(name, prop, m_entries.find(id)->second.get());
        }
};


class PhysicalDomainIntegrationTest : public Cyphesis::TestBase
{
    protected:
        static long m_id_counter;

    public:
        PhysicalDomainIntegrationTest();

        static long newId();

        void setup() override;

        void teardown() override;

        void test_scaleBbox();

        void test_convert();

        void test_movePlantedAndResting();

        void test_plantedOn();

        void test_terrainMods();

        void test_lake_rotated();

        void test_lake();

        void test_ocean();

        void test_placement();

        void test_fallToBottom();

        void test_standOnFixed();

        void test_fallToTerrain();

        void test_collision();

        void test_mode();

        void test_static_entities_no_move();

        void test_determinism();

        void test_zoffset();

        void test_zscaledoffset();

        void test_visibility();

        void test_visibilityPerformance();

        void test_stairs();

        void test_terrainPrecision();
};

long PhysicalDomainIntegrationTest::m_id_counter = 0L;

PhysicalDomainIntegrationTest::PhysicalDomainIntegrationTest()
{
    ADD_TEST(PhysicalDomainIntegrationTest::test_scaleBbox);
    ADD_TEST(PhysicalDomainIntegrationTest::test_movePlantedAndResting);
    ADD_TEST(PhysicalDomainIntegrationTest::test_plantedOn);
    ADD_TEST(PhysicalDomainIntegrationTest::test_terrainMods);
    ADD_TEST(PhysicalDomainIntegrationTest::test_lake_rotated);
    ADD_TEST(PhysicalDomainIntegrationTest::test_lake);
    ADD_TEST(PhysicalDomainIntegrationTest::test_ocean);
    ADD_TEST(PhysicalDomainIntegrationTest::test_placement);
    ADD_TEST(PhysicalDomainIntegrationTest::test_convert);
    ADD_TEST(PhysicalDomainIntegrationTest::test_terrainPrecision);
    ADD_TEST(PhysicalDomainIntegrationTest::test_fallToBottom);
    ADD_TEST(PhysicalDomainIntegrationTest::test_standOnFixed);
    ADD_TEST(PhysicalDomainIntegrationTest::test_fallToTerrain);
    ADD_TEST(PhysicalDomainIntegrationTest::test_collision);
    ADD_TEST(PhysicalDomainIntegrationTest::test_mode);
    ADD_TEST(PhysicalDomainIntegrationTest::test_determinism);
    ADD_TEST(PhysicalDomainIntegrationTest::test_zoffset);
    ADD_TEST(PhysicalDomainIntegrationTest::test_zscaledoffset);
    ADD_TEST(PhysicalDomainIntegrationTest::test_visibility);
    ADD_TEST(PhysicalDomainIntegrationTest::test_stairs);
}

long PhysicalDomainIntegrationTest::newId()
{
    return ++m_id_counter;
}

void PhysicalDomainIntegrationTest::setup()
{
    m_id_counter = 0;
}

void PhysicalDomainIntegrationTest::teardown()
{
}

#define ASSERT_FUZZY_EQUAL_FN(_lval, _rval, _epsilon, _fn) {\
    if (this->assertFuzzyEqual(#_lval, _lval, #_rval, _rval, #_epsilon, _epsilon, __PRETTY_FUNCTION__,\
                          __FILE__, __LINE__) != 0) {_fn(); return;}\
}


void PhysicalDomainIntegrationTest::test_scaleBbox()
{
    double tickSize = 1.0 / 15.0;
    double time = 0;

    OpVector res;

    TypeNode* rockType = new TypeNode("rock");

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    ModeProperty* plantedProperty = new ModeProperty();
    plantedProperty->set("planted");

    ModeProperty* freeProperty = new ModeProperty();
    freeProperty->set("free");

    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, 0, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));


    long id = newId();
    Entity* plantedEntity = new Entity(std::to_string(id), id);
    plantedEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(plantedProperty));
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(0, 0, 0);
    plantedEntity->m_location.m_orientation = WFMath::Quaternion::IDENTITY();

    BBoxProperty* bBoxProperty = new BBoxProperty();
    bBoxProperty->data() = {{-1, 0, -1},
                            {1,  1, 1}};
    bBoxProperty->install(plantedEntity, "bbox");
    bBoxProperty->apply(plantedEntity);
    plantedEntity->setProperty("bbox", std::unique_ptr<PropertyBase>(bBoxProperty));


    domain->addEntity(*plantedEntity);

    btVector3 from(0, 10, 0);
    btVector3 to(0, -10, 0);

    btCollisionWorld::ClosestRayResultCallback callback(from, to);
    domain->test_getPhysicalWorld()->rayTest(from, to, callback);
    domain->tick(1.0f, res);

    ASSERT_TRUE(callback.hasHit());
    ASSERT_FUZZY_EQUAL(1.0f, callback.m_hitPointWorld.y(), 0.1f);

    //Add a box and let it fall on the planted entity
    id = newId();
    Entity* freeEntity = new Entity(std::to_string(id), id);
    freeEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(freeProperty));
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(0, 10, 0);
    freeEntity->m_location.m_orientation = WFMath::Quaternion::IDENTITY();
    freeEntity->m_location.setBBox({{-0.5f, 0, -0.5f},
                                    {0.5f,  1, 0.5f}});
    freeEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));

    domain->addEntity(*freeEntity);

    while (time < 5) {
        time += tickSize;
        domain->tick(tickSize, res);
    }

    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.y(), 1.0f, 0.1f);

    //Make the bbox larger and test that it adjust itself against the terrain.
    bBoxProperty->data() = {{-1, 0, -1},
                            {1,  2, 1}};
    bBoxProperty->apply(plantedEntity);

    domain->test_childEntityPropertyApplied("bbox", *bBoxProperty, plantedEntity->getIntId());

    domain->tick(1.0f, res);

    callback = btCollisionWorld::ClosestRayResultCallback(from, to);
    domain->test_getPhysicalWorld()->rayTest(from, to, callback);

    ASSERT_TRUE(callback.hasHit());
    ASSERT_FUZZY_EQUAL(2.0f, callback.m_hitPointWorld.y(), 0.1f);

    domain->removeEntity(*freeEntity);

    //Test again with the falling box
    freeEntity->m_location.m_pos = WFMath::Point<3>(0, 10, 0);

    domain->addEntity(*freeEntity);

    time = 0;
    while (time < 5) {
        time += tickSize;
        domain->tick(tickSize, res);
    }

    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.y(), 2.0f, 0.1f);

}

void PhysicalDomainIntegrationTest::test_movePlantedAndResting()
{

    //Place a box, "planted". On top of that, place another box, also "planted". And on top of that, place a box which is "free".
    //Then move the first box. The two boxes on top should move along with it.

    auto id = newId();
    Entity* rootEntity = new Entity(std::to_string(id), id);
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox({{-64, 0,  -64},
                                    {64,  64, 64}});
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    ModeProperty* fixedProperty = new ModeProperty();
    fixedProperty->set("fixed");
    ModeProperty* plantedProperty = new ModeProperty();
    plantedProperty->set("planted");
    ModeProperty* freeProperty = new ModeProperty();
    freeProperty->set("free");

    auto massProp = new Property<double>();
    massProp->data() = 10000;

    id = newId();
    Entity* fixed1 = new Entity(std::to_string(id), id);
    fixed1->m_location.m_pos = {0, 0, 0};
    fixed1->m_location.setBBox({{-2, -1, -2},
                                {2,  1,  2}});
    fixed1->m_location.m_orientation = WFMath::Quaternion::IDENTITY();
    fixed1->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(fixedProperty));

    domain->addEntity(*fixed1);

    OpVector res;
    domain->tick(0, res);

    ASSERT_EQUAL(0, fixed1->m_location.m_pos.y());


    id = newId();
    Entity* planted1 = new Entity(std::to_string(id), id);
    planted1->m_location.m_pos = {1, 1, 0};
    planted1->m_location.m_orientation = WFMath::Quaternion::IDENTITY();
    planted1->m_location.setBBox({{-2, -1, -2},
                                  {2,  1,  2}});
    planted1->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(plantedProperty));

    domain->addEntity(*planted1);

    domain->tick(0, res);

    id = newId();
    Entity* planted2 = new Entity(std::to_string(id), id);
    planted2->m_location.m_pos = {0, 2, 1};
    planted2->m_location.m_orientation = WFMath::Quaternion::IDENTITY();
    planted2->m_location.setBBox({{-2, -1, -2},
                                  {2,  1,  2}});
    planted2->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(plantedProperty));

    domain->addEntity(*planted2);

    domain->tick(0, res);

    ASSERT_FUZZY_EQUAL(2.0f, planted2->m_location.m_pos.y(), 0.1f);

    id = newId();
    Entity* freeEntity = new Entity(std::to_string(id), id);
    freeEntity->m_location.m_pos = {1, 4, 0};
    freeEntity->m_location.m_orientation = WFMath::Quaternion::IDENTITY();
    freeEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(freeProperty));
    freeEntity->m_location.setBBox({{-1, -1, -1},
                                    {1,  1,  1}});
    freeEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    domain->addEntity(*freeEntity);

    ASSERT_FUZZY_EQUAL(4.0f, freeEntity->m_location.m_pos.y(), 0.1);
    domain->tick(1, res);

    ASSERT_FUZZY_EQUAL(4.0f, freeEntity->m_location.m_pos.y(), 0.1);

    //Only change position
    {
        std::set<LocatedEntity*> transformedEntities;

        domain->applyTransform(*fixed1, Domain::TransformData{WFMath::Quaternion(), {10, 10, 10}, {}, nullptr, {}}, transformedEntities);
        ASSERT_EQUAL(4u, transformedEntities.size());
        ASSERT_EQUAL(WFMath::Point<3>(11, 11, 10), planted1->m_location.pos());
        ASSERT_EQUAL(WFMath::Point<3>(10, 12, 11), planted2->m_location.pos());
        ASSERT_TRUE(WFMath::Equal(WFMath::Point<3>(11, 14, 10), freeEntity->m_location.pos(), 0.1));
    }
    domain->tick(0, res);


    //Only change orientation
    {
        std::set<LocatedEntity*> transformedEntities;

        domain->applyTransform(*fixed1, Domain::TransformData{WFMath::Quaternion(1, WFMath::numeric_constants<float>::pi() / 2), {}, {}, nullptr, {}}, transformedEntities);
        ASSERT_EQUAL(4u, transformedEntities.size());
        ASSERT_EQUAL(WFMath::Point<3>(10, 11, 9), planted1->m_location.pos());
        ASSERT_EQUAL(WFMath::Point<3>(11, 12, 10), planted2->m_location.pos());
        ASSERT_TRUE(WFMath::Equal(WFMath::Point<3>(10, 14, 9), freeEntity->m_location.pos(), 0.1));
    }
    domain->tick(0, res);

    //Move it, and at the same time rotate it 90 degrees around the y axis.
    {
        std::set<LocatedEntity*> transformedEntities;


        domain->applyTransform(*fixed1, Domain::TransformData{WFMath::Quaternion(1, WFMath::numeric_constants<float>::pi()), {15, 15, 15}, {}, nullptr, {}}, transformedEntities);
        ASSERT_EQUAL(4u, transformedEntities.size());
        ASSERT_EQUAL(WFMath::Point<3>(14, 16, 15), planted1->m_location.pos());
        ASSERT_EQUAL(WFMath::Point<3>(15, 17, 14), planted2->m_location.pos());
        ASSERT_TRUE(WFMath::Equal(WFMath::Point<3>(14, 19, 15), freeEntity->m_location.pos(), 0.1));
    }
    domain->tick(0, res);

    //Move away the first planted entity, which should also move along the second planted and the free entity, and not affect the fixed entity.
    {
        std::set<LocatedEntity*> transformedEntities;

        domain->applyTransform(*planted1, Domain::TransformData{{},
                                                                {20, 0, 20},
                                                                {}, nullptr, {}}, transformedEntities);
        ASSERT_EQUAL(3u, transformedEntities.size());
        ASSERT_EQUAL(WFMath::Point<3>(21, 1, 19), planted2->m_location.pos());
        ASSERT_TRUE(WFMath::Equal(WFMath::Point<3>(20, 3, 20), freeEntity->m_location.pos(), 0.1));
        ASSERT_EQUAL(WFMath::Point<3>(15, 15, 15), fixed1->m_location.pos());
    }

    {
        std::set<LocatedEntity*> transformedEntities;
        domain->applyTransform(*fixed1, Domain::TransformData{WFMath::Quaternion(), {5, 20, 5}, {}, nullptr, {}}, transformedEntities);
        ASSERT_EQUAL(1u, transformedEntities.size());
    }

    //Remove the second planted entity, making sure that the first planted doesn't keep a reference
    domain->removeEntity(*planted2);
    {
        std::set<LocatedEntity*> transformedEntities;

        domain->applyTransform(*planted1, Domain::TransformData{WFMath::Quaternion(), {15, 0, 20}, {}, nullptr, {}}, transformedEntities);
        ASSERT_EQUAL(1u, transformedEntities.size());
    }
}

void PhysicalDomainIntegrationTest::test_plantedOn()
{
    std::vector<std::string> shapes{"box", "cylinder-x", "cylinder-y", "cylinder-z", "capsule-x", "capsule-y", "capsule-z"};

    for (auto plantedShape : shapes) {
        for (auto plantedOnTopShape : shapes) {

            auto id = newId();
            Entity rootEntity{std::to_string(id), id};
            TerrainProperty* terrainProperty = new TerrainProperty();
            Mercator::Terrain& terrain = terrainProperty->getData();
            terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
            terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
            terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
            terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
            rootEntity.setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
            rootEntity.m_location.m_pos = WFMath::Point<3>::ZERO();
            rootEntity.m_location.setBBox({{-64, -64, -64},
                                           {64,  64,  64}});
            TestPhysicalDomain domain{rootEntity};


            id = newId();
            std::unique_ptr<Entity> planted1(new Entity(std::to_string(id), id));
            planted1->m_location.m_pos = WFMath::Point<3>(0, 10, 0);
            planted1->m_location.setBBox({{-1, -1, -1},
                                          {1,  1,  1}});
            {
                GeometryProperty* plantedGeometryProperty = new GeometryProperty();
                plantedGeometryProperty->set(MapType{{"type", plantedShape}});
                ModeProperty* modeProperty = new ModeProperty();
                modeProperty->set("planted");

                planted1->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty));
                planted1->setProperty(GeometryProperty::property_name, std::unique_ptr<PropertyBase>(plantedGeometryProperty));
            }

            domain.addEntity(*planted1);

            OpVector res;
            domain.tick(0, res);

            ASSERT_TRUE(planted1->getPropertyClassFixed<ModeDataProperty>())
            ASSERT_TRUE(planted1->getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entity)
            ASSERT_EQUAL(rootEntity.getIntId(), planted1->getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entity->getIntId())
            ASSERT_FUZZY_EQUAL(10, planted1->m_location.m_pos.y(), 0.1);
            {
                auto* planted1RigidBody = domain.test_getRigidBody(planted1->getIntId());
                btVector3 aabbMin, aabbMax;
                planted1RigidBody->getAabb(aabbMin, aabbMax);
                ASSERT_FUZZY_EQUAL_FN(aabbMin.y(), 9, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1'.", plantedShape)); });
                ASSERT_FUZZY_EQUAL_FN(aabbMax.y(), 11, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1'.", plantedShape)); });

            }

            id = newId();
            std::unique_ptr<Entity> planted2(new Entity(std::to_string(id), id));
            planted2->m_location.m_pos = WFMath::Point<3>(0, 15, 0);
            planted2->m_location.setBBox({{-1, -1, -1},
                                          {1,  1,  1}});
            {
                GeometryProperty* plantedGeometryProperty = new GeometryProperty();
                plantedGeometryProperty->set(MapType{{"type", plantedShape}});
                ModeProperty* modeProperty = new ModeProperty();
                modeProperty->set("planted");

                planted2->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty));
                planted2->setProperty(GeometryProperty::property_name, std::unique_ptr<PropertyBase>(plantedGeometryProperty));
            }

            domain.addEntity(*planted2);

            domain.tick(0, res);

            ASSERT_TRUE(planted2->getPropertyClassFixed<ModeDataProperty>());
            ASSERT_TRUE(planted2->getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entity);
            ASSERT_EQUAL(planted1->getIntId(), planted2->getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entity->getIntId());
            ASSERT_FUZZY_EQUAL(11, planted2->m_location.m_pos.y(), 0.1);
            {
                auto* planted2RigidBody = domain.test_getRigidBody(planted2->getIntId());
                btVector3 aabbMin, aabbMax;
                planted2RigidBody->getAabb(aabbMin, aabbMax);
                ASSERT_FUZZY_EQUAL_FN(aabbMin.y(), 10, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1'.", plantedShape)); });
                ASSERT_FUZZY_EQUAL_FN(aabbMax.y(), 12, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1'.", plantedShape)); });

            }


            id = newId();
            std::unique_ptr<Entity> plantedOn(new Entity(std::to_string(id), id));
            plantedOn->m_location.m_pos = {0, 15, 0};
            {
                ModeProperty* modeProperty = new ModeProperty();
                modeProperty->set("planted");
                plantedOn->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty));
            }
            plantedOn->m_location.setBBox({{-1, 0, -1},
                                           {1,  1, 1}});
            auto modeDataProperty = new ModeDataProperty();
            modeDataProperty->setPlantedData({WeakEntityRef(planted1.get())});
            plantedOn->setProperty(ModeDataProperty::property_name, std::unique_ptr<PropertyBase>(modeDataProperty));

            GeometryProperty* geometryProperty = new GeometryProperty();
            geometryProperty->set(MapType{{"type", plantedOnTopShape}});
            plantedOn->setProperty(GeometryProperty::property_name, std::unique_ptr<PropertyBase>(geometryProperty));

            domain.addEntity(*plantedOn);


            ASSERT_TRUE(plantedOn->getPropertyClassFixed<ModeDataProperty>());
            ASSERT_TRUE(plantedOn->getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entity);
            ASSERT_EQUAL(planted1->getIntId(), plantedOn->getPropertyClassFixed<ModeDataProperty>()->getPlantedOnData().entity->getIntId());
            ASSERT_FUZZY_EQUAL_FN(plantedOn->m_location.m_pos.y(), 11, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1' on top of '%2'.", plantedOnTopShape, plantedShape)); });
            {
                auto* plantedOnRigidBody = domain.test_getRigidBody(plantedOn->getIntId());
                btVector3 aabbMin, aabbMax;
                plantedOnRigidBody->getAabb(aabbMin, aabbMax);
                ASSERT_FUZZY_EQUAL_FN(aabbMin.y(), 11, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1' on top of '%2'.", plantedOnTopShape, plantedShape)); });
                ASSERT_FUZZY_EQUAL_FN(aabbMax.y(), 12, 0.1, [&]() { this->addFailure(String::compose("Using shape '%1' on top of '%2'.", plantedOnTopShape, plantedShape)); });
            }

            domain.removeEntity(*planted2);
            domain.removeEntity(*planted1);
            domain.removeEntity(*plantedOn);

        }
    }
}


void PhysicalDomainIntegrationTest::test_terrainMods()
{

    Entity* rootEntity = new Entity("0", newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    Mercator::Terrain& terrain = terrainProperty->getData();
    terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
    rootEntity->setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("planted");

    Entity* terrainModEntity = new Entity("1", newId());
    terrainModEntity->m_location.m_pos = WFMath::Point<3>(32, 10, 32);
    terrainModEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty));
    TerrainModProperty* terrainModProperty = new TerrainModProperty();

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

    terrainModProperty->set(modElement);
    terrainModEntity->setProperty(TerrainModProperty::property_name, std::unique_ptr<PropertyBase>(terrainModProperty));
    terrainModProperty->apply(terrainModEntity);

    domain->addEntity(*terrainModEntity);

    OpVector res;
    std::set<LocatedEntity*> transformedEntities;

    domain->tick(0, res);


    ASSERT_FUZZY_EQUAL(terrain.get(10, 10), 10.0f, 0.1f);
    ASSERT_TRUE(terrain.hasMod(terrainModEntity->getIntId()));
    ASSERT_FUZZY_EQUAL(terrain.get(32, 32), 5.0f, 0.1f);


    {
        btVector3 rayFrom(32, 32, 32);
        btVector3 rayTo(32, -32, 32);
        btCollisionWorld::ClosestRayResultCallback callback(rayFrom, rayTo);
        domain->test_getPhysicalWorld()->rayTest(rayFrom, rayTo, callback);

        ASSERT_FUZZY_EQUAL(callback.m_hitPointWorld.y(), 5.0f, 0.1f);
    }
    domain->applyTransform(*terrainModEntity, Domain::TransformData{WFMath::Quaternion(), WFMath::Point<3>(10, 10, 10), WFMath::Vector<3>(), nullptr, {}}, transformedEntities);

    domain->tick(0, res);

    ASSERT_FUZZY_EQUAL(terrain.get(10, 10), 5.0f, 0.1f);
    ASSERT_TRUE(terrain.hasMod(terrainModEntity->getIntId()));
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
    terrainModEntity->propertyApplied.emit("mode", *modeProperty);

    domain->tick(0, res);

    ASSERT_FUZZY_EQUAL(terrain.get(10, 10), 10.0f, 0.1f);
    ASSERT_FALSE(terrain.hasMod(terrainModEntity->getIntId()));
    ASSERT_FUZZY_EQUAL(terrain.get(32, 32), 10.0f, 0.1f);

    //And back to "planted" which should bring it back
    modeProperty->set("planted");
    modeProperty->apply(terrainModEntity);
    terrainModEntity->propertyApplied.emit("mode", *modeProperty);

    domain->tick(0, res);

    ASSERT_FUZZY_EQUAL(terrain.get(10, 10), 5.0f, 0.1f);
    ASSERT_TRUE(terrain.hasMod(terrainModEntity->getIntId()));
    ASSERT_FUZZY_EQUAL(terrain.get(32, 32), 10.0f, 0.1f);
}


void PhysicalDomainIntegrationTest::test_lake_rotated()
{
    class TestEntity : public Entity
    {
        public:
            explicit TestEntity(const std::string& id, long intId) : Entity(id, intId)
            {
            }

            decltype(LocatedEntity::propertyApplied)& test_propertyApplied()
            {
                return propertyApplied;
            }
    };

    TypeNode* rockType = new TypeNode("rock");
    TypeNode* lakeType = new TypeNode("lake");

    auto massProp = new Property<double>();
    massProp->data() = 10000;

    auto waterBodyProp = new BoolProperty();
    waterBodyProp->set(1);

    auto modeFreeProperty = new ModeProperty();
    modeFreeProperty->set("free");
    rockType->injectProperty("mode", std::unique_ptr<PropertyBase>(modeFreeProperty));

    auto modeFixedProperty = new ModeProperty();
    modeFixedProperty->set("fixed");


    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, 0, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    long id = newId();
    TestEntity* lake = new TestEntity(std::to_string(id), id);
    lake->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeFixedProperty));
    lake->setType(lakeType);
    lake->setProperty("water_body", std::unique_ptr<PropertyBase>(waterBodyProp));
    lake->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(0, -64, 0), WFMath::Point<3>(10, 0, 2)));
    lake->m_location.m_pos = WFMath::Point<3>(0, 10, 0);
    //rotate 90 degrees
    lake->m_location.m_orientation = WFMath::Quaternion(1, -WFMath::numeric_constants<float>::pi() / 2.0f);
    domain->addEntity(*lake);

    //Should be in water
    id = newId();
    Entity* freeEntity = new Entity("freeEntity", id);
    freeEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(-1, 1, 9);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    //Should not be in water
    id = newId();
    Entity* freeEntity2 = new Entity("freeEntity2", id);
    freeEntity2->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity2->setType(rockType);
    freeEntity2->m_location.m_pos = WFMath::Point<3>(9, 1, 1);
    freeEntity2->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity2);

    id = newId();
    ModeProperty* plantedProp = new ModeProperty();
    plantedProp->set("planted");
    auto modeDataProp = new ModeDataProperty();
    modeDataProp->setPlantedData({WeakEntityRef(lake)});
    Entity* floatingEntity = new Entity("floatingEntity", id);
    floatingEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(plantedProp));
    floatingEntity->setProperty(ModeDataProperty::property_name, std::unique_ptr<PropertyBase>(modeDataProp));

    floatingEntity->setType(rockType);
    floatingEntity->m_location.m_pos = WFMath::Point<3>(5, 20, 1);
    floatingEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*floatingEntity);

    OpVector res;
    domain->tick(0, res);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);
    ASSERT_TRUE(freeEntity2->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);
    ASSERT_TRUE(floatingEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Planted);
    ASSERT_EQUAL(WFMath::Point<3>(5, 10, 1), floatingEntity->m_location.pos());

}


void PhysicalDomainIntegrationTest::test_lake()
{
    class TestEntity : public Entity
    {
        public:
            explicit TestEntity(const std::string& id, long intId) : Entity(id, intId)
            {
            }

            decltype(LocatedEntity::propertyApplied)& test_propertyApplied()
            {
                return propertyApplied;
            }
    };

    double tickSize = 1.0 / 15.0;
    double time = 0;

    TypeNode* rockType = new TypeNode("rock");
    TypeNode* lakeType = new TypeNode("lake");

    auto massProp = new Property<double>();
    massProp->data() = 10000;

    auto waterBodyProp = new BoolProperty();
    waterBodyProp->set(1);

    auto modeFreeProperty = new ModeProperty();
    modeFreeProperty->set("free");
    rockType->injectProperty("mode", std::unique_ptr<PropertyBase>(modeFreeProperty));

    auto modeFixedProperty = new ModeProperty();
    modeFixedProperty->set("fixed");


    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    long id = newId();
    TestEntity* lake = new TestEntity(std::to_string(id), id);
    lake->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeFixedProperty));
    lake->setType(lakeType);
    lake->setProperty("water_body", std::unique_ptr<PropertyBase>(waterBodyProp));
    lake->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-5, -64, -5), WFMath::Point<3>(5, 0, 5)));
    lake->m_location.m_pos = WFMath::Point<3>(20, 0, 0);
    lake->m_location.m_orientation = WFMath::Quaternion::IDENTITY();
    domain->addEntity(*lake);

    id = newId();
    Entity* freeEntity = new Entity(std::to_string(id), id);
    freeEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(20, 2, 0);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    //The second entity is placed in water, and should be submerged from the start
    id = newId();
    Entity* freeEntity2 = new Entity(std::to_string(id), id);
    freeEntity2->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity2->setType(rockType);
    freeEntity2->m_location.m_pos = WFMath::Point<3>(20, -2, 2);
    freeEntity2->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity2);

    //The third entity is placed outside of the lake, and should never be submerged.
    id = newId();
    Entity* freeEntity3 = new Entity(std::to_string(id), id);
    freeEntity3->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity3->setType(rockType);
    freeEntity3->m_location.m_pos = WFMath::Point<3>(-20, 2, 0);
    freeEntity3->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity3);

    OpVector res;
    std::set<LocatedEntity*> transformedEntities;
    domain->tick(0, res);
    while (time < 5) {
        time += tickSize;
        domain->tick(tickSize, res);
    }

    ASSERT_TRUE(freeEntity->m_location.pos().y() < 0);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);

    ASSERT_TRUE(freeEntity2->m_location.pos().y() < 0);
    ASSERT_TRUE(freeEntity2->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);

    ASSERT_TRUE(freeEntity3->m_location.pos().y() < 0);
    ASSERT_TRUE(freeEntity3->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

    //Move outside
    domain->applyTransform(*freeEntity, Domain::TransformData{WFMath::Quaternion::IDENTITY(), WFMath::Point<3>(20, 60, 0), WFMath::Vector<3>(), nullptr, {}}, transformedEntities);
    domain->tick(0, res);
    ASSERT_TRUE(freeEntity->m_location.pos().y() > 0);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

    //Move back in.
    domain->applyTransform(*freeEntity, Domain::TransformData{WFMath::Quaternion::IDENTITY(), WFMath::Point<3>(20, -10, 0), WFMath::Vector<3>(), nullptr, {}}, transformedEntities);
    domain->tick(0, res);
    ASSERT_TRUE(freeEntity->m_location.pos().y() < 0);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);
    ASSERT_TRUE(freeEntity3->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

    //Move the lake to where freeEntity3 is
    domain->applyTransform(*lake, Domain::TransformData{WFMath::Quaternion(), freeEntity3->m_location.m_pos + WFMath::Vector<3>(0, 5, 0), WFMath::Vector<3>(), nullptr, {}}, transformedEntities);
    domain->tick(0, res);
    ASSERT_TRUE(freeEntity3->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

    //Update the bbox of the lake so that it's outside of freeEntity3.
    //To emulate the propertyApplied signal being called in this test we need to do it ourselves.
    auto newBbox = lake->m_location.bBox();
    newBbox.highCorner().x() = 1;
    newBbox.highCorner().y() = 1;
    newBbox.highCorner().z() = 1;
    newBbox.lowCorner().x() = -1;
    newBbox.lowCorner().y() = -1;
    newBbox.lowCorner().z() = -1;

    BBoxProperty* bBoxProperty = new BBoxProperty();
    bBoxProperty->set(newBbox.toAtlas());
    lake->setProperty("bbox", std::unique_ptr<PropertyBase>(bBoxProperty));
    bBoxProperty->apply(lake);
    lake->test_propertyApplied().emit("bbox", *bBoxProperty);

    domain->tick(0, res);
    ASSERT_TRUE(freeEntity3->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

    domain->removeEntity(*lake);
    domain->tick(0, res);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);
    ASSERT_TRUE(freeEntity2->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);
    ASSERT_TRUE(freeEntity3->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

}


void PhysicalDomainIntegrationTest::test_ocean()
{
    double tickSize = 1.0 / 15.0;
    double time = 0;

    TypeNode* rockType = new TypeNode("rock");
    TypeNode* oceanType = new TypeNode("ocean");

    auto massProp = new Property<double>();
    massProp->data() = 10000;

    auto waterBodyProp = new BoolProperty();
    waterBodyProp->set(1);

    auto modeFreeProperty = new ModeProperty();
    modeFreeProperty->set("free");
    auto modeFixedProperty = new ModeProperty();
    modeFixedProperty->set("fixed");

    rockType->injectProperty("mode", std::unique_ptr<PropertyBase>(modeFreeProperty));


    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    long id = newId();
    Entity* ocean = new Entity(std::to_string(id), id);
    ocean->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeFixedProperty));
    ocean->setType(oceanType);
    ocean->setProperty("water_body", std::unique_ptr<PropertyBase>(waterBodyProp));
    ocean->m_location.m_pos = WFMath::Point<3>(0, 0, 0);
    ocean->m_location.m_orientation = WFMath::Quaternion::IDENTITY();
    domain->addEntity(*ocean);

    id = newId();
    Entity* freeEntity = new Entity(std::to_string(id), id);
    freeEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(0, 2, 0);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    //The second entity is placed in water, and should be submerged from the start
    id = newId();
    Entity* freeEntity2 = new Entity(std::to_string(id), id);
    freeEntity2->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity2->setType(rockType);
    freeEntity2->m_location.m_pos = WFMath::Point<3>(10, -10, 0);
    freeEntity2->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity2);


    OpVector res;
    std::set<LocatedEntity*> transformedEntities;
    domain->tick(0, res);
    while (time < 5) {
        time += tickSize;
        domain->tick(tickSize, res);
    }

    ASSERT_TRUE(freeEntity->m_location.pos().y() < 0);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);

    ASSERT_TRUE(freeEntity2->m_location.pos().y() < 0);
    ASSERT_TRUE(freeEntity2->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);

    //Move outside
    domain->applyTransform(*freeEntity, Domain::TransformData{WFMath::Quaternion::IDENTITY(), WFMath::Point<3>(0, 60, 0), WFMath::Vector<3>(), nullptr, {}}, transformedEntities);
    domain->tick(0, res);
    ASSERT_TRUE(freeEntity->m_location.pos().y() > 0);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

    //Move back in.
    domain->applyTransform(*freeEntity, Domain::TransformData{WFMath::Quaternion::IDENTITY(), WFMath::Point<3>(0, -10, 0), WFMath::Vector<3>(), nullptr, {}}, transformedEntities);
    domain->tick(0, res);
    ASSERT_TRUE(freeEntity->m_location.pos().y() < 0);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);

    domain->removeEntity(*ocean);
    domain->tick(0, res);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);
    ASSERT_TRUE(freeEntity2->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);
}


void PhysicalDomainIntegrationTest::test_placement()
{

    TypeNode* rockType = new TypeNode("rock");

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("fixed");


    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    // btDiscreteDynamicsWorld* bulletWorld = domain->test_getBulletWorld();

    auto verifyBboxes = [&](Entity* entity) {
        btRigidBody* rigidBody = domain->test_getRigidBody(entity->getIntId());
        btVector3 aabbMin, aabbMax;
        rigidBody->getAabb(aabbMin, aabbMax);

        //Get the final positions of the entity's bbox

        btVector3 expectedBtAabbMax(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
        btVector3 expectedBtAabbMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

        for (size_t i = 0; i < entity->m_location.bBox().numCorners(); ++i) {
            WFMath::Point<3> point = entity->m_location.bBox().getCorner(i);
            point.rotate(entity->m_location.orientation(), WFMath::Point<3>::ZERO());

            point += WFMath::Vector<3>(entity->m_location.pos());

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


    auto performPlacementTests = [&](Entity* entity) {
        verifyBboxes(entity);
        std::set<LocatedEntity*> transformedEntities;

        //Change pos only
        domain->applyTransform(*entity, Domain::TransformData{WFMath::Quaternion(), WFMath::Point<3>(20, 30, 1), WFMath::Vector<3>(), nullptr, {}}, transformedEntities);

        verifyBboxes(entity);

        //Change orientation only
        domain->applyTransform(*entity, Domain::TransformData{WFMath::Quaternion(1, WFMath::numeric_constants<float>::pi() / 3.0f), WFMath::Point<3>(), WFMath::Vector<3>(), nullptr, {}},
                               transformedEntities);

        verifyBboxes(entity);

        //Change pos and orientation
        domain->applyTransform(*entity, Domain::TransformData{WFMath::Quaternion(1, WFMath::numeric_constants<float>::pi() / 5.0f), WFMath::Point<3>(10, -25, 6), WFMath::Vector<3>(), nullptr, {}},
                               transformedEntities);

        verifyBboxes(entity);

        //Change velocity (should not change pos and orientation)
        domain->applyTransform(*entity, Domain::TransformData{WFMath::Quaternion(), WFMath::Point<3>(), WFMath::Vector<3>(4, 4, 4), nullptr, {}}, transformedEntities);

        verifyBboxes(entity);
    };


    //Start with a box centered at origo, with no orientation
    {
        long id = newId();
        Entity* entity = new Entity(std::to_string(id), id);
        entity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty));
        entity->setType(rockType);
        entity->m_location.m_pos = WFMath::Point<3>(10, -20, 1);
        entity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-6, -1, -2), WFMath::Point<3>(6, 1, 2)));
        entity->m_location.m_orientation = WFMath::Quaternion::IDENTITY();
        domain->addEntity(*entity);

        performPlacementTests(entity);
    }

    //Start with a box centered at origo, with 45 degrees orientation
    {
        long id = newId();
        Entity* entity = new Entity(std::to_string(id), id);
        entity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty));
        entity->setType(rockType);
        entity->m_location.m_pos = WFMath::Point<3>(10, -20, 1);
        entity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-6, -1, -2), WFMath::Point<3>(6, 1, 2)));
        WFMath::Quaternion wfQuat;
        wfQuat.rotation(1, -WFMath::numeric_constants<float>::pi() / 4.0f);
        entity->m_location.m_orientation = wfQuat;
        domain->addEntity(*entity);

        performPlacementTests(entity);
    }

    //A box not centered at origo, with no orientation
    {
        long id = newId();
        Entity* entity = new Entity(std::to_string(id), id);
        entity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty));
        entity->setType(rockType);
        entity->m_location.m_pos = WFMath::Point<3>(10, -20, 1);
        entity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(0, 0, 0), WFMath::Point<3>(6, 1, 2)));
        entity->m_location.m_orientation = WFMath::Quaternion::IDENTITY();
        domain->addEntity(*entity);

        performPlacementTests(entity);
    }

    //A box not centered at origo, with 45 degrees orientation
    {
        long id = newId();
        Entity* entity = new Entity(std::to_string(id), id);
        entity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty));
        entity->setType(rockType);
        entity->m_location.m_pos = WFMath::Point<3>(10, -20, 1);
        entity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(2, 0, 1), WFMath::Point<3>(6, 1, 2)));
        WFMath::Quaternion wfQuat;
        wfQuat.rotation(1, -WFMath::numeric_constants<float>::pi() / 4.0f);
        entity->m_location.m_orientation = wfQuat;
        domain->addEntity(*entity);

        performPlacementTests(entity);
    }
}

void PhysicalDomainIntegrationTest::test_convert()
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

    ASSERT_EQUAL(wfQuat.scalar(), btQuat.getW());
    ASSERT_EQUAL(wfQuat.vector().x(), btQuat.getX());
    ASSERT_EQUAL(wfQuat.vector().y(), btQuat.getY());
    ASSERT_EQUAL(wfQuat.vector().z(), btQuat.getZ());


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


void PhysicalDomainIntegrationTest::test_fallToBottom()
{

    double tickSize = 1.0 / 15.0;
    double time = 0;

    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    TypeNode* rockType = new TypeNode("rock");


    Entity* freeEntity = new Entity("1", newId());
    freeEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    Entity* fixedEntity = new Entity("2", newId());
    fixedEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("fixed");
    fixedEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty));
    fixedEntity->setType(rockType);
    fixedEntity->m_location.m_pos = WFMath::Point<3>(10, 0, 10);
    fixedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*fixedEntity);

    OpVector res;

    //First tick should not update anything
    domain->tick(0, res);
    ASSERT_EQUAL(freeEntity->m_location.m_pos, WFMath::Point<3>::ZERO());
    ASSERT_EQUAL(fixedEntity->m_location.m_pos, WFMath::Point<3>(10, 0, 10));

    //Inject enough ticks to move rock to bottom
    while (time < 5) {
        time += tickSize;
        domain->tick(tickSize, res);
    }
    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.y(), -64, 0.1);
    //Fixed entity should not move
    ASSERT_EQUAL(fixedEntity->m_location.m_pos, WFMath::Point<3>(10, 0, 10));
}

void PhysicalDomainIntegrationTest::test_standOnFixed()
{

    double tickSize = 1.0 / 15.0;
    double time = 0;
    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    TypeNode* rockType = new TypeNode("rock");


    Entity* freeEntity = new Entity("1", newId());
    freeEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(0, 1, 0);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    Entity* fixedEntity = new Entity("2", newId());
    fixedEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("fixed");
    fixedEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty));
    fixedEntity->setType(rockType);
    fixedEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    fixedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*fixedEntity);

    OpVector res;

    //Inject enough ticks to move rock to bottom
    while (time < 5) {
        time += tickSize;
        domain->tick(tickSize, res);
    }
    ASSERT_EQUAL(freeEntity->m_location.m_pos, WFMath::Point<3>(0, 1, 0));
}

void PhysicalDomainIntegrationTest::test_fallToTerrain()
{

    double tickSize = 1.0 / 15.0;
    double time = 0;
    Entity* rootEntity = new Entity("0", newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    Mercator::Terrain& terrain = terrainProperty->getData();
    terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
    rootEntity->setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    TypeNode* rockType = new TypeNode("rock");


    Entity* freeEntity = new Entity("1", newId());
    freeEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(10, 20, 10);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    Entity* plantedEntity = new Entity("2", newId());
    plantedEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("planted");

    plantedEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty));
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(20, 20, 20);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*plantedEntity);

    ASSERT_EQUAL(freeEntity->m_location.m_pos.y(), 20);
    //Planted entity should be placed on the terrain when added to the domain.
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(20, 10.0058, 20));

    OpVector res;

    //Inject enough ticks to move rock to bottom
    while (time < 5) {
        time += tickSize;
        domain->tick(tickSize, res);
    }
    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.y(), 10.0087f, 0.01f);
    //Planted entity should not move
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(20, 10.0058, 20));
}

void PhysicalDomainIntegrationTest::test_collision()
{

    double tickSize = 1.0 / 15.0;

    Property<double>* zeroFrictionProperty = new Property<double>();
    zeroFrictionProperty->data() = 0;
    auto speedGroundProperty = new Property<double>();
    speedGroundProperty->data() = 5.0;


    Entity* rootEntity = new Entity("0", newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    Mercator::Terrain& terrain = terrainProperty->getData();
    terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
    rootEntity->setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
    rootEntity->setProperty("friction", std::unique_ptr<PropertyBase>(zeroFrictionProperty));
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    Property<double>* massProp = new Property<double>();
    massProp->data() = 100;

    TypeNode* rockType = new TypeNode("rock");


    PropelProperty* propelProperty = new PropelProperty();
    //Move y axis 2 meter per second.
    propelProperty->data() = WFMath::Vector<3>(0, 0, 2.0 / speedGroundProperty->data());

    auto angularZeroFactorProperty = new AngularFactorProperty();
    angularZeroFactorProperty->data() = WFMath::Vector<3>::ZERO();


    Entity* freeEntity = new Entity("1", newId());
    freeEntity->setProperty(PropelProperty::property_name, std::unique_ptr<PropertyBase>(propelProperty));
    freeEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity->setProperty("friction", std::unique_ptr<PropertyBase>(zeroFrictionProperty));
    freeEntity->setProperty("speed_ground", std::unique_ptr<PropertyBase>(speedGroundProperty));
    freeEntity->setProperty(AngularFactorProperty::property_name, std::unique_ptr<PropertyBase>(angularZeroFactorProperty));
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(10, 10, 10);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));

    domain->addEntity(*freeEntity);

    Entity* plantedEntity = new Entity("2", newId());
    plantedEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("planted");

    plantedEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeProperty));
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(10, 10, 15);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*plantedEntity);
    const WFMath::Point<3> plantedPos = plantedEntity->m_location.m_pos;

    OpVector res;

    domain->tick(tickSize, res);

    //Should have moved 2/15 meters
    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.z(), 10 + (2.0 / 15.0), 0.1f);

    //Inject ticks for one second
    domain->tick(14.0 / 15.0, res);

    //Should have moved 2 meters in y axis
    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.z(), 12, 0.1f);

    //Inject ticks for three seconds
    for (int i = 0; i < (3 * 15); ++i) {
        domain->tick(tickSize, res);
    }

    //Should have stopped at planted entity
    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.z(), 13, 0.1f);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, plantedPos);

    domain->removeEntity(*plantedEntity);
    domain->tick(1.0, res);

    //Should have moved two more meters as planted entity was removed.
    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.z(), 15, 0.1f);

}

void PhysicalDomainIntegrationTest::test_mode()
{

    double tickSize = 1.0 / 15.0;

    ModeProperty* modePlantedProperty = new ModeProperty();
    modePlantedProperty->set("planted");
    ModeProperty* modeFixedProperty = new ModeProperty();
    modeFixedProperty->set("fixed");
    ModeProperty* modeFreeProperty = new ModeProperty();
    modeFreeProperty->set("");
    TypeNode* rockType = new TypeNode("rock");

    Entity* rootEntity = new Entity("0", newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    Mercator::Terrain& terrain = terrainProperty->getData();
    terrain.setBasePoint(0, 0, Mercator::BasePoint(40));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(40));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
    rootEntity->setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    Property<double>* massProp = new Property<double>();
    massProp->data() = 100;

    Entity* freeEntity1 = new Entity("free1", newId());
    freeEntity1->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity1->setType(rockType);
    freeEntity1->m_location.m_pos = WFMath::Point<3>(10, 30, 10);
    freeEntity1->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity1);

    ASSERT_EQUAL(freeEntity1->m_location.m_pos, WFMath::Point<3>(10, 30, 10));

    //The other free entity is placed below the terrain; it's expected to then be clamped to the terrain
    Entity* freeEntity2 = new Entity("free2", newId());
    freeEntity2->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    freeEntity2->setType(rockType);
    freeEntity2->m_location.m_pos = WFMath::Point<3>(20, -10, 20);
    freeEntity2->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity2);
    ASSERT_EQUAL(freeEntity2->m_location.m_pos, WFMath::Point<3>(20, 22.6006, 20));

    Entity* plantedEntity = new Entity("planted", newId());
    plantedEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    plantedEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty));
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(30, 10, 30);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*plantedEntity);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 18.4325, 30));


    Entity* fixedEntity = new Entity("fixed", newId());
    fixedEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
    fixedEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modeFixedProperty));
    fixedEntity->setType(rockType);
    fixedEntity->m_location.m_pos = WFMath::Point<3>(40, 50, 40);
    fixedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*fixedEntity);
    ASSERT_EQUAL(fixedEntity->m_location.m_pos, WFMath::Point<3>(40, 50, 40));


    OpVector res;

    //Inject ticks for two seconds
    for (int i = 0; i < 30; ++i) {
        domain->tick(tickSize, res);
    }

    ASSERT_NOT_EQUAL(freeEntity1->m_location.m_pos, WFMath::Point<3>(10, 30, 10));
    ASSERT_NOT_EQUAL(freeEntity2->m_location.m_pos, WFMath::Point<3>(20, 22.6006, 20));
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 18.4325, 30));
    ASSERT_EQUAL(fixedEntity->m_location.m_pos, WFMath::Point<3>(40, 50, 40));
}


void PhysicalDomainIntegrationTest::test_determinism()
{

    double tickSize = 1.0 / 15.0;

    TypeNode* rockType = new TypeNode("rock");

    Entity* rootEntity = new Entity("0", newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    Mercator::Terrain& terrain = terrainProperty->getData();
    terrain.setBasePoint(0, 0, Mercator::BasePoint(40));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(40));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
    rootEntity->setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(0, 0, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    Property<double>* massProp = new Property<double>();
    massProp->data() = 100;

    std::vector<Entity*> entities;

    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            long id = newId();
            std::stringstream ss;
            ss << "free" << id;
            Entity* freeEntity = new Entity(ss.str(), id);
            freeEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
            freeEntity->setType(rockType);
            freeEntity->m_location.m_pos = WFMath::Point<3>(i, j, i + j);
            freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.25f, 0, -0.25f), WFMath::Point<3>(-0.25f, 0.5f, -0.25f)));
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
//    ASSERT_EQUAL(entities[0]->m_location.m_pos, WFMath::Point<3>(0.495543, -0, 19.949));
//    ASSERT_EQUAL(entities[10]->m_location.m_pos, WFMath::Point<3>(1, -0, 18.0217));
//    ASSERT_EQUAL(entities[15]->m_location.m_pos, WFMath::Point<3>(2.61429, 0.0884429, 26.4489));
//    ASSERT_EQUAL(entities[16]->m_location.m_pos, WFMath::Point<3>(0.948305, 0.105805, 18.7352));
//    ASSERT_EQUAL(entities[55]->m_location.m_pos, WFMath::Point<3>(6.30361, -1.19749f, 28.0569));

}

void PhysicalDomainIntegrationTest::test_zoffset()
{


    TypeNode* rockType = new TypeNode("rock");
    ModeProperty* modePlantedProperty = new ModeProperty();
    modePlantedProperty->set("planted");

    Property<double>* plantedOffset = new Property<double>();
    plantedOffset->data() = -2;

    Ref<Entity> rootEntity = new Entity("0", newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    Mercator::Terrain& terrain = terrainProperty->getData();
    terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
    rootEntity->setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(0, -64, 0), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    TestWorld testWorld(rootEntity);


    Ref<Entity> plantedEntity = new Entity("planted", newId());
    plantedEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty));
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(30, 10, 30);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 10, 1)));
    plantedEntity->setProperty("planted_offset", std::unique_ptr<PropertyBase>(plantedOffset));
    domain->addEntity(*plantedEntity);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 8.01695, 30));

    plantedOffset->data() = -3;
    plantedOffset->apply(plantedEntity.get());
    plantedEntity->propertyApplied.emit("planted_offset", *plantedOffset);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 7.01695, 30));

}


void PhysicalDomainIntegrationTest::test_zscaledoffset()
{
    TypeNode* rockType = new TypeNode("rock");
    ModeProperty* modePlantedProperty = new ModeProperty();
    modePlantedProperty->set("planted");

    Property<double>* plantedScaledOffset = new Property<double>();
    plantedScaledOffset->data() = -0.2;

    Ref<Entity> rootEntity = new Entity("0", newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    Mercator::Terrain& terrain = terrainProperty->getData();
    terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
    rootEntity->setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(0, -64, 0), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    TestWorld testWorld(rootEntity);


    Ref<Entity> plantedEntity = new Entity("planted", newId());
    plantedEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty));
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(30, 10, 30);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 10, 10)));
    plantedEntity->setProperty("planted_scaled_offset", std::unique_ptr<PropertyBase>(plantedScaledOffset));
    domain->addEntity(*plantedEntity);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 8.01695, 30));

    plantedScaledOffset->data() = -0.3;
    plantedScaledOffset->apply(plantedEntity.get());
    plantedEntity->propertyApplied.emit("planted_offset", *plantedScaledOffset);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 7.01695, 30));

}

void PhysicalDomainIntegrationTest::test_visibility()
{
    TypeNode* rockType = new TypeNode("rock");
    TypeNode* humanType = new TypeNode("human");
    ModeProperty* modePlantedProperty = new ModeProperty();
    modePlantedProperty->set("planted");

    VisibilityProperty* visibilityProperty = new VisibilityProperty();
    visibilityProperty->set(1000.f);


    Ref<Entity> rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, 0, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    TestWorld testWorld(rootEntity);

    Ref<Entity> smallEntity1 = new Entity("small1", newId());
    smallEntity1->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty->copy()));
    smallEntity1->setType(rockType);
    smallEntity1->m_location.m_pos = WFMath::Point<3>(30, 0, 30);
    smallEntity1->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, 0, -0.2f), WFMath::Point<3>(0.2, 0.4, 0.2)));
    domain->addEntity(*smallEntity1);

    Ref<Entity> smallEntity2 = new Entity("small2", newId());
    smallEntity2->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty->copy()));
    smallEntity2->setType(rockType);
    smallEntity2->m_location.m_pos = WFMath::Point<3>(-31, 0, -31);
    smallEntity2->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, 0, -0.2f), WFMath::Point<3>(0.2, 0.4, 0.2)));
    domain->addEntity(*smallEntity2);

    //This entity should always be seen, as "visibility" is specified.
    Ref<Entity> smallVisibleEntity = new Entity("smallVisible", newId());
    smallVisibleEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty->copy()));
    smallVisibleEntity->setType(rockType);
    smallVisibleEntity->m_location.m_pos = WFMath::Point<3>(-63, 0, -63);
    smallVisibleEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, 0, -0.2f), WFMath::Point<3>(0.2, 0.4, 0.2)));
    smallVisibleEntity->setProperty("visibility", std::unique_ptr<PropertyBase>(visibilityProperty));
    domain->addEntity(*smallVisibleEntity);

    Ref<Entity> largeEntity1 = new Entity("large1", newId());
    largeEntity1->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty->copy()));
    largeEntity1->setType(rockType);
    largeEntity1->m_location.m_pos = WFMath::Point<3>(0, 0, 0);
    largeEntity1->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-10.f, 0, -10.f), WFMath::Point<3>(10, 20, 10)));
    domain->addEntity(*largeEntity1);

    Ref<Entity> observerEntity = new Entity("observer", newId());
    observerEntity->setType(humanType);
    observerEntity->m_location.m_pos = WFMath::Point<3>(-30, 0, -30);
    observerEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, 0, -0.2f), WFMath::Point<3>(0.2, 2, 0.2)));
    observerEntity->addFlags(entity_perceptive);
    domain->addEntity(*observerEntity);

    OpVector res;
    std::set<LocatedEntity*> transformedEntities;
    domain->tick(0.1, res);

    ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *observerEntity));

    {
        ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *smallVisibleEntity));
        ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *smallEntity2));
        ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *largeEntity1));
        ASSERT_FALSE(domain->isEntityVisibleFor(*observerEntity, *smallEntity1));

        std::list<LocatedEntity*> observedList;
        domain->getVisibleEntitiesFor(*observerEntity, observedList);

        ASSERT_EQUAL(5u, observedList.size());
        ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "small2"; }) != observedList.end());
        ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "smallVisible"; }) != observedList.end());
        ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "large1"; }) != observedList.end());
        ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "observer"; }) != observedList.end());

    }
    //Now move the observer to "small1"
    domain->applyTransform(*observerEntity, Domain::TransformData{WFMath::Quaternion(), WFMath::Point<3>(30, 0, 30), WFMath::Vector<3>(), nullptr, {}}, transformedEntities);
    //Force visibility updates
    domain->tick(2, res);
    {
        ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *smallVisibleEntity));
        ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *smallEntity1));
        ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *largeEntity1));
        ASSERT_FALSE(domain->isEntityVisibleFor(*observerEntity, *smallEntity2));

        std::list<LocatedEntity*> observedList;

        domain->getVisibleEntitiesFor(*observerEntity, observedList);

        ASSERT_EQUAL(5u, observedList.size());
        ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "small1"; }) != observedList.end());
        ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "smallVisible"; }) != observedList.end());
        ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "large1"; }) != observedList.end());
        ASSERT_TRUE(std::find_if(observedList.begin(), observedList.end(), [](const LocatedEntity* entity) { return entity->getId() == "observer"; }) != observedList.end());

    }
}

void PhysicalDomainIntegrationTest::test_stairs()
{
    TypeNode* rockType = new TypeNode("rock");
    TypeNode* humanType = new TypeNode("human");
    ModeProperty* modePlantedProperty = new ModeProperty();
    modePlantedProperty->set("planted");
    Property<double>* massProp = new Property<double>();
    massProp->data() = 100;
    auto speedGroundProperty = new Property<double>();
    speedGroundProperty->data() = 5.0;
    PropelProperty* propelProperty = new PropelProperty();
    propelProperty->data() = WFMath::Vector<3>(0, 0, 1.0 / speedGroundProperty->data());
    AngularFactorProperty angularZeroFactorProperty;
    angularZeroFactorProperty.data() = WFMath::Vector<3>::ZERO();
    GeometryProperty capsuleProperty;
    capsuleProperty.set(Atlas::Message::MapType({{"type", "capsule-y"}}));
//    Property<double>* stepFactorProp = new Property<double>();
//    stepFactorProp->data() = 0.3;

    humanType->injectProperty("speed_ground", std::unique_ptr<PropertyBase>(speedGroundProperty));


    Ref<Entity> rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, 0, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));

    TestWorld testWorld(rootEntity);


    //Create 10 entities at increasing height, forming a stair.
    for (int i = 0; i < 10; ++i) {
        std::stringstream ss;
        long id = newId();
        ss << "step" << id;
        Ref<Entity> stepElement = new Entity(ss.str(), id);
        stepElement->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty->copy()));
        float height = 0.1f + (i * 0.1f);
        float zPos = i * 0.2f;
        WFMath::Point<3> pos(0, 0, zPos);
        WFMath::AxisBox<3> bbox(WFMath::Point<3>(-0.4f, 0, -0.1f), WFMath::Point<3>(0.4f, height, 0.1f));
        stepElement->m_location.setBBox(bbox);
        stepElement->m_location.m_pos = pos;
        stepElement->setType(rockType);


        domain->addEntity(*stepElement);
    }

    //First with an entity which doesn't step; it should collide and be kept in place
    {
        Ref<Entity> human = new Entity("human", newId());
        human->setProperty(AngularFactorProperty::property_name, std::unique_ptr<PropertyBase>(angularZeroFactorProperty.copy()));
        human->setProperty("mass", std::unique_ptr<PropertyBase>(massProp->copy()));
        human->setProperty(PropelProperty::property_name, std::unique_ptr<PropertyBase>(propelProperty->copy()));
        human->setType(humanType);
        human->m_location.m_pos = WFMath::Point<3>(0, 0, -1);
        human->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.4f, 0, -0.4f), WFMath::Point<3>(0.4, 1.8, 0.4)));
        domain->addEntity(*human);

        OpVector res;
        domain->tick(2, res);

        ASSERT_FUZZY_EQUAL(-0.5f, human->m_location.m_pos.z(), 0.1f);
        domain->removeEntity(*human);
    }

    //Then with an entity with a capsule geometry, it should step
    {
        Ref<Entity> human = new Entity("human", newId());
        //human->setProperty("step_factor", stepFactorProp));
        human->setProperty(AngularFactorProperty::property_name, std::unique_ptr<PropertyBase>(angularZeroFactorProperty.copy()));
        human->setProperty("mass", std::unique_ptr<PropertyBase>(massProp->copy()));
        human->setProperty(PropelProperty::property_name, std::unique_ptr<PropertyBase>(propelProperty->copy()));
        human->setProperty(GeometryProperty::property_name, std::unique_ptr<PropertyBase>(capsuleProperty.copy()));
        human->setType(humanType);
        human->m_location.m_pos = WFMath::Point<3>(0, 0, -1);
        human->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.4f, 0, -0.4f), WFMath::Point<3>(0.4, 1.8, 0.4)));
        domain->addEntity(*human);

        OpVector res;
        domain->tick(2, res);

        ASSERT_FUZZY_EQUAL(0.5, human->m_location.m_pos.z(), 0.3f);
        domain->removeEntity(*human);
    }

    //Also place a tilted square entity, which is too tilted to allow for stepping on
    //The human entity shouldn't step up on the tilted entity
    {

        long id = newId();
        Ref<Entity> stepElement = new Entity("tilted", id);
        stepElement->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty->copy()));
        WFMath::Point<3> pos(20, 0, 0);
        WFMath::AxisBox<3> bbox(WFMath::Point<3>(-0.4f, 0.f, 0), WFMath::Point<3>(0.4f, 1, 0.4f));
        stepElement->m_location.m_orientation.rotate(WFMath::Quaternion(0, WFMath::numeric_constants<float>::pi() * 0.2f));
        stepElement->m_location.setBBox(bbox);
        stepElement->m_location.m_pos = pos;
        stepElement->setType(rockType);


        domain->addEntity(*stepElement);

        Ref<Entity> human = new Entity("human", newId());
        //human->setProperty("step_factor", stepFactorProp));
        human->setProperty(AngularFactorProperty::property_name, std::unique_ptr<PropertyBase>(angularZeroFactorProperty.copy()));
        human->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
        human->setProperty(PropelProperty::property_name, std::unique_ptr<PropertyBase>(propelProperty->copy()));
        human->setProperty(GeometryProperty::property_name, std::unique_ptr<PropertyBase>(capsuleProperty.copy()));
        human->setType(humanType);
        human->m_location.m_pos = WFMath::Point<3>(20, 0, -1);
        human->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.4f, 0, -0.4f), WFMath::Point<3>(0.4, 1.8, 0.4)));
        domain->addEntity(*human);

        OpVector res;
        domain->tick(2, res);

        ASSERT_FUZZY_EQUAL(0, human->m_location.m_pos.y(), 0.01f);
        ASSERT_FUZZY_EQUAL(-0.4f, human->m_location.m_pos.z(), 0.1f);
    }


}

void PhysicalDomainIntegrationTest::test_terrainPrecision()
{

    Ref<Entity> rootEntity = new Entity("0", newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    Mercator::Terrain& terrain = terrainProperty->getData();
    terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(15));
    terrain.setBasePoint(0, -1, Mercator::BasePoint(15));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(20));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(25));
    terrain.setBasePoint(1, -1, Mercator::BasePoint(30));
    terrain.setBasePoint(-1, 0, Mercator::BasePoint(35));
    terrain.setBasePoint(-1, 1, Mercator::BasePoint(40));
    terrain.setBasePoint(-1, -1, Mercator::BasePoint(45));
    rootEntity->setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    std::unique_ptr<TestPhysicalDomain> domain(new TestPhysicalDomain(*rootEntity));


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


int main()
{
    PhysicalDomainIntegrationTest t;

    return t.run();
}

