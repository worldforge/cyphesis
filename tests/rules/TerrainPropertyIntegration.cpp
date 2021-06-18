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


#include "../TestBaseWithContext.h"
#include "../DatabaseNull.h"
#include "../TestWorld.h"
#include "common/Monitors.h"
#include "common/Inheritance.h"
#include "common/operations/Thought.h"
#include "rules/simulation/World.h"
#include "../NullEntityCreator.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <wfmath/atlasconv.h>
#include <rules/simulation/CorePropertyManager.h>

#include <memory>
#include <rules/simulation/ModifiersProperty.h>
#include <rules/simulation/WorldRouter.h>
#include <rules/simulation/MindsProperty.h>
#include <rules/simulation/TerrainProperty.h>
#include <rules/simulation/TerrainPointsProperty.h>
#include <rules/simulation/PhysicalDomain.h>
#include "rules/BBoxProperty.h"

using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

struct TestContext
{

    Atlas::Objects::Factories factories;
    DatabaseNull database;
    Ref<World> world;
    Inheritance inheritance;
    WorldRouter testWorld;
    NullEntityCreator entityCreator;
    CorePropertyManager propertyManager;

    TestContext() :
            world(new World()),
            inheritance(factories),
            testWorld(world, entityCreator, [] { return std::chrono::steady_clock::now().time_since_epoch(); }),
            propertyManager(inheritance)
    {
    }

};


double epsilon = 0.01;

struct Tested : public Cyphesis::TestBaseWithContext<TestContext>
{
    Tested()
    {
        ADD_TEST(test_adjust_entities_when_terrain_changes);
        ADD_TEST(test_set_terrain);
    }


    void test_set_terrain(TestContext& context)
    {
        Ref<Thing> t1 = new Thing(1);
        t1->requirePropertyClassFixed<BBoxProperty>().data() = {{-64, -10, -64},
                                                                {64,  10,  64}};
        t1->setAttrValue("domain", "physical");
        t1->setAttrValue(TerrainProperty::property_name, ListType{MapType{
                {"name",    "rock"},
                {"pattern", "fill"}}});

        t1->setAttrValue(TerrainPointsProperty::property_name, MapType{
                {"-1x-1", ListType{-1.0, -1.0, 10.0}},
                {"0x-1",  ListType{0.0, -1.0, 10.0}},
                {"1x-1",  ListType{1.0, -1.0, 10.0}},
                {"-1x0",  ListType{-1.0, 0.0, 10.0}},
                {"0x0",   ListType{0.0, 0.0, 10.0}},
                {"1x0",   ListType{1.0, 0.0, 10.0}},
                {"-1x1",  ListType{-1.0, 1.0, 10.0}},
                {"0x1",   ListType{0.0, 1.0, 10.0}},
                {"1x1",   ListType{1.0, 1.0, 10.0}},
        });
        context.testWorld.addEntity(t1, context.world);

        auto terrainProp = t1->getPropertyClassFixed<TerrainProperty>();
        ASSERT_NOT_NULL(terrainProp);

        float height;
        terrainProp->getHeight(*t1, 10, 10, height);
        ASSERT_FUZZY_EQUAL(10.0, height, epsilon)

        t1->setAttrValue("terrain_points!prepend", MapType{
                {"0x0", ListType{0.0, 0.0, 20.0}}
        });

        terrainProp->getHeight(*t1, 10, 10, height);
        ASSERT_FUZZY_EQUAL(14.6848, height, epsilon)

        //Update roughness
        t1->setAttrValue("terrain_points!prepend", MapType{
                {"0x0", ListType{0.0, 0.0, 20.0, 0.75}}
        });

        terrainProp->getHeight(*t1, 10, 10, height);
        ASSERT_FUZZY_EQUAL(15.2658, height, epsilon)

        //Set roughness to zero
        t1->setAttrValue("terrain_points!prepend", MapType{
                {"0x0", ListType{0.0, 0.0, 20.0, 0.0}}
        });

        terrainProp->getHeight(*t1, 10, 10, height);
        ASSERT_FUZZY_EQUAL(16.12, height, epsilon)

        //Handle ints
        t1->setAttrValue("terrain_points!prepend", MapType{
                {"0x0", ListType{0, 0, 20, 0}}
        });

        terrainProp->getHeight(*t1, 10, 10, height);
        ASSERT_FUZZY_EQUAL(16.12, height, epsilon)

        t1->destroy();
    }

