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

#include "common/debug.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <wfmath/atlasconv.h>

#include <rulesets/PhysicalDomain.h>
#include <common/TypeNode.h>
#include "physics/Convert.h"
#include <rulesets/TerrainProperty.h>
#include <Mercator/BasePoint.h>
#include <Mercator/Terrain.h>
#include <rulesets/PropelProperty.h>
#include <rulesets/AngularFactorProperty.h>
#include <rulesets/VisibilityProperty.h>
#include <rulesets/GeometryProperty.h>
#include "rulesets/PhysicalWorld.h"
#include "rulesets/BBoxProperty.h"
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

#include <chrono>
#include <rulesets/TerrainModProperty.h>

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
        TestPhysicalDomain(LocatedEntity& entity) :
            PhysicalDomain(entity)
        {

        }


        PhysicalWorld* getPhysicalWorld() const
        {
            return m_dynamicsWorld;
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

        void test_convert();

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

void PhysicalDomainIntegrationTest::test_terrainMods()
{

    class TestPhysicalDomain : public PhysicalDomain
    {
        public:
            explicit TestPhysicalDomain(LocatedEntity& entity) : PhysicalDomain(entity)
            {
            }

            btDiscreteDynamicsWorld* test_getBulletWorld()
            {
                return m_dynamicsWorld;
            }

    };

    Entity* rootEntity = new Entity("0", newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    Mercator::Terrain& terrain = terrainProperty->getData();
    terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
    rootEntity->setProperty("terrain", terrainProperty);
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    TestPhysicalDomain* domain = new TestPhysicalDomain(*rootEntity);

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("planted");

    Entity* terrainModEntity = new Entity("1", newId());
    terrainModEntity->m_location.m_pos = WFMath::Point<3>(32, 10, 32);
    terrainModEntity->setProperty(ModeProperty::property_name, modeProperty);
    TerrainModProperty* terrainModProperty = new TerrainModProperty();

    Atlas::Message::MapType modElement{
        {"heightoffset", -5.0f},
        {"shape",        MapType{
            {"points", ListType {
                ListType {-10.f, -10.f},
                ListType {10.f, -10.f},
                ListType {10.f, 10.f},
                ListType {-10.f, 10.f},
            }
            },
            {"type",   "polygon"}
        }
        },
        {"type",         "levelmod"}
    };

    terrainModProperty->set(modElement);
    terrainModEntity->setProperty(TerrainModProperty::property_name, terrainModProperty);
    terrainModProperty->apply(terrainModEntity);

    domain->addEntity(*terrainModEntity);

    OpVector res;

    domain->tick(0, res);


    ASSERT_FUZZY_EQUAL(terrain.get(10, 10), 10.0f, 0.1f);
    ASSERT_TRUE(terrain.hasMod(terrainModEntity->getIntId()));
    ASSERT_FUZZY_EQUAL(terrain.get(32, 32), 5.0f, 0.1f);


    {
        btVector3 rayFrom(32, 32, 32);
        btVector3 rayTo(32, -32, 32);
        btCollisionWorld::ClosestRayResultCallback callback(rayFrom, rayTo);
        domain->test_getBulletWorld()->rayTest(rayFrom, rayTo, callback);

        ASSERT_FUZZY_EQUAL(callback.m_hitPointWorld.y(), 5.0f, 0.1f);
    }
    domain->applyTransform(*terrainModEntity, WFMath::Quaternion(), WFMath::Point<3>(10, 10, 10), WFMath::Vector<3>());

    domain->tick(0, res);

    ASSERT_FUZZY_EQUAL(terrain.get(10, 10), 5.0f, 0.1f);
    ASSERT_TRUE(terrain.hasMod(terrainModEntity->getIntId()));
    ASSERT_FUZZY_EQUAL(terrain.get(32, 32), 10.0f, 0.1f);


    {
        btVector3 rayFrom(32, 32, 32);
        btVector3 rayTo(32, -32, 32);
        btCollisionWorld::ClosestRayResultCallback callback(rayFrom, rayTo);
        domain->test_getBulletWorld()->rayTest(rayFrom, rayTo, callback);

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

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    Property<int>* waterBodyProp = new Property<int>();
    waterBodyProp->data() = 1;

    ModeProperty* modeFreeProperty = new ModeProperty();
    modeFreeProperty->set("free");
    rockType->injectProperty("mode", modeFreeProperty);

    ModeProperty* modeFixedProperty = new ModeProperty();
    modeFixedProperty->set("fixed");


    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, 0, -64), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    long id = newId();
    TestEntity* lake = new TestEntity(std::to_string(id), id);
    lake->setProperty(ModeProperty::property_name, modeFixedProperty);
    lake->setType(lakeType);
    lake->setProperty("water_body", waterBodyProp);
    lake->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(0, -64, 0), WFMath::Point<3>(10, 0, 2)));
    lake->m_location.m_pos = WFMath::Point<3>(0, 10, 0);
    //rotate 90 degrees
    lake->m_location.m_orientation = WFMath::Quaternion(1, -WFMath::numeric_constants<float>::pi() / 2.0f);
    domain->addEntity(*lake);

    //Should be in water
    id = newId();
    Entity* freeEntity = new Entity("freeEntity", id);
    freeEntity->setProperty("mass", massProp);
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(-1, 1, 9);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    //Should not be in water
    id = newId();
    Entity* freeEntity2 = new Entity("freeEntity2", id);
    freeEntity2->setProperty("mass", massProp);
    freeEntity2->setType(rockType);
    freeEntity2->m_location.m_pos = WFMath::Point<3>(9, 1, 1);
    freeEntity2->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity2);

    OpVector res;
    domain->tick(0, res);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);
    ASSERT_TRUE(freeEntity2->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

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

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    Property<int>* waterBodyProp = new Property<int>();
    waterBodyProp->data() = 1;

    ModeProperty* modeFreeProperty = new ModeProperty();
    modeFreeProperty->set("free");
    rockType->injectProperty("mode", modeFreeProperty);

    ModeProperty* modeFixedProperty = new ModeProperty();
    modeFixedProperty->set("fixed");


    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    long id = newId();
    TestEntity* lake = new TestEntity(std::to_string(id), id);
    lake->setProperty(ModeProperty::property_name, modeFixedProperty);
    lake->setType(lakeType);
    lake->setProperty("water_body", waterBodyProp);
    lake->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-5, -64, -5), WFMath::Point<3>(5, 0, 5)));
    lake->m_location.m_pos = WFMath::Point<3>(20, 0, 0);
    lake->m_location.m_orientation = WFMath::Quaternion::IDENTITY();
    domain->addEntity(*lake);

    id = newId();
    Entity* freeEntity = new Entity(std::to_string(id), id);
    freeEntity->setProperty("mass", massProp);
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(20, 2, 0);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    //The second entity is placed in water, and should be submerged from the start
    id = newId();
    Entity* freeEntity2 = new Entity(std::to_string(id), id);
    freeEntity2->setProperty("mass", massProp);
    freeEntity2->setType(rockType);
    freeEntity2->m_location.m_pos = WFMath::Point<3>(20, -2, 2);
    freeEntity2->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity2);

    //The third entity is placed outside of the lake, and should never be submerged.
    id = newId();
    Entity* freeEntity3 = new Entity(std::to_string(id), id);
    freeEntity3->setProperty("mass", massProp);
    freeEntity3->setType(rockType);
    freeEntity3->m_location.m_pos = WFMath::Point<3>(-20, 2, 0);
    freeEntity3->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity3);

    OpVector res;
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
    domain->applyTransform(*freeEntity, WFMath::Quaternion::IDENTITY(), WFMath::Point<3>(20, 60, 0), WFMath::Vector<3>());
    domain->tick(0, res);
    ASSERT_TRUE(freeEntity->m_location.pos().y() > 0);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

    //Move back in.
    domain->applyTransform(*freeEntity, WFMath::Quaternion::IDENTITY(), WFMath::Point<3>(20, -10, 0), WFMath::Vector<3>());
    domain->tick(0, res);
    ASSERT_TRUE(freeEntity->m_location.pos().y() < 0);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Submerged);
    ASSERT_TRUE(freeEntity3->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

    //Move the lake to where freeEntity3 is
    domain->applyTransform(*lake, WFMath::Quaternion(), freeEntity3->m_location.m_pos + WFMath::Vector<3>(0, 5, 0), WFMath::Vector<3>());
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
    lake->setProperty("bbox", bBoxProperty);
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

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    Property<int>* waterBodyProp = new Property<int>();
    waterBodyProp->data() = 1;

    ModeProperty* modeFreeProperty = new ModeProperty();
    modeFreeProperty->set("free");
    ModeProperty* modeFixedProperty = new ModeProperty();
    modeFixedProperty->set("fixed");

    rockType->injectProperty("mode", modeFreeProperty);


    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    long id = newId();
    Entity* ocean = new Entity(std::to_string(id), id);
    ocean->setProperty(ModeProperty::property_name, modeFixedProperty);
    ocean->setType(oceanType);
    ocean->setProperty("water_body", waterBodyProp);
    ocean->m_location.m_pos = WFMath::Point<3>(0, 0, 0);
    ocean->m_location.m_orientation = WFMath::Quaternion::IDENTITY();
    domain->addEntity(*ocean);

    id = newId();
    Entity* freeEntity = new Entity(std::to_string(id), id);
    freeEntity->setProperty("mass", massProp);
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(0, 2, 0);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    //The second entity is placed in water, and should be submerged from the start
    id = newId();
    Entity* freeEntity2 = new Entity(std::to_string(id), id);
    freeEntity2->setProperty("mass", massProp);
    freeEntity2->setType(rockType);
    freeEntity2->m_location.m_pos = WFMath::Point<3>(10, -10, 0);
    freeEntity2->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity2);


    OpVector res;
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
    domain->applyTransform(*freeEntity, WFMath::Quaternion::IDENTITY(), WFMath::Point<3>(0, 60, 0), WFMath::Vector<3>());
    domain->tick(0, res);
    ASSERT_TRUE(freeEntity->m_location.pos().y() > 0);
    ASSERT_TRUE(freeEntity->getPropertyClassFixed<ModeProperty>()->getMode() == ModeProperty::Mode::Free);

    //Move back in.
    domain->applyTransform(*freeEntity, WFMath::Quaternion::IDENTITY(), WFMath::Point<3>(0, -10, 0), WFMath::Vector<3>());
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
    class TestPhysicalDomain : public PhysicalDomain
    {
        public:
            explicit TestPhysicalDomain(LocatedEntity& entity) : PhysicalDomain(entity)
            {
            }

            btDiscreteDynamicsWorld* test_getBulletWorld()
            {
                return m_dynamicsWorld;
            }

            btRigidBody* test_getRigidBody(long id)
            {
                return btRigidBody::upcast(m_entries.find(id)->second->collisionObject);
            }
    };

    TypeNode* rockType = new TypeNode("rock");

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("fixed");


    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    TestPhysicalDomain* domain = new TestPhysicalDomain(*rootEntity);

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

        //Change pos only
        domain->applyTransform(*entity, WFMath::Quaternion(), WFMath::Point<3>(20, 30, 1), WFMath::Vector<3>());

        verifyBboxes(entity);

        //Change orientation only
        domain->applyTransform(*entity, WFMath::Quaternion(1, WFMath::numeric_constants<float>::pi() / 3.0f), WFMath::Point<3>(), WFMath::Vector<3>());

        verifyBboxes(entity);

        //Change pos and orientation
        domain->applyTransform(*entity, WFMath::Quaternion(1, WFMath::numeric_constants<float>::pi() / 5.0f), WFMath::Point<3>(10, -25, 6), WFMath::Vector<3>());

        verifyBboxes(entity);

        //Change velocity (should not change pos and orientation)
        domain->applyTransform(*entity, WFMath::Quaternion(), WFMath::Point<3>(), WFMath::Vector<3>(4, 4, 4));

        verifyBboxes(entity);
    };


    //Start with a box centered at origo, with no orientation
    {
        long id = newId();
        Entity* entity = new Entity(std::to_string(id), id);
        entity->setProperty(ModeProperty::property_name, modeProperty);
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
        entity->setProperty(ModeProperty::property_name, modeProperty);
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
        entity->setProperty(ModeProperty::property_name, modeProperty);
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
        entity->setProperty(ModeProperty::property_name, modeProperty);
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
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    TypeNode* rockType = new TypeNode("rock");


    Entity* freeEntity = new Entity("1", newId());
    freeEntity->setProperty("mass", massProp);
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    Entity* fixedEntity = new Entity("2", newId());
    fixedEntity->setProperty("mass", massProp);

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("fixed");
    fixedEntity->setProperty(ModeProperty::property_name, modeProperty);
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
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    TypeNode* rockType = new TypeNode("rock");


    Entity* freeEntity = new Entity("1", newId());
    freeEntity->setProperty("mass", massProp);
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(0, 1, 0);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    Entity* fixedEntity = new Entity("2", newId());
    fixedEntity->setProperty("mass", massProp);

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("fixed");
    fixedEntity->setProperty(ModeProperty::property_name, modeProperty);
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
    rootEntity->setProperty("terrain", terrainProperty);
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    TypeNode* rockType = new TypeNode("rock");


    Entity* freeEntity = new Entity("1", newId());
    freeEntity->setProperty("mass", massProp);
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(10, 20, 10);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    Entity* plantedEntity = new Entity("2", newId());
    plantedEntity->setProperty("mass", massProp);

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("planted");

    plantedEntity->setProperty(ModeProperty::property_name, modeProperty);
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
    rootEntity->setProperty("terrain", terrainProperty);
    rootEntity->setProperty("friction", zeroFrictionProperty);
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    Property<double>* massProp = new Property<double>();
    massProp->data() = 100;

    TypeNode* rockType = new TypeNode("rock");


    PropelProperty* propelProperty = new PropelProperty();
    //Move y axis 2 meter per second.
    propelProperty->data() = WFMath::Vector<3>(0, 0, 2.0 / speedGroundProperty->data());

    AngularFactorProperty angularZeroFactorProperty;
    angularZeroFactorProperty.data() = WFMath::Vector<3>::ZERO();


    Entity* freeEntity = new Entity("1", newId());
    freeEntity->setProperty(PropelProperty::property_name, propelProperty);
    freeEntity->setProperty("mass", massProp);
    freeEntity->setProperty("friction", zeroFrictionProperty);
    freeEntity->setProperty("speed-ground", speedGroundProperty);
    freeEntity->setProperty(AngularFactorProperty::property_name, &angularZeroFactorProperty);
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(10, 10, 10);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));

    domain->addEntity(*freeEntity);

    Entity* plantedEntity = new Entity("2", newId());
    plantedEntity->setProperty("mass", massProp);

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("planted");

    plantedEntity->setProperty(ModeProperty::property_name, modeProperty);
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
    rootEntity->setProperty("terrain", terrainProperty);
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    Property<double>* massProp = new Property<double>();
    massProp->data() = 100;

    Entity* freeEntity1 = new Entity("free1", newId());
    freeEntity1->setProperty("mass", massProp);
    freeEntity1->setType(rockType);
    freeEntity1->m_location.m_pos = WFMath::Point<3>(10, 30, 10);
    freeEntity1->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity1);

    ASSERT_EQUAL(freeEntity1->m_location.m_pos, WFMath::Point<3>(10, 30, 10));

    //The other free entity is placed below the terrain; it's expected to then be clamped to the terrain
    Entity* freeEntity2 = new Entity("free2", newId());
    freeEntity2->setProperty("mass", massProp);
    freeEntity2->setType(rockType);
    freeEntity2->m_location.m_pos = WFMath::Point<3>(20, -10, 20);
    freeEntity2->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity2);
    ASSERT_EQUAL(freeEntity2->m_location.m_pos, WFMath::Point<3>(20, 22.6006, 20));

    Entity* plantedEntity = new Entity("planted", newId());
    plantedEntity->setProperty("mass", massProp);
    plantedEntity->setProperty(ModeProperty::property_name, modePlantedProperty);
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(30, 10, 30);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*plantedEntity);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 18.4325, 30));


    Entity* fixedEntity = new Entity("fixed", newId());
    fixedEntity->setProperty("mass", massProp);
    fixedEntity->setProperty(ModeProperty::property_name, modeFixedProperty);
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
    rootEntity->setProperty("terrain", terrainProperty);
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(0, 0, -64), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    Property<double>* massProp = new Property<double>();
    massProp->data() = 100;

    std::vector<Entity*> entities;

    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            long id = newId();
            std::stringstream ss;
            ss << "free" << id;
            Entity* freeEntity = new Entity(ss.str(), id);
            freeEntity->setProperty("mass", massProp);
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

    Entity* rootEntity = new Entity("0", newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    Mercator::Terrain& terrain = terrainProperty->getData();
    terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
    rootEntity->setProperty("terrain", terrainProperty);
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(0, -64, 0), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    TestWorld testWorld(*rootEntity);


    Entity* plantedEntity = new Entity("planted", newId());
    plantedEntity->setProperty(ModeProperty::property_name, modePlantedProperty);
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(30, 10, 30);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 10, 1)));
    plantedEntity->setProperty("planted-offset", plantedOffset);
    domain->addEntity(*plantedEntity);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 8.01695, 30));

    plantedOffset->data() = -3;
    plantedOffset->apply(plantedEntity);
    plantedEntity->propertyApplied.emit("planted-offset", *plantedOffset);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 7.01695, 30));

}


