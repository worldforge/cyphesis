// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#include "rulesets/Entity.h"

#include "modules/Location.h"

#include "common/log.h"

// RelPos to self: (0,0,0)
// RelPos ent -> tlve: (-1,-1,0)
// RelPos tlve -> ent: (1,1,0)
// RelPos ent1 -> ent2: (2,0,0)
// RelPos ent3 -> ent4: (4,0,0)
// RelPos ent3 -> ent4: (-1.36914e-07,-4,0)
// RelPos ent3 -> ent4: (-2,-4,0)
// RelPos ent3 -> ent4: (-4,-2,0)

int main()
{
    int ret = 0;

    {
        Entity tlve("0", 0), ent("1", 1);

        ent.m_location.m_loc = &tlve;
        ent.m_location.m_pos = Point3D(1, 1, 0);
        ent.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent.m_location, ent.m_location);

        std::cout << "RelPos to self: " << relPos << std::endl << std::flush;

        relPos = relativePos(ent.m_location, tlve.m_location);

        std::cout << "RelPos ent -> tlve: " << relPos
                  << std::endl << std::flush;

        relPos = relativePos(tlve.m_location, ent.m_location);

        std::cout << "RelPos tlve -> ent: " << relPos
                  << std::endl << std::flush;

        ent.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent1.m_location, ent2.m_location);

        std::cout << "RelPos ent1 -> ent2: " << relPos
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2),
               ent3("3", 3), ent4("4", 4);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        ent3.m_location.m_loc = &ent1;
        ent3.m_location.m_pos = Point3D(-1, 1, 0);
        ent3.m_location.m_orientation = WFMath::Quaternion().identity();

        ent4.m_location.m_loc = &ent2;
        ent4.m_location.m_pos = Point3D(1, 1, 0);
        ent4.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent3.m_location, ent4.m_location);

        std::cout << "RelPos ent3 -> ent4: " << relPos
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
        ent3.m_location.m_loc = 0;
        ent4.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2), ent3("3", 3), ent4("4", 4);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        ent3.m_location.m_loc = &ent1;
        ent3.m_location.m_pos = Point3D(-1, 1, 0);
        ent3.m_location.m_orientation = WFMath::Quaternion(2, M_PI / 2.f);

        ent4.m_location.m_loc = &ent2;
        ent4.m_location.m_pos = Point3D(1, 1, 0);
        ent4.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent3.m_location, ent4.m_location);

        std::cout << "RelPos ent3 -> ent4: " << relPos
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
        ent3.m_location.m_loc = 0;
        ent4.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2),
               ent3("3", 3), ent4("4", 4);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion(2, -M_PI / 2.f);

        ent3.m_location.m_loc = &ent1;
        ent3.m_location.m_pos = Point3D(-1, 1, 0);
        ent3.m_location.m_orientation = WFMath::Quaternion(2, M_PI / 2.f);

        ent4.m_location.m_loc = &ent2;
        ent4.m_location.m_pos = Point3D(1, 1, 0);
        ent4.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent3.m_location, ent4.m_location);

        std::cout << "RelPos ent3 -> ent4: " << relPos
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
        ent3.m_location.m_loc = 0;
        ent4.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2),
               ent3("3", 3), ent4("4", 4);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion(2, M_PI / 2.f);

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        ent3.m_location.m_loc = &ent1;
        ent3.m_location.m_pos = Point3D(-1, 1, 0);
        ent3.m_location.m_orientation = WFMath::Quaternion(2, M_PI / 2.f);

        ent4.m_location.m_loc = &ent2;
        ent4.m_location.m_pos = Point3D(1, 1, 0);
        ent4.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent3.m_location, ent4.m_location);
        Vector3D distance = distanceTo(ent3.m_location, ent4.m_location);

        std::cout << "RelPos ent3 -> ent4: " << relPos
                  << " Distance ent3 -> ent4: " << distance
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
        ent3.m_location.m_loc = 0;
        ent4.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        ent2.m_location.m_loc = &ent1;
        ent2.m_location.m_pos = Point3D(0, 0, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        Vector3D distance = distanceTo(ent1.m_location, ent2.m_location);

        std::cout << "Distance ent1 -> ent2: "
                  << distance << "," << distance.isValid()
                  << std::endl << std::flush;

        assert(distance.isValid());
        assert(distance == Vector3D(0,0,0));
        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
    }
    return ret;
}

// stubs

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId), m_motion(0), m_flags(0)
{
}

Entity::~Entity()
{
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

void Entity::onContainered()
{
}

void Entity::onUpdated()
{
}

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_contains(0)
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

void log(LogLevel lvl, const std::string & msg)
{
}
