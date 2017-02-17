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
#include <rulesets/PhysicalDomain.h>
#include <common/TypeNode.h>
#include <rulesets/ModeProperty.h>
#include <rulesets/TerrainProperty.h>
#include <Mercator/BasePoint.h>
#include <Mercator/Terrain.h>
#include <rulesets/PropelProperty.h>
#include <rulesets/AngularFactorProperty.h>
#include <chrono>
#include <rulesets/VisibilityProperty.h>

#include "stubs/common/stubLog.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using String::compose;


class PhysicalDomainIntegrationTest : public Cyphesis::TestBase
{
    protected:
        static long m_id_counter;

    public:
        PhysicalDomainIntegrationTest();

        static long newId();

        void setup();

        void teardown();

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
};

long PhysicalDomainIntegrationTest::m_id_counter = 0L;

PhysicalDomainIntegrationTest::PhysicalDomainIntegrationTest()
{
    ADD_TEST(PhysicalDomainIntegrationTest::test_fallToBottom);
    ADD_TEST(PhysicalDomainIntegrationTest::test_standOnFixed);
    ADD_TEST(PhysicalDomainIntegrationTest::test_fallToTerrain);
    ADD_TEST(PhysicalDomainIntegrationTest::test_collision);
    ADD_TEST(PhysicalDomainIntegrationTest::test_mode);
    ADD_TEST(PhysicalDomainIntegrationTest::test_determinism);
    ADD_TEST(PhysicalDomainIntegrationTest::test_zoffset);
    ADD_TEST(PhysicalDomainIntegrationTest::test_zscaledoffset);
    ADD_TEST(PhysicalDomainIntegrationTest::test_visibility);

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
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    Entity* fixedEntity = new Entity("2", newId());
    fixedEntity->setProperty("mass", massProp);

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("fixed");
    fixedEntity->setProperty(ModeProperty::property_name, modeProperty);
    fixedEntity->setType(rockType);
    fixedEntity->m_location.m_pos = WFMath::Point<3>(10, 10, 0);
    fixedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*fixedEntity);

    OpVector res;

    //First tick should not update anything
    domain->tick(0, res);
    ASSERT_EQUAL(freeEntity->m_location.m_pos, WFMath::Point<3>::ZERO());
    ASSERT_EQUAL(fixedEntity->m_location.m_pos, WFMath::Point<3>(10, 10, 0));

    //Inject enough ticks to move rock to bottom
    while (time < 5) {
        time += tickSize;
        domain->tick(tickSize, res);
    }
    ASSERT_EQUAL(freeEntity->m_location.m_pos.z(), -64);
    //Fixed entity should not move
    ASSERT_EQUAL(fixedEntity->m_location.m_pos, WFMath::Point<3>(10, 10, 0));
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
    freeEntity->m_location.m_pos = WFMath::Point<3>(0, 0, 1);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    Entity* fixedEntity = new Entity("2", newId());
    fixedEntity->setProperty("mass", massProp);

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("fixed");
    fixedEntity->setProperty(ModeProperty::property_name, modeProperty);
    fixedEntity->setType(rockType);
    fixedEntity->m_location.m_pos = WFMath::Point<3>::ZERO();
    fixedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*fixedEntity);

    OpVector res;

    //Inject enough ticks to move rock to bottom
    while (time < 5) {
        time += tickSize;
        domain->tick(tickSize, res);
    }
    ASSERT_EQUAL(freeEntity->m_location.m_pos, WFMath::Point<3>(0, 0, 1));
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
    freeEntity->m_location.m_pos = WFMath::Point<3>(10, 10, 20);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity);

    Entity* plantedEntity = new Entity("2", newId());
    plantedEntity->setProperty("mass", massProp);

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("planted");

    plantedEntity->setProperty(ModeProperty::property_name, modeProperty);
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(20, 20, 20);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*plantedEntity);

    ASSERT_EQUAL(freeEntity->m_location.m_pos.z(), 20);
    //Planted entity should be placed on the terrain when added to the domain.
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(20, 20, 10.0058));

    OpVector res;

    //Inject enough ticks to move rock to bottom
    while (time < 5) {
        time += tickSize;
        domain->tick(tickSize, res);
    }
    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.z(), 10.0087f, 0.01f);
    //Planted entity should not move
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(20, 20, 10.0058));
}

