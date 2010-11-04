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

#include "rulesets/Entity.h"

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

class TestInnerTerrainMod : public InnerTerrainMod
{
  public:
    TestInnerTerrainMod() : InnerTerrainMod("test") { }

    virtual bool parseAtlasData(const WFMath::Point<3> & pos, const WFMath::Quaternion & orientation, const Atlas::Message::MapType& modElement)
    {
        return true;
    }

    virtual Mercator::TerrainMod* getModifier()
    {
        return 0;
    }

    float test_parsePosition(const WFMath::Point<3> & pos, const MapType& modElement)
    {
        return parsePosition(pos, modElement);
    }

    InnerTerrainMod::Shape test_parseShape(const Atlas::Message::MapType& modElement, Atlas::Message::Element& shapeMap)
    {
        return parseShape(modElement, shapeMap);
    }
};

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
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        float z = titm->test_parsePosition(e.m_location.pos(), data);
        assert(z < 0);

        delete titm;
    }

    // Call parsePosition with int height data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        data["height"] = 1;
        float z = titm->test_parsePosition(e.m_location.pos(), data);
        assert(z > 0);

        delete titm;
    }

    // Call parsePosition with float height data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        data["height"] = 1.;
        float z = titm->test_parsePosition(e.m_location.pos(), data);
        assert(z > 0);

        delete titm;
    }

    // Call parsePosition with bad (string) height data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        data["height"] = "1.";
        float z = titm->test_parsePosition(e.m_location.pos(), data);
        assert(z < 0);

        delete titm;
    }

    // Call parsePosition with int heightoffset data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        data["heightoffset"] = 2;
        float z = titm->test_parsePosition(e.m_location.pos(), data);
        assert(z > 0);

        delete titm;
    }

    // Call parsePosition with float heightoffset data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        data["heightoffset"] = 2.;
        float z = titm->test_parsePosition(e.m_location.pos(), data);
        assert(z > 0);

        delete titm;
    }

    // Call parsePosition with bad (string) heightoffset data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        data["heightoffset"] = "1.";
        float z = titm->test_parsePosition(e.m_location.pos(), data);
        assert(z < 0);

        delete titm;
    }

    // Call parseShape with empty mod data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;

        Element e;
        MapType mod;
        InnerTerrainMod::Shape shape = titm->test_parseShape(mod, e);

        assert(shape == InnerTerrainMod::SHAPE_UNKNOWN);
        assert(e.isNone());
        delete titm;
    }

    // Call parseShape with invalid shape data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;

        Element e;
        MapType mod;
        mod["shape"] = "invalid_shape";
        InnerTerrainMod::Shape shape = titm->test_parseShape(mod, e);

        assert(shape == InnerTerrainMod::SHAPE_UNKNOWN);
        assert(e.isNone());
        delete titm;
    }

    // Call parseShape with empty shape data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;

        Element e;
        MapType mod;
        mod["shape"] = MapType();
        InnerTerrainMod::Shape shape = titm->test_parseShape(mod, e);

        assert(shape == InnerTerrainMod::SHAPE_UNKNOWN);
        delete titm;
    }

    // Call parseShape with invalid shape type
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;

        Element e;
        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = 1;
        mod["shape"] = shape_desc;
        InnerTerrainMod::Shape shape = titm->test_parseShape(mod, e);

        assert(shape == InnerTerrainMod::SHAPE_UNKNOWN);
        delete titm;
    }

    // Call parseShape with valid shape type
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;

        Element e;
        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "rotbox";
        mod["shape"] = shape_desc;
        InnerTerrainMod::Shape shape = titm->test_parseShape(mod, e);

        assert(shape == InnerTerrainMod::SHAPE_ROTBOX);
        assert(e.isMap());
        delete titm;
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

    // Call parseAtlasData with empty map
    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), data);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with unknown shape
    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "unknown_shape";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);


        delete titm;
    }

    // Call parseAtlasData with ball shape
    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "ball";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with ball shape and valid ball params
    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "ball";
        shape_desc["radius"] = 1.f;
        shape_desc["position"] = ListType(2, 1.);
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    // Call parseAtlasData with ball shape and invalid ball params
    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "ball";
        shape_desc["radius"] = 1.f;
        shape_desc["position"] = ListType(3, "1");
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
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

    // Call parseAtlasData with empty map
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), data);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with malformed slope
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        mod["slopes"] = 1;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with empty slope
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        mod["slopes"] = ListType();
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with non-numeric slope
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        mod["slopes"] = ListType(2, "naughty_string");
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with valid slope
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        mod["slopes"] = ListType(2, 2.);
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with unknown shape
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        mod["slopes"] = ListType(2, 2.);
        MapType shape_desc;
        shape_desc["type"] = "unknown";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with ball shape
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        mod["slopes"] = ListType(2, 2.);
        MapType shape_desc;
        shape_desc["type"] = "ball";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with rotbox shape
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        mod["slopes"] = ListType(2, 2.);
        MapType shape_desc;
        shape_desc["type"] = "rotbox";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with polygon shape
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        mod["slopes"] = ListType(2, 2.);
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with polygon shape and valid polygon
    {
        InnerTerrainModSlope * titm = new InnerTerrainModSlope;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        mod["slopes"] = ListType(2, 2.);
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(ret);
        assert(titm->getModifier() != 0);

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

    // Call parseAtlasData with empty map
    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), data);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with unknown shape
    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "unknown_shape";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with ball shape
    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "ball";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with rotbox shape
    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "rotbox";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with polygon shape
    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with polygon shape and valid points
    {
        InnerTerrainModLevel * titm = new InnerTerrainModLevel;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
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

    // Call parseAtlasData with empty map
    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), data);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with unknown shape
    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "unknown_shape";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with ball shape
    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "ball";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with rotbox shape
    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "rotbox";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with polygon shape
    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(!ret);

        delete titm;
    }

    // Call parseAtlasData with polygon shape and valid points
    {
        InnerTerrainModAdjust * titm = new InnerTerrainModAdjust;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType mod;
        MapType shape_desc;
        shape_desc["type"] = "polygon";
        shape_desc["points"] = ListType(3, ListType(2, 1.));
        mod["shape"] = shape_desc;
        bool ret = titm->parseAtlasData(e.m_location.pos(), e.m_location.orientation(), mod);
        assert(ret);
        assert(titm->getModifier() != 0);

        delete titm;
    }

    return 0;
}

