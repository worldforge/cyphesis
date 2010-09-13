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

#include "PropertyCoverage.h"

#include "rulesets/Entity.h"
#include "rulesets/TerrainModProperty.h"
#include "rulesets/TerrainProperty.h"

using Atlas::Message::ListType;
using Atlas::Message::MapType;

int main()
{
    HandlerMap terrainModHandlers;

    TerrainModProperty * ap = new TerrainModProperty(terrainModHandlers);

    PropertyCoverage pc(ap);

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

    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}

// stubs

#include "rulesets/TerrainMod.h"

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

void Entity::installHandler(int class_no, Handler handler)
{
}

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return m_properties[name] = prop;
}

InnerTerrainMod::InnerTerrainMod(const std::string& typemod)
{
}

InnerTerrainMod::~InnerTerrainMod()
{
}

InnerTerrainModCrater::InnerTerrainModCrater()
: InnerTerrainMod("cratermod")
, mModifier(0)
{
}

InnerTerrainModCrater::~InnerTerrainModCrater()
{
}

Mercator::TerrainMod* InnerTerrainModCrater::getModifier()
{
    return 0;
}


bool InnerTerrainModCrater::parseAtlasData(Entity * owner, const Atlas::Message::MapType& modElement)
{
    return true;
}

InnerTerrainModSlope::InnerTerrainModSlope()
: InnerTerrainMod("slopemod")
, mModifier_impl(0)
{
}

InnerTerrainModSlope::~InnerTerrainModSlope()
{
}

Mercator::TerrainMod* InnerTerrainModSlope::getModifier()
{
    return 0;
}


bool InnerTerrainModSlope::parseAtlasData(Entity * owner, const Atlas::Message::MapType& modElement)
{
    return true;
}

InnerTerrainModLevel::InnerTerrainModLevel()
: InnerTerrainMod("levelmod")
, mModifier_impl(0)
{
}

InnerTerrainModLevel::~InnerTerrainModLevel()
{
}

Mercator::TerrainMod* InnerTerrainModLevel::getModifier()
{
    return 0;
}

bool InnerTerrainModLevel::parseAtlasData(Entity * owner, const Atlas::Message::MapType& modElement)
{
    return true;
}

InnerTerrainModAdjust::InnerTerrainModAdjust()
: InnerTerrainMod("adjustmod")
, mModifier_impl(0)
{
}

InnerTerrainModAdjust::~InnerTerrainModAdjust()
{
}

Mercator::TerrainMod* InnerTerrainModAdjust::getModifier()
{
    return 0;
}


bool InnerTerrainModAdjust::parseAtlasData(Entity * owner, const Atlas::Message::MapType& modElement)
{
    return true;
}