void PhysicalDomainIntegrationTest::test_collision()
{

    double tickSize = 1.0 / 15.0;

    Property<float>* zeroFrictionProperty = new Property<float>();
    zeroFrictionProperty->data() = 0;


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
    propelProperty->data() = WFMath::Vector<3>(0, 2, 0);

    AngularFactorProperty angularZeroFactorProperty;
    angularZeroFactorProperty.data() = WFMath::Vector<3>::ZERO();


    Entity* freeEntity = new Entity("1", newId());
    freeEntity->setProperty(PropelProperty::property_name, propelProperty);
    freeEntity->setProperty("mass", massProp);
    freeEntity->setProperty("friction", zeroFrictionProperty);
    freeEntity->setProperty(AngularFactorProperty::property_name, &angularZeroFactorProperty);
    freeEntity->setType(rockType);
    freeEntity->m_location.m_pos = WFMath::Point<3>(10, 10, 10);
    freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 1)));

    domain->addEntity(*freeEntity);

    Entity* plantedEntity = new Entity("2", newId());
    plantedEntity->setProperty("mass", massProp);

    ModeProperty* modeProperty = new ModeProperty();
    modeProperty->set("planted");

    plantedEntity->setProperty(ModeProperty::property_name, modeProperty);
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(10, 15, 10);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*plantedEntity);
    const WFMath::Point<3> plantedPos = plantedEntity->m_location.m_pos;

    OpVector res;

    domain->tick(tickSize, res);

    //Should have moved 2/15 meters
    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.y(), 10 + (2.0 / 15.0), 0.01f);

    //Inject ticks for one second
    for (int i = 0; i < 14; ++i) {
        domain->tick(tickSize, res);
    }

    //Should have moved 2 meters in y axis
    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.y(), 12, 0.1f);

    //Inject ticks for three seconds
    for (int i = 0; i < (3 * 15); ++i) {
        domain->tick(tickSize, res);
    }

    //Should have stopped at planted entity
    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.y(), 13, 0.01f);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, plantedPos);

    domain->removeEntity(*plantedEntity);
    for (int i = 0; i < 15; ++i) {
        domain->tick(tickSize, res);
    }

    //Should have moved two more meters as planted entity was removed.
    ASSERT_FUZZY_EQUAL(freeEntity->m_location.m_pos.y(), 15, 0.1f);

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
    freeEntity1->m_location.m_pos = WFMath::Point<3>(10, 10, 30);
    freeEntity1->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity1);

    ASSERT_EQUAL(freeEntity1->m_location.m_pos, WFMath::Point<3>(10, 10, 30));

    //The other free entity is placed below the terrain; it's expected to then be clamped to the terrain
    Entity* freeEntity2 = new Entity("free2", newId());
    freeEntity2->setProperty("mass", massProp);
    freeEntity2->setType(rockType);
    freeEntity2->m_location.m_pos = WFMath::Point<3>(20, 20, -10);
    freeEntity2->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*freeEntity2);
    ASSERT_EQUAL(freeEntity2->m_location.m_pos, WFMath::Point<3>(20, 20, 22.6006));

    Entity* plantedEntity = new Entity("planted", newId());
    plantedEntity->setProperty("mass", massProp);
    plantedEntity->setProperty(ModeProperty::property_name, modePlantedProperty);
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(30, 30, 10);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*plantedEntity);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 30, 18.4325));


    Entity* fixedEntity = new Entity("fixed", newId());
    fixedEntity->setProperty("mass", massProp);
    fixedEntity->setProperty(ModeProperty::property_name, modeFixedProperty);
    fixedEntity->setType(rockType);
    fixedEntity->m_location.m_pos = WFMath::Point<3>(40, 40, 50);
    fixedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 1)));
    domain->addEntity(*fixedEntity);
    ASSERT_EQUAL(fixedEntity->m_location.m_pos, WFMath::Point<3>(40, 40, 50));


    OpVector res;

    //Inject ticks for two seconds
    for (int i = 0; i < 30; ++i) {
        domain->tick(tickSize, res);
    }

    ASSERT_NOT_EQUAL(freeEntity1->m_location.m_pos, WFMath::Point<3>(10, 10, 30));
    ASSERT_NOT_EQUAL(freeEntity2->m_location.m_pos, WFMath::Point<3>(20, 20, 22.6006));
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 30, 18.4325));
    ASSERT_EQUAL(fixedEntity->m_location.m_pos, WFMath::Point<3>(40, 40, 50));
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
            freeEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.25f, -0.25f, 0), WFMath::Point<3>(-0.25f, -0.25f, 0.5f)));
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
    ASSERT_EQUAL(entities[0]->m_location.m_pos, WFMath::Point<3>(0.495543, -0, 19.949));
    ASSERT_EQUAL(entities[10]->m_location.m_pos, WFMath::Point<3>(1, -0, 18.0217));
    ASSERT_EQUAL(entities[15]->m_location.m_pos, WFMath::Point<3>(2.61429, 0.0884429, 26.4489));
    ASSERT_EQUAL(entities[16]->m_location.m_pos, WFMath::Point<3>(0.948305, 0.105805, 18.7352));
    ASSERT_EQUAL(entities[55]->m_location.m_pos, WFMath::Point<3>(6.30361, -1.19749f, 28.0569));

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
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(0, 0, -64), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    TestWorld testWorld(*rootEntity);


    Entity* plantedEntity = new Entity("planted", newId());
    plantedEntity->setProperty(ModeProperty::property_name, modePlantedProperty);
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(30, 30, 10);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 10)));
    plantedEntity->setProperty("planted-offset", plantedOffset);
    domain->addEntity(*plantedEntity);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 30, 8.01695));

    plantedOffset->data() = -3;
    plantedOffset->apply(plantedEntity);
    plantedEntity->propertyApplied.emit("planted-offset", *plantedOffset);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 30, 7.01695));

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
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(0, 0, -64), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    TestWorld testWorld(*rootEntity);


    Entity* plantedEntity = new Entity("planted", newId());
    plantedEntity->setProperty(ModeProperty::property_name, modePlantedProperty);
    plantedEntity->setType(rockType);
    plantedEntity->m_location.m_pos = WFMath::Point<3>(30, 30, 10);
    plantedEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-1, -1, 0), WFMath::Point<3>(1, 1, 10)));
    plantedEntity->setProperty("planted-scaled-offset", plantedScaledOffset);
    domain->addEntity(*plantedEntity);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 30, 8.01695));

    plantedScaledOffset->data() = -0.3;
    plantedScaledOffset->apply(plantedEntity);
    plantedEntity->propertyApplied.emit("planted-offset", *plantedScaledOffset);
    ASSERT_EQUAL(plantedEntity->m_location.m_pos, WFMath::Point<3>(30, 30, 7.01695));

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
    rootEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-64, -64, 0), WFMath::Point<3>(64, 64, 64)));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    TestWorld testWorld(*rootEntity);

    Entity* smallEntity1 = new Entity("small1", newId());
    smallEntity1->setProperty(ModeProperty::property_name, modePlantedProperty);
    smallEntity1->setType(rockType);
    smallEntity1->m_location.m_pos = WFMath::Point<3>(30, 30, 0);
    smallEntity1->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, -0.2f, 0), WFMath::Point<3>(0.2, 0.2, 0.4)));
    domain->addEntity(*smallEntity1);

    Entity* smallEntity2 = new Entity("small2", newId());
    smallEntity2->setProperty(ModeProperty::property_name, modePlantedProperty);
    smallEntity2->setType(rockType);
    smallEntity2->m_location.m_pos = WFMath::Point<3>(-31, -31, 0);
    smallEntity2->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, -0.2f, 0), WFMath::Point<3>(0.2, 0.2, 0.4)));
    domain->addEntity(*smallEntity2);

    //This entity should always be seen, as "visibility" is specified.
    Entity* smallVisibleEntity = new Entity("smallVisible", newId());
    smallVisibleEntity->setProperty(ModeProperty::property_name, modePlantedProperty);
    smallVisibleEntity->setType(rockType);
    smallVisibleEntity->m_location.m_pos = WFMath::Point<3>(-63, -63, 0);
    smallVisibleEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, -0.2f, 0), WFMath::Point<3>(0.2, 0.2, 0.4)));
    smallVisibleEntity->setProperty("visibility", visibilityProperty);
    domain->addEntity(*smallVisibleEntity);

    Entity* largeEntity1 = new Entity("large1", newId());
    largeEntity1->setProperty(ModeProperty::property_name, modePlantedProperty);
    largeEntity1->setType(rockType);
    largeEntity1->m_location.m_pos = WFMath::Point<3>(0, 0, 0);
    largeEntity1->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-10.f, -10.f, 0), WFMath::Point<3>(10, 10, 20)));
    domain->addEntity(*largeEntity1);

    Entity* observerEntity = new Entity("observer", newId());
    observerEntity->setType(humanType);
    observerEntity->m_location.m_pos = WFMath::Point<3>(-30, -30, 0);
    observerEntity->m_location.setBBox(WFMath::AxisBox<3>(WFMath::Point<3>(-0.2f, -0.2f, 0), WFMath::Point<3>(0.2, 0.2, 2)));
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
    domain->applyTransform(*observerEntity, WFMath::Quaternion(), WFMath::Point<3>(30, 30, 0), WFMath::Vector<3>());
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

