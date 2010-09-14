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

using Atlas::Message::MapType;

class TestInnerTerrainMod : public InnerTerrainMod
{
  public:
    TestInnerTerrainMod() : InnerTerrainMod("test") { }

    virtual bool parseAtlasData(Entity * owner, const Atlas::Message::MapType& modElement)
    {
        return true;
    }

    virtual Mercator::TerrainMod* getModifier()
    {
        return 0;
    }

    WFMath::Point<3> test_parsePosition(Entity * owner, const MapType& modElement)
    {
        return parsePosition(owner, modElement);
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
        Point3D new_pos = titm->test_parsePosition(&e, data);
        assert(new_pos.isValid());
        assert(new_pos.z() < 0);

        delete titm;
    }

    // Call parsePosition with int height data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        data["height"] = 1;
        Point3D new_pos = titm->test_parsePosition(&e, data);
        assert(new_pos.isValid());
        assert(new_pos.z() > 0);

        delete titm;
    }

    // Call parsePosition with float height data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        data["height"] = 1.;
        Point3D new_pos = titm->test_parsePosition(&e, data);
        assert(new_pos.isValid());
        assert(new_pos.z() > 0);

        delete titm;
    }

    // Call parsePosition with bad (string) height data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        data["height"] = "1.";
        Point3D new_pos = titm->test_parsePosition(&e, data);
        assert(new_pos.isValid());
        assert(new_pos.z() < 0);

        delete titm;
    }

    // Call parsePosition with int heightoffset data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        data["heightoffset"] = 2;
        Point3D new_pos = titm->test_parsePosition(&e, data);
        assert(new_pos.isValid());
        assert(new_pos.z() > 0);

        delete titm;
    }

    // Call parsePosition with float heightoffset data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        data["heightoffset"] = 2.;
        Point3D new_pos = titm->test_parsePosition(&e, data);
        assert(new_pos.isValid());
        assert(new_pos.z() > 0);

        delete titm;
    }

    // Call parsePosition with bad (string) heightoffset data
    {
        TestInnerTerrainMod * titm = new TestInnerTerrainMod;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        data["heightoffset"] = "1.";
        Point3D new_pos = titm->test_parsePosition(&e, data);
        assert(new_pos.isValid());
        assert(new_pos.z() < 0);

        delete titm;
    }

    // FIXME test parseShape()

    ////////////////////// Specific classes ///////////////////////////

    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        delete titm;
    }

    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        assert(titm->getModifier() == 0);
        delete titm;
    }

    {
        InnerTerrainModCrater * titm = new InnerTerrainModCrater;
        Entity e("1", 1);
        e.m_location.m_pos = Point3D(0,0,-1);

        MapType data;
        titm->parseAtlasData(&e, data);

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

Location::Location()
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