// stubs

#include "rulesets/Script.h"

#include "common/log.h"

Script noScript;

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId), m_motion(0), m_flags(0)
{
}

Entity::~Entity()
{
}

void Entity::destroy()
{
    destroyed.emit();
}

void Entity::ActuateOperation(const Operation &, OpVector &)
{
}

void Entity::AppearanceOperation(const Operation &, OpVector &)
{
}

void Entity::AttackOperation(const Operation &, OpVector &)
{
}

void Entity::CombineOperation(const Operation &, OpVector &)
{
}

void Entity::CreateOperation(const Operation &, OpVector &)
{
}

void Entity::DeleteOperation(const Operation &, OpVector &)
{
}

void Entity::DisappearanceOperation(const Operation &, OpVector &)
{
}

void Entity::DivideOperation(const Operation &, OpVector &)
{
}

void Entity::EatOperation(const Operation &, OpVector &)
{
}

void Entity::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Entity::LookOperation(const Operation &, OpVector &)
{
}

void Entity::MoveOperation(const Operation &, OpVector &)
{
}

void Entity::NourishOperation(const Operation &, OpVector &)
{
}

void Entity::SetOperation(const Operation &, OpVector &)
{
}

void Entity::SightOperation(const Operation &, OpVector &)
{
}

void Entity::SoundOperation(const Operation &, OpVector &)
{
}

void Entity::TalkOperation(const Operation &, OpVector &)
{
}

void Entity::TickOperation(const Operation &, OpVector &)
{
}

void Entity::TouchOperation(const Operation &, OpVector &)
{
}

void Entity::UpdateOperation(const Operation &, OpVector &)
{
}

void Entity::UseOperation(const Operation &, OpVector &)
{
}

void Entity::WieldOperation(const Operation &, OpVector &)
{
}

void Entity::externalOperation(const Operation & op)
{
}

void Entity::operation(const Operation & op, OpVector & res)
{
}

void Entity::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Entity::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Entity::setAttr(const std::string & name,
                     const Atlas::Message::Element & attr)
{
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

void Entity::onContainered()
{
}

void Entity::onUpdated()
{
}

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(&noScript), m_type(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

bool LocatedEntity::getAttr(const std::string & name, Atlas::Message::Element & attr) const
{
    return false;
}

bool LocatedEntity::getAttrType(const std::string & name,
                                Atlas::Message::Element & attr,
                                int type) const
{
    return false;
}

void LocatedEntity::setAttr(const std::string & name, const Atlas::Message::Element & attr)
{
    return;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

void LocatedEntity::onContainered()
{
}

void LocatedEntity::onUpdated()
{
}

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
}

void LocatedEntity::merge(const MapType & ent)
{
}

Router::Router(const std::string & id, long intId) : m_id(id),
                                                             m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Router::error(const Operation & op,
                   const std::string & errstring,
                   OpVector & res,
                   const std::string & to) const
{
}

void Location::addToMessage(MapType & omap) const
{
}

Location::Location() : m_loc(0)
{
}

Location::Location(LocatedEntity * rf, const Point3D & pos)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

Script::Script()
{
}

/// \brief Script destructor
Script::~Script()
{
}

bool Script::operation(const std::string & opname,
                       const Atlas::Objects::Operation::RootOperation & op,
                       OpVector & res)
{
   return false;
}

void Script::hook(const std::string & function, LocatedEntity * entity)
{
}

void log(LogLevel lvl, const std::string & msg)
{
}
