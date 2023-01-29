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

#include "../TestBase.h"
#include "../TestWorld.h"

#include "server/Ruleset.h"
#include "server/ServerRouting.h"

#include "rules/simulation/Entity.h"

#include "common/compose.hpp"
#include "common/debug.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>
#include <rules/simulation/PhysicalDomain.h>
#include <common/TypeNode.h>
#include <rules/simulation/ModeProperty.h>
#include <rules/simulation/TerrainProperty.h>
#include <Mercator/BasePoint.h>
#include <Mercator/Terrain.h>
#include <rules/simulation/PropelProperty.h>
#include <rules/simulation/AngularFactorProperty.h>
#include <chrono>
#include <rules/simulation/VisibilityProperty.h>
#include <rules/BBoxProperty.h>
#include <rules/SolidProperty.h>

#include "../stubs/common/stublog.h"
#include "common/Monitors.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using String::compose;


class PhysicalDomainBenchmark : public Cyphesis::TestBase
{
    protected:
        static long m_id_counter;

    public:
        PhysicalDomainBenchmark();

        static long newId();

        void setup();

        void teardown();

        void test_static_entities_no_move();

        void test_determinism();

        void test_visibilityPerformance();
};

long PhysicalDomainBenchmark::m_id_counter = 0L;

PhysicalDomainBenchmark::PhysicalDomainBenchmark()
{
    ADD_TEST(PhysicalDomainBenchmark::test_static_entities_no_move);
    ADD_TEST(PhysicalDomainBenchmark::test_determinism);
    ADD_TEST(PhysicalDomainBenchmark::test_visibilityPerformance);

}

long PhysicalDomainBenchmark::newId()
{
    return ++m_id_counter;
}

void PhysicalDomainBenchmark::setup()
{
    m_id_counter = 0;
}

void PhysicalDomainBenchmark::teardown()
{

}
void PhysicalDomainBenchmark::test_static_entities_no_move()
{

    double tickSize = 1.0 / 15.0;

    TypeNode* rockType = new TypeNode("rock");
    ModeProperty* modePlantedProperty = new ModeProperty();
    modePlantedProperty->set("planted");

    Entity* rootEntity = new Entity(newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    rootEntity->setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
    Mercator::Terrain& terrain = terrainProperty->getData(*rootEntity);
    terrain.setBasePoint(0, 0, Mercator::BasePoint(40));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(40));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
    rootEntity->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
    rootEntity->requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(0, -64, 0), WFMath::Point<3>(64, 64, 64));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    Property<double>* massProp = new Property<double>();
    massProp->data() = 100;

    std::vector<Entity*> entities;

    for (size_t i = 0; i < 60; ++i) {
        for (size_t j = 0; j < 60; ++j) {
            long id = newId();
            std::stringstream ss;
            ss << "planted" << id;
            Entity* entity = new Entity(id);
            entity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
            entity->setType(rockType);
            entity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty));
            entity->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(i, j, i + j);
            entity->requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-0.25f, 0, -0.25f), WFMath::Point<3>(-0.25f, 0.5f, -0.25f));
            domain->addEntity(*entity);
            entities.push_back(entity);
        }
    }

    OpVector res;

    //First tick is setup, so we'll exclude that from time measurement
    domain->tick(tickSize, res);
    auto start = std::chrono::high_resolution_clock::now();
    //Inject ticks for two seconds
    for (int i = 0; i < 30; ++i) {
        domain->tick(tickSize, res);
    }

    std::stringstream ss;
    long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    ss << "Average tick duration: " << milliseconds / 30.0 << " ms";
    log(INFO, ss.str());
    ss = std::stringstream();
    ss << "Physics per second: " << (milliseconds / 2.0) / 10.0 << " %";
    log(INFO, ss.str());

}

void PhysicalDomainBenchmark::test_determinism()
{

    double tickSize = 1.0 / 15.0;

    TypeNode* rockType = new TypeNode("rock");

    Entity* rootEntity = new Entity(newId());
    TerrainProperty* terrainProperty = new TerrainProperty();
    rootEntity->setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
    Mercator::Terrain& terrain = terrainProperty->getData(*rootEntity);
    terrain.setBasePoint(0, 0, Mercator::BasePoint(40));
    terrain.setBasePoint(0, 1, Mercator::BasePoint(40));
    terrain.setBasePoint(1, 0, Mercator::BasePoint(10));
    terrain.setBasePoint(1, 1, Mercator::BasePoint(10));
    rootEntity->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
    rootEntity->requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(0, -64, 0), WFMath::Point<3>(64, 64, 64));
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    Property<double>* massProp = new Property<double>();
    massProp->data() = 100;

    std::vector<Entity*> entities;

    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            long id = newId();
            std::stringstream ss;
            ss << "free" << id;
            Entity* freeEntity = new Entity(id);
            freeEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
            freeEntity->setType(rockType);
            freeEntity->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(i, j, i + j);
            freeEntity->requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-0.25f, 0, -0.25f), WFMath::Point<3>(-0.25f, 0.5f, -0.25f));
            domain->addEntity(*freeEntity);
            entities.push_back(freeEntity);
        }
    }

    OpVector res;

    //First tick is setup, so we'll exclude that from time measurement
    domain->tick(tickSize, res);
    auto start = std::chrono::high_resolution_clock::now();
    //Inject ticks for two seconds
    for (int i = 0; i < 30; ++i) {
        domain->tick(tickSize, res);
    }
    std::stringstream ss;
    long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    ss << "Average tick duration: " << milliseconds / 30.0 << " ms";
    log(INFO, ss.str());
    ss = std::stringstream();
    ss << "Physics per second: " << (milliseconds / 2.0) / 10.0 << " %";
    log(INFO, ss.str());
}