    void test_adjust_entities_when_terrain_changes(TestContext& context)
    {
        Ref<Thing> t1 = new Thing(1);
        t1->requirePropertyClassFixed<BBoxProperty>().data() = {{-64, -10, -64},
                                                                {64,  10,  64}};
        t1->setAttrValue("domain", "physical");
        t1->setAttrValue(TerrainProperty::property_name, ListType{MapType{
                {"name",    "rock"},
                {"pattern", "fill"}}});

        t1->setAttrValue(TerrainPointsProperty::property_name, MapType{
                {"-1x-1", ListType{-1.0, -1.0, 10.0}},
                {"0x-1",  ListType{0.0, -1.0, 10.0}},
                {"1x-1",  ListType{1.0, -1.0, 10.0}},
                {"-1x0",  ListType{-1.0, 0.0, 10.0}},
                {"0x0",   ListType{0.0, 0.0, 10.0}},
                {"1x0",   ListType{1.0, 0.0, 10.0}},
                {"-1x1",  ListType{-1.0, 1.0, 10.0}},
                {"0x1",   ListType{0.0, 1.0, 10.0}},
                {"1x1",   ListType{1.0, 1.0, 10.0}},
        });
        context.testWorld.addEntity(t1, context.world);

        auto terrainProp = t1->getPropertyClassFixed<TerrainProperty>();

        Ref<Thing> tPlanted = new Thing(2);
        tPlanted->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                                                      {1,  2, 1}};
        tPlanted->requirePropertyClassFixed<PositionProperty>().data() = {10, 100, 10};
        tPlanted->setAttrValue("mode", "planted");
        context.testWorld.addEntity(tPlanted, t1);


        Ref<Thing> tFixed = new Thing(3);
        tFixed->requirePropertyClassFixed<BBoxProperty>().data() = {{-1, 0, -1},
                                                                    {1,  2, 1}};
        tFixed->requirePropertyClassFixed<PositionProperty>().data() = {20, 100, 10};
        tFixed->setAttrValue("mode", "fixed");
        context.testWorld.addEntity(tFixed, t1);

        //Height should not be adjusted to the height of the terrain.
        ASSERT_FUZZY_EQUAL(100.0, tFixed->requirePropertyClassFixed<PositionProperty>().data().y(), epsilon)

        //Adjust the terrain
        t1->setAttrValue("terrain_points!prepend", MapType{
                {"1x1", ListType{1.0, 1.0, 20.0}}
        });

        //First process the Tick op for the physical domain
        context.testWorld.getOperationsHandler().dispatchNextOp();
        //Then process the Move op generated.
        context.testWorld.getOperationsHandler().dispatchNextOp();
        //Process an extra op to catch any incorrect Move ops that would be generated for the fixed entity.
        context.testWorld.getOperationsHandler().dispatchNextOp();

        float height;
        terrainProp->getHeight(*t1, tPlanted->requirePropertyClassFixed<PositionProperty>().data().x(), tPlanted->requirePropertyClassFixed<PositionProperty>().data().z(), height);
        ASSERT_FUZZY_EQUAL(height, tPlanted->requirePropertyClassFixed<PositionProperty>().data().y(), epsilon)
        ASSERT_FUZZY_EQUAL(100.0, tFixed->requirePropertyClassFixed<PositionProperty>().data().y(), epsilon)


        tFixed->destroy();
        tPlanted->destroy();
        t1->destroy();

    }

};


int main()
{
    Monitors m;
    Tested t;

    return t.run();
}