void PhysicalDomainIntegrationTest::test_zscaledoffset()
{
    TypeNode* rockType = new TypeNode("rock");
    ModeProperty* modePlantedProperty = new ModeProperty();
    modePlantedProperty->set("planted");

    Property<double>* plantedScaledOffset = new Property<double>();
    plantedScaledOffset->data() = -0.2;

    Entity* rootEntity = new Entity("0", newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    Mercator::Terrain& terrain = terrainProperty->getData();
    terrain.setBasePoint(0, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
    rootEntity->setProperty("terrain", terrainProperty);
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(0, -64, 0), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    TestWorld testWorld(*rootEntity);


    Entity* plantedEntity = new Entity("planted", newId());
    plantedEntity->setProperty(ModeProperty::property_name, modePlantedProperty);
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(30, 10, 30);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, 0, -1), WFMath::Point<3>(1, 10, 10)));
    plantedEntity->setProperty("planted-scaled-offset", plantedScaledOffset);
    domain->addEntity(*plantedEntity);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 8.01695, 30));

    plantedScaledOffset->data() = -0.3;
    plantedScaledOffset->apply(plantedEntity);
    plantedEntity->propertyApplied.emit("planted-offset", *plantedScaledOffset);
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


    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, 0, -64), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    TestWorld testWorld(*rootEntity);

    Entity* smallEntity1 = new Entity("small1", newId());
    smallEntity1->setProperty(ModeProperty::property_name, modePlantedProperty);
    smallEntity1->setType(rockType);
    smallEntity1->m_location.m_pos = WFMath::Point<3>(30, 0, 30);
    smallEntity1->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, 0, -0.2f), WFMath::Point<3>(0.2, 0.4, 0.2)));
    domain->addEntity(*smallEntity1);

    Entity* smallEntity2 = new Entity("small2", newId());
    smallEntity2->setProperty(ModeProperty::property_name, modePlantedProperty);
    smallEntity2->setType(rockType);
    smallEntity2->m_location.m_pos = WFMath::Point<3>(-31, 0, -31);
    smallEntity2->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, 0, -0.2f), WFMath::Point<3>(0.2, 0.4, 0.2)));
    domain->addEntity(*smallEntity2);

    //This entity should always be seen, as "visibility" is specified.
    Entity* smallVisibleEntity = new Entity("smallVisible", newId());
    smallVisibleEntity->setProperty(ModeProperty::property_name, modePlantedProperty);
    smallVisibleEntity->setType(rockType);
    smallVisibleEntity->m_location.m_pos = WFMath::Point<3>(-63, 0, -63);
    smallVisibleEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, 0, -0.2f), WFMath::Point<3>(0.2, 0.4, 0.2)));
    smallVisibleEntity->setProperty("visibility", visibilityProperty);
    domain->addEntity(*smallVisibleEntity);

    Entity* largeEntity1 = new Entity("large1", newId());
    largeEntity1->setProperty(ModeProperty::property_name, modePlantedProperty);
    largeEntity1->setType(rockType);
    largeEntity1->m_location.m_pos = WFMath::Point<3>(0, 0, 0);
    largeEntity1->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-10.f, 0, -10.f), WFMath::Point<3>(10, 20, 10)));
    domain->addEntity(*largeEntity1);

    Entity* observerEntity = new Entity("observer", newId());
    observerEntity->setType(humanType);
    observerEntity->m_location.m_pos = WFMath::Point<3>(-30, 0, -30);
    observerEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, 0, -0.2f), WFMath::Point<3>(0.2, 2, 0.2)));
    observerEntity->setFlags(entity_perceptive);
    domain->addEntity(*observerEntity);

    OpVector res;
    domain->tick(0.1, res);

    ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *observerEntity));

    {
        ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *smallVisibleEntity));
        ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *smallEntity2));
        ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *largeEntity1));
        ASSERT_FALSE(domain->isEntityVisibleFor(*observerEntity, *smallEntity1));

        std::list<LocatedEntity*> observedList;
        domain->getVisibleEntitiesFor(*observerEntity, observedList);

        ASSERT_EQUAL(4u, observedList.size());
        auto I = observedList.begin();
        ASSERT_EQUAL("small2", (*I)->getId());
        ++I;
        ASSERT_EQUAL("smallVisible", (*I)->getId());
        ++I;
        ASSERT_EQUAL("large1", (*I)->getId());
        ++I;
        ASSERT_EQUAL("observer", (*I)->getId());
    }
    //Now move the observer to "small1"
    domain->applyTransform(*observerEntity, WFMath::Quaternion(), WFMath::Point<3>(30, 0, 30), WFMath::Vector<3>());
    //Force visibility updates
    domain->tick(2, res);
    {
        ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *smallVisibleEntity));
        ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *smallEntity1));
        ASSERT_TRUE(domain->isEntityVisibleFor(*observerEntity, *largeEntity1));
        ASSERT_FALSE(domain->isEntityVisibleFor(*observerEntity, *smallEntity2));

        std::list<LocatedEntity*> observedList;

        domain->getVisibleEntitiesFor(*observerEntity, observedList);

        ASSERT_EQUAL(4u, observedList.size());
        auto I = observedList.begin();
        ASSERT_EQUAL("small1", (*I)->getId());
        ++I;
        ASSERT_EQUAL("smallVisible", (*I)->getId());
        ++I;
        ASSERT_EQUAL("large1", (*I)->getId());
        ++I;
        ASSERT_EQUAL("observer", (*I)->getId());
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

    humanType->injectProperty("speed-ground", speedGroundProperty);


    Entity* rootEntity = new Entity("0", newId());
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, 0, -64), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    TestWorld testWorld(*rootEntity);


    //Create 10 entities at increasing height, forming a stair.
    for (int i = 0; i < 10; ++i) {
        std::stringstream ss;
        long id = newId();
        ss << "step" << id;
        Entity* stepElement = new Entity(ss.str(), id);
        stepElement->setProperty(ModeProperty::property_name, modePlantedProperty);
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
        Entity* human = new Entity("human", newId());
        human->setProperty(AngularFactorProperty::property_name, &angularZeroFactorProperty);
        human->setProperty("mass", massProp);
        human->setProperty(PropelProperty::property_name, propelProperty);
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
        Entity* human = new Entity("human", newId());
        //human->setProperty("step_factor", stepFactorProp);
        human->setProperty(AngularFactorProperty::property_name, &angularZeroFactorProperty);
        human->setProperty("mass", massProp);
        human->setProperty(PropelProperty::property_name, propelProperty);
        human->setProperty(GeometryProperty::property_name, &capsuleProperty);
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
        Entity* stepElement = new Entity("tilted", id);
        stepElement->setProperty(ModeProperty::property_name, modePlantedProperty);
        WFMath::Point<3> pos(20, 0, 0);
        WFMath::AxisBox<3> bbox(WFMath::Point<3>(-0.4f, 0.f, 0), WFMath::Point<3>(0.4f, 1, 0.4f));
        stepElement->m_location.m_orientation.rotate(WFMath::Quaternion(0, WFMath::numeric_constants<float>::pi() * 0.2f));
        stepElement->m_location.setBBox(bbox);
        stepElement->m_location.m_pos = pos;
        stepElement->setType(rockType);


        domain->addEntity(*stepElement);

        Entity* human = new Entity("human", newId());
        //human->setProperty("step_factor", stepFactorProp);
        human->setProperty(AngularFactorProperty::property_name, &angularZeroFactorProperty);
        human->setProperty("mass", massProp);
        human->setProperty(PropelProperty::property_name, propelProperty);
        human->setProperty(GeometryProperty::property_name, &capsuleProperty);
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

    Entity* rootEntity = new Entity("0", newId());
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
    rootEntity->setProperty("terrain", terrainProperty);
    rootEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, -64), WFMath::Point<3>(64, 64, 64)));
    TestPhysicalDomain* domain = new TestPhysicalDomain(*rootEntity);


    auto checkHeightFunc = [&](float x, float z) {
        PhysicalWorld* physicalWorld = domain->getPhysicalWorld();

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


void TestWorld::message(const Operation& op, LocatedEntity& ent)
{
}

LocatedEntity* TestWorld::addNewEntity(const std::string&,
                                       const Atlas::Objects::Entity::RootEntity&)
{
    return 0;
}

int main()
{
    PhysicalDomainIntegrationTest t;

    return t.run();
}

