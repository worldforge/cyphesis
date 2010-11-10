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

#include "rulesets/TerrainMod.h"

#include <wfmath/quaternion.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

class TestInnerTerrainMod : public InnerTerrainMod
{
  public:
    TestInnerTerrainMod() : InnerTerrainMod("test") { }

    virtual bool parseAtlasData(const WFMath::Point<3> & pos, const WFMath::Quaternion & orientation, const Atlas::Message::MapType& modElement, ShapeT, const Atlas::Message::MapType & )
    {
        return true;
    }

    virtual Mercator::TerrainMod* getModifier()
    {
        return 0;
    }

    static float test_parsePosition(const WFMath::Point<3> & pos, const MapType& modElement)
    {
        return parsePosition(pos, modElement);
    }

    static InnerTerrainMod::ShapeT test_parseShape(const Atlas::Message::MapType& modElement, Atlas::Message::Element& shapeMap)
    {
        return parseShape(modElement, shapeMap);
    }
};

static int test_reparse()
{
    // Call parseData with polygon shape and valid points
    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;
        bool ret;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        Mercator::TerrainMod * tm1 = titm->getModifier();
        assert(tm1 != 0);

        // Re-parse the same data. Should update the mod in place.
        ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        Mercator::TerrainMod * tm2 = titm->getModifier();
        assert(tm2 != 0);
        // FIXME assert(tm2 == tm1);

        // Change it to 2D ball shape. This requires a new mod.
        shape_desc["type"] = "ball";
        shape_desc["radius"] = 1.f;
        shape_desc["position"] = ListType(2, 1.);
        mod["shape"] = shape_desc;
        ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        Mercator::TerrainMod * tm3 = titm->getModifier();
        assert(tm3 != 0);

        delete titm;
    }

    return 0;
}

