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

#include "PropertyCoverage.h"

#include "rulesets/Entity.h"
#include "rulesets/TerrainModProperty.h"
#include "rulesets/TerrainProperty.h"

using Atlas::Message::ListType;
using Atlas::Message::MapType;

static int run_coverage()
{
    HandlerMap terrainModHandlers;

    TerrainModProperty * ap = new TerrainModProperty(terrainModHandlers);

    PropertyChecker<TerrainModProperty> pc(ap);

    MapType shape;
    MapType mod;
    mod["type"] = 1;
    pc.testDataAppend(mod);

    mod.clear();
    shape.clear();
    mod["type"] = "unknown";
    pc.testDataAppend(mod);

    mod.clear();
    shape.clear();
    mod["type"] = "slopemod";
    mod["slopes"] = 1;
    pc.testDataAppend(mod);
    mod["slopes"] = ListType();
    pc.testDataAppend(mod);

    mod.clear();
    shape.clear();
    mod["type"] = "slopemod";
    mod["slopes"] = ListType(2, 1.f);
    shape["type"] = "ball";
    mod["shape"] = shape;
    pc.testDataAppend(mod);

    shape["type"] = "ball";
    mod["shape"] = shape;
    pc.testDataAppend(mod);

    shape["type"] = "rotbox";
    mod["shape"] = shape;
    pc.testDataAppend(mod);

    shape["type"] = "polygon";
    mod["shape"] = shape;
    pc.testDataAppend(mod);

    mod.clear();
    shape.clear();
    mod["type"] = "levelmod";
    pc.testDataAppend(mod);

    mod.clear();
    shape.clear();
    mod["type"] = "adjustmod";
    pc.testDataAppend(mod);

    mod.clear();
    shape.clear();
    mod["type"] = "cratermod";
    shape["type"] = "ball";
    mod["shape"] = shape;
    pc.testDataAppend(mod);

    pc.basicCoverage();

    pc.tlve()->setProperty("terrain", new TerrainProperty);

    pc.basicCoverage();

    return 0;
}

static TerrainProperty * stub_getTerrain_return = 0;

int main()
{
    {
        HandlerMap terrainModHandlers;

        TerrainModProperty * ap = new TerrainModProperty(terrainModHandlers);

        MapType shape;
        MapType mod;
        
        shape["type"] = "polygon";
        shape["points"] = ListType(3, ListType(2, 1.1f));
        mod["shape"] = shape;
        mod["type"] = "levelmod";

        ap->set(mod);

        // FIXME verify that the mod really takes effect
        // ap->apply(0);
    }

    return run_coverage();
}

// stubs

#include "TestWorld.h"

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}

#include "rulesets/TerrainModTranslator.h"

#include "modules/TerrainContext.h"

#include <Mercator/TerrainMod.h>

const TerrainProperty * TerrainEffectorProperty::getTerrain(LocatedEntity * owner)
{
    return stub_getTerrain_return;
}

TerrainModTranslator::TerrainModTranslator()
{
}

bool TerrainModTranslator::parseData(const WFMath::Point<3> & pos,
                                     const WFMath::Quaternion & orientation,
                                     const MapType& modElement)
{
    WFMath::Polygon<2> p;
    p.addCorner(0, WFMath::Point<2>(0., 0.));
    m_mod = new Mercator::LevelTerrainMod<WFMath::Polygon>(1.f, p);
    return true;
}

Mercator::TerrainMod* TerrainModTranslator::getModifier()
{
    return m_mod;
}

TerrainContext::TerrainContext(LocatedEntity * e) : m_entity(e)
{
}

TerrainContext::~TerrainContext()
{
}

EntityRef::EntityRef(LocatedEntity* e) : m_inner(e)
{
}
