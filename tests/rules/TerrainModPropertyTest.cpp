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

#include "../PropertyCoverage.h"

#include "common/TypeNode.h"
#include "rules/simulation/Entity.h"
#include "rules/simulation/TerrainModProperty.h"
#include "rules/simulation/TerrainProperty.h"


#include "../stubs/common/stubTypeNode.h"
#include "../stubs/common/stubcustom.h"

using Atlas::Message::ListType;
using Atlas::Message::MapType;

static int run_coverage()
{
    TerrainModProperty ap;

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

    pc.tlve()->setProperty("terrain", std::unique_ptr<PropertyBase>(new TerrainProperty));

    pc.basicCoverage();

    return 0;
}

static TerrainProperty * stub_getTerrain_return = 0;

int main()
{
    {
        TerrainModProperty ap;

        MapType shape;
        MapType mod;
        
        shape["type"] = "polygon";
        shape["points"] = ListType(3, ListType(2, 1.1f));
        mod["shape"] = shape;
        mod["type"] = "levelmod";

        ap.set(mod);

        // FIXME verify that the mod really takes effect
        // ap->apply(0);
    }

    return run_coverage();
}

// stubs

#include "../TestWorld.h"
#include "rules/simulation/TerrainModTranslator.h"

#include "modules/TerrainContext.h"

#include <Mercator/TerrainMod.h>

const TerrainProperty * TerrainEffectorProperty::getTerrain(LocatedEntity & owner, LocatedEntity**)
{
    return stub_getTerrain_return;
}

TerrainModTranslator::TerrainModTranslator(const Atlas::Message::MapType& data)
{
}

std::unique_ptr<Mercator::TerrainMod> TerrainModTranslator::parseData(const WFMath::Point<3> & pos,
                                     const WFMath::Quaternion & orientation)
{
    WFMath::Polygon<2> p;
    p.addCorner(0, WFMath::Point<2>(0., 0.));
    return std::make_unique<Mercator::LevelTerrainMod<WFMath::Polygon>>(1.f, p);
}

#include "../stubs/modules/stubWeakEntityRef.h"
