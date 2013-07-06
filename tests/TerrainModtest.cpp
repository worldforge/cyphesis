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

#include "rulesets/TerrainModTranslator.h"

#include <wfmath/quaternion.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

class TestTerrainModTranslator : public TerrainModTranslator
{
  public:
    TestTerrainModTranslator() : TerrainModTranslator() { }

    static float test_parsePosition(const WFMath::Point<3> & pos,
                                    const MapType& modElement)
    {
        return parsePosition(pos, modElement);
    }
};

static int test_reparse()
{
    // Call parseData with polygon shape and valid points
    {
        TerrainModTranslator * titm = new TerrainModTranslator;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;
        bool ret;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        mod["type"] = "levelmod";
        ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        Mercator::TerrainMod * tm1 = titm->getModifier();
        assert(tm1 != 0);

        // Re-parse the same data. Should update the mod in place.
        ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        Mercator::TerrainMod * tm2 = titm->getModifier();
        assert(tm2 != 0);
        assert(tm2 == tm1);

        // Change it to 2D ball shape. This requires a new mod.
        shape_desc["type"] = "ball";
        shape_desc["radius"] = 1.f;
        shape_desc["position"] = ListType(2, 1.);
        mod["shape"] = shape_desc;
        mod["type"] = "levelmod";
        ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        Mercator::TerrainMod * tm3 = titm->getModifier();
        assert(tm3 != 0);
        assert(tm3 != tm1);

        // Change it to an adjustmod. This requires a new mod
        mod["type"] = "adjustmod";
        ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        Mercator::TerrainMod * tm4 = titm->getModifier();
        assert(tm4 != 0);
        assert(tm4 != tm1);

        delete titm;
    }

    return 0;
}

int main()
{
    {
        TerrainModTranslator * titm = new TestTerrainModTranslator;
        delete titm;
    }

    // Call parsePosition with empty height data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        float z = TestTerrainModTranslator::test_parsePosition(pos, data);
        assert(z < 0);
    }

    // Call parsePosition with int height data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        data["height"] = 1;
        float z = TestTerrainModTranslator::test_parsePosition(pos, data);
        assert(z > 0);
    }

    // Call parsePosition with float height data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        data["height"] = 1.;
        float z = TestTerrainModTranslator::test_parsePosition(pos, data);
        assert(z > 0);
    }

    // Call parsePosition with bad (string) height data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        data["height"] = "1.";
        float z = TestTerrainModTranslator::test_parsePosition(pos, data);
        assert(z < 0);
    }

    // Call parsePosition with int heightoffset data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        data["heightoffset"] = 2;
        float z = TestTerrainModTranslator::test_parsePosition(pos, data);
        assert(z > 0);
    }

    // Call parsePosition with float heightoffset data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        data["heightoffset"] = 2.;
        float z = TestTerrainModTranslator::test_parsePosition(pos, data);
        assert(z > 0);
    }

    // Call parsePosition with bad (string) heightoffset data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        data["heightoffset"] = "1.";
        float z = TestTerrainModTranslator::test_parsePosition(pos, data);
        assert(z < 0);
    }

    ////////////////////// Concrete classes ///////////////////////////

    {
        TerrainModTranslator * titm = new TerrainModTranslator;
        delete titm;
    }

    {
        TerrainModTranslator * titm = new TerrainModTranslator;
        assert(titm->getModifier() == 0);
        delete titm;
    }

    // Call parseData with empty map
    {
        TerrainModTranslator * titm = new TerrainModTranslator;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType data;
        bool ret = titm->parseData(pos, orientation, data);
        assert(!ret);

        delete titm;
    }

    // Call parseData with unknown shape
    {
        TerrainModTranslator * titm = new TerrainModTranslator;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "unknown_shape";
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);


        delete titm;
    }

    // Call parseData with ball shape
    {
        TerrainModTranslator * titm = new TerrainModTranslator;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "ball";
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with ball shape and valid ball params
    {
        TerrainModTranslator * titm = new TerrainModTranslator;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "ball";
        shape_desc["radius"] = 1.f;
        shape_desc["position"] = ListType(2, 1.);
        mod["shape"] = shape_desc;
        mod["type"] = "levelmod";
        bool ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    // Call parseData with ball shape and valid ball and orientation
    {
        TerrainModTranslator * titm = new TerrainModTranslator;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation(0,0,0,1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "ball";
        shape_desc["radius"] = 1.f;
        shape_desc["position"] = ListType(2, 1.);
        mod["shape"] = shape_desc;
        mod["type"] = "levelmod";
        bool ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    // Call parseData with polygon shape and valid polygon params
    {
        TerrainModTranslator * titm = new TerrainModTranslator;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        mod["type"] = "levelmod";
        bool ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    // Call parseData with rotbox shape and valid rotbox params
    {
        TerrainModTranslator * titm = new TerrainModTranslator;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "rotbox";
        shape_desc["point"] = ListType(2, 1.);
        shape_desc["size"] = ListType(2, 1.);
        mod["shape"] = shape_desc;
        mod["type"] = "levelmod";
        bool ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    // Call parseData with ball shape and invalid ball params
    {
        TerrainModTranslator * titm = new TerrainModTranslator;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "ball";
        shape_desc["radius"] = 1.f;
        shape_desc["position"] = ListType(3, "1");
        mod["shape"] = shape_desc;
        mod["type"] = "levelmod";
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);
        assert(titm->getModifier() == 0);

        delete titm;
    }

    return test_reparse();
}

// stubs

#include "common/log.h"

void log(LogLevel lvl, const std::string & msg)
{
}