void PhysicalDomainBenchmark::test_visibilityPerformance()
{

    double tickSize = 1.0 / 15.0;

    TypeNode* rockType = new TypeNode("rock");
    TypeNode* humanType = new TypeNode("human");

    PropelProperty* propelProperty = new PropelProperty();
    ////Move diagonally up
    propelProperty->data() = WFMath::Vector<3>(5, 0, 5);

    Property<double>* massProp = new Property<double>();
    massProp->data() = 10000;

    Ref<Entity> rootEntity = new Entity(newId());
    rootEntity->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
    WFMath::AxisBox<3> aabb(WFMath::Point<3>(-512, 0, -512), WFMath::Point<3>(512, 64, 512));
    rootEntity->requirePropertyClassFixed<BBoxProperty>().data() = aabb;
    PhysicalDomain* domain = new PhysicalDomain(*rootEntity);

    TestWorld testWorld(rootEntity);

    ModeProperty* modePlantedProperty = new ModeProperty();
    modePlantedProperty->set("planted");

    std::vector<Entity*> entities;

    int counter = 0;

    auto size = aabb.highCorner() - aabb.lowCorner();

    for (float i = aabb.lowCorner().x(); i <= aabb.highCorner().x(); i = i + (size.x() / 100.0f)) {
        for (float j = aabb.lowCorner().z(); j <= aabb.highCorner().z(); j = j + (size.z() / 100.0f)) {
            counter++;
            long id = newId();
            std::stringstream ss;
            ss << "planted" << id;
            Entity* plantedEntity = new Entity(id);
            plantedEntity->setProperty(ModeProperty::property_name, std::unique_ptr<PropertyBase>(modePlantedProperty));
            plantedEntity->setType(rockType);
            plantedEntity->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(i, 0, j);
            plantedEntity->requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-0.25f, 0, -0.25f), WFMath::Point<3>(-0.25f, .2f, -0.25f));
            domain->addEntity(*plantedEntity);
            entities.push_back(plantedEntity);
        }
    }

    {
        std::stringstream ss;
        ss << "Added " << counter << " planted entities at " << (size.x() / 100.0) << " meter interval.";
        log(INFO, ss.str());
    }

    int numberOfObservers = 200;

    std::vector<Entity*> observers;
    for (int i = 0; i < numberOfObservers; ++i) {
        long id = newId();
        std::stringstream ss;
        ss << "observer" << id;
        Entity* observerEntity = new Entity(id);
        observers.push_back(observerEntity);
        observerEntity->requirePropertyClassFixed<SolidProperty>().set(0);
        observerEntity->setType(humanType);
        observerEntity->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>(aabb.lowCorner().x() + (i * 4), 0, aabb.lowCorner().z());
        observerEntity->requirePropertyClassFixed<BBoxProperty>().data() = WFMath::AxisBox<3>(WFMath::Point<3>(-0.1f, 0, -0.1f), WFMath::Point<3>(0.1, 2, 0.1));
        observerEntity->setProperty(PropelProperty::property_name, std::unique_ptr<PropertyBase>(propelProperty));
        observerEntity->addFlags(entity_perceptive);
        observerEntity->setProperty("mass", std::unique_ptr<PropertyBase>(massProp));
        domain->addEntity(*observerEntity);
    }


    OpVector res;

    //First tick is setup, so we'll exclude that from time measurement
    domain->tick(2, res);
    {
        auto start = std::chrono::high_resolution_clock::now();
        //Inject ticks for 20 seconds
        for (int i = 0; i < 15 * 20; ++i) {
            domain->tick(tickSize, res);
        }
        std::stringstream ss;
        long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        ss << "Average tick duration with " << numberOfObservers << " moving observers: " << milliseconds / (15. * 20.0) << " ms";
        log(INFO, ss.str());
        ss = std::stringstream();
        ss << "Physics per second with " << numberOfObservers << " moving observers: " << (milliseconds / 20.0) / 10.0 << " %";
        log(INFO, ss.str());
    }
    std::set<LocatedEntity*> transformedEntities;
    //Now stop the observers from moving, and measure again
    for (Entity* observer : observers) {
        Domain::TransformData transformData{WFMath::Quaternion(), WFMath::Point<3>(),nullptr,WFMath::Vector<3>::ZERO() };
        domain->applyTransform(*observer, transformData, transformedEntities);
    }
    domain->tick(10, res);
    {
        auto start = std::chrono::high_resolution_clock::now();
        //Inject ticks for 1 seconds
        for (int i = 0; i < 15; ++i) {
            domain->tick(tickSize, res);
        }
        std::stringstream ss;
        long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        ss << "Average tick duration without moving observer: " << milliseconds / 15. << " ms";
        log(INFO, ss.str());
        ss = std::stringstream();
        ss << "Physics per second without moving observer: " << (milliseconds / 1.0) / 10.0 << " %";
        log(INFO, ss.str());
    }
}



int main()
{
    Monitors m;
    PhysicalDomainBenchmark t;

    return t.run();
}