int main()
{
    {
        InnerTerrainMod * titm = new TestInnerTerrainMod;
        delete titm;
    }
    {
        InnerTerrainMod * titm = new TestInnerTerrainMod;

        const std::string & type = titm->getTypename();
        assert(type == "test");

        delete titm;
    }

    // Call parsePosition with empty height data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        float z = TestInnerTerrainMod::test_parsePosition(pos, data);
        assert(z < 0);
    }

    // Call parsePosition with int height data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        data["height"] = 1;
        float z = TestInnerTerrainMod::test_parsePosition(pos, data);
        assert(z > 0);
    }

    // Call parsePosition with float height data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        data["height"] = 1.;
        float z = TestInnerTerrainMod::test_parsePosition(pos, data);
        assert(z > 0);
    }

    // Call parsePosition with bad (string) height data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        data["height"] = "1.";
        float z = TestInnerTerrainMod::test_parsePosition(pos, data);
        assert(z < 0);
    }

    // Call parsePosition with int heightoffset data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        data["heightoffset"] = 2;
        float z = TestInnerTerrainMod::test_parsePosition(pos, data);
        assert(z > 0);
    }

    // Call parsePosition with float heightoffset data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        data["heightoffset"] = 2.;
        float z = TestInnerTerrainMod::test_parsePosition(pos, data);
        assert(z > 0);
    }

    // Call parsePosition with bad (string) heightoffset data
    {
        WFMath::Point<3> pos(0,0,-1);

        MapType data;
        data["heightoffset"] = "1.";
        float z = TestInnerTerrainMod::test_parsePosition(pos, data);
        assert(z < 0);
    }

    // Call parseShape with empty mod data
    {
        Element e;
        MapType mod;
        InnerTerrainMod::ShapeT shape = TestInnerTerrainMod::test_parseShape(mod, e);

        assert(shape == InnerTerrainMod::SHAPE_UNKNOWN);
        assert(e.isNone());
    }

    // Call parseShape with invalid shape data
    {
        Element e;
        MapType mod;
        mod["shape"] = "invalid_shape";
        InnerTerrainMod::ShapeT shape = TestInnerTerrainMod::test_parseShape(mod, e);

        assert(shape == InnerTerrainMod::SHAPE_UNKNOWN);
        assert(e.isNone());
    }

    // Call parseShape with empty shape data
    {
        Element e;
        MapType mod;
        mod["shape"] = MapType();
        InnerTerrainMod::ShapeT shape = TestInnerTerrainMod::test_parseShape(mod, e);

        assert(shape == InnerTerrainMod::SHAPE_UNKNOWN);
    }

    // Call parseShape with invalid shape type
    {
        Element e;
        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = 1;
        mod["shape"] = shape_desc;
        InnerTerrainMod::ShapeT shape = TestInnerTerrainMod::test_parseShape(mod, e);

        assert(shape == InnerTerrainMod::SHAPE_UNKNOWN);
    }

    // Call parseShape with valid shape type
    {
        Element e;
        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "rotbox";
        mod["shape"] = shape_desc;
        InnerTerrainMod::ShapeT shape = TestInnerTerrainMod::test_parseShape(mod, e);

        assert(shape == InnerTerrainMod::SHAPE_ROTBOX);
        assert(e.isMap());
    }

    ////////////////////// Concrete classes ///////////////////////////

    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        delete titm;
    }

    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        assert(titm->getModifier() == 0);
        delete titm;
    }

    // Call parseData with empty map
    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType data;
        bool ret = titm->parseData(pos, orientation, data);
        assert(!ret);

        delete titm;
    }

    // Call parseData with unknown shape
    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
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
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
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
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "ball";
        shape_desc["radius"] = 1.f;
        shape_desc["position"] = ListType(2, 1.);
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    // Call parseData with ball shape and valid ball and orientation
    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation(0,0,0,1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "ball";
        shape_desc["radius"] = 1.f;
        shape_desc["position"] = ListType(2, 1.);
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    // Call parseData with polygon shape and valid polygon params
    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    // Call parseData with rotbox shape and valid rotbox params
    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "rotbox";
        shape_desc["point"] = ListType(2, 1.);
        shape_desc["size"] = ListType(2, 1.);
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    // Call parseData with ball shape and invalid ball params
    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "ball";
        shape_desc["radius"] = 1.f;
        shape_desc["position"] = ListType(3, "1");
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);
        assert(titm->getModifier() == 0);

        delete titm;
    }

    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        delete titm;
    }

    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        assert(titm->getModifier() == 0);
        delete titm;
    }

    // Call parseData with empty map
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType data;
        bool ret = titm->parseData(pos, orientation, data);
        assert(!ret);

        delete titm;
    }

    // Call parseData with malformed slope
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        mod["slopes"] = 1;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with empty slope
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        mod["slopes"] = ListType();
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with non-numeric slope
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        mod["slopes"] = ListType(2, "naughty_string");
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with valid slope
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        mod["slopes"] = ListType(2, 2.);
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with unknown shape
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        mod["slopes"] = ListType(2, 2.);
        MapType shape_desc;
        shape_desc["type"] = "unknown";
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with ball shape
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        mod["slopes"] = ListType(2, 2.);
        MapType shape_desc;
        shape_desc["type"] = "ball";
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with rotbox shape
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        mod["slopes"] = ListType(2, 2.);
        MapType shape_desc;
        shape_desc["type"] = "rotbox";
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with polygon shape
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        mod["slopes"] = ListType(2, 2.);
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with polygon shape and valid polygon
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        mod["slopes"] = ListType(2, 2.);
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    // Call parseData with polygon shape and no slopes
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);
        assert(titm->getModifier() == 0);

        delete titm;
    }

    // Call parseData with polygon shape and non-list slopes
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        mod["slopes"] = "slopes";
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);
        assert(titm->getModifier() == 0);

        delete titm;
    }


    // Call parseData with polygon shape and wrong size list slopes
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        mod["slopes"] = ListType(1, 2.);
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);
        assert(titm->getModifier() == 0);

        delete titm;
    }

    // Call parseData with polygon shape and non numeric slopes
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        mod["slopes"] = ListType(1, "slope1");
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);
        assert(titm->getModifier() == 0);

        delete titm;
    }


    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
        delete titm;
    }

    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
        assert(titm->getModifier() == 0);
        delete titm;
    }

    // Call parseData with empty map
    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType data;
        bool ret = titm->parseData(pos, orientation, data);
        assert(!ret);

        delete titm;
    }

    // Call parseData with unknown shape
    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
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
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
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

    // Call parseData with rotbox shape
    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "rotbox";
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with polygon shape
    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with polygon shape and valid points
    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        delete titm;
    }

    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        assert(titm->getModifier() == 0);
        delete titm;
    }

    // Call parseData with empty map
    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType data;
        bool ret = titm->parseData(pos, orientation, data);
        assert(!ret);

        delete titm;
    }

    // Call parseData with unknown shape
    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
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
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
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

    // Call parseData with rotbox shape
    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "rotbox";
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with polygon shape
    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(!ret);

        delete titm;
    }

    // Call parseData with polygon shape and valid points
    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        WFMath::Point<3> pos(0,0,-1);
        WFMath::Quaternion orientation;

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        bool ret = titm->parseData(pos, orientation, mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    return test_reparse();
}

// stubs

#include "common/log.h"

void log(LogLevel lvl, const std::string & msg)
{
}
