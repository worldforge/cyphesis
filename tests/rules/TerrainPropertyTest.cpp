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

#include "../stubs/common/stubcustom.h"
#include "../stubs/common/stubTypeNode.h"

#include "rules/simulation/TerrainProperty.h"

#include <Mercator/Terrain.h>

using Atlas::Message::ListType;
using Atlas::Message::MapType;

int main()
{
    TerrainProperty ap;

    PropertyChecker<TerrainProperty> pc(ap);

    MapType terrain;
    terrain["points"] = 1;
    pc.testDataAppend(terrain);

    terrain.clear();
    MapType points;
    points["0x0"] = ListType(3, 0);
    terrain["points"] = points;
    pc.testDataAppend(terrain);

    terrain.clear();
    points.clear();
    points["0x0"] = ListType(2, 0);
    terrain["points"] = points;
    pc.testDataAppend(terrain);

    terrain.clear();
    points.clear();
    points["0x0"] = 1;
    terrain["points"] = points;
    pc.testDataAppend(terrain);

    terrain.clear();
    terrain["surfaces"] = 1;
    pc.testDataAppend(terrain);

    terrain.clear();
    MapType surface;
    surface["name"] = "mud";
    surface["pattern"] = "fill";
    terrain["surfaces"] = ListType(1, surface);
    pc.testDataAppend(terrain);

    pc.basicCoverage();

    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}

// stubs

#include "../TestWorld.h"



#include "modules/TerrainContext.h"

#include "../stubs/modules/stubWeakEntityRef.h"

