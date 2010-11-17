// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003-2005 Alistair Riddoch
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

#include "modules/Location.h"

#include "rulesets/Entity.h"

#include "common/log.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootOperation.h>

#include <cassert>


void testDistanceFunctions()
{
    {
        Location * l = new Location;
        delete l;
    }

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

    // Coverage for broken entity hierarchy
    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2);

        // BROKEN ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        // BROKEN ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent1.m_location, ent2.m_location);
        assert(relPos.isValid());

        std::cout << "RelPos ent1 -> ent2: " << relPos
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        // BROKEN ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent1.m_location, ent2.m_location);
        assert(relPos.isValid());

        std::cout << "RelPos ent1 -> ent2: " << relPos
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2);

        // BROKEN ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent1.m_location, ent2.m_location);
        assert(relPos.isValid());

        std::cout << "RelPos ent1 -> ent2: " << relPos
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
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
        assert(relPos.isValid());

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

    // Coverage for no orientation
    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2),
               ent3("3", 3), ent4("4", 4);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);

        ent3.m_location.m_loc = &ent1;
        ent3.m_location.m_pos = Point3D(-1, 1, 0);

        ent4.m_location.m_loc = &ent2;
        ent4.m_location.m_pos = Point3D(1, 1, 0);

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
        float d = squareDistance(ent3.m_location, ent4.m_location);
        float hd = squareHorizontalDistance(ent3.m_location, ent4.m_location);

        std::cout << "RelPos ent3 -> ent4: " << relPos
                  << " Distance ent3 -> ent4: " << distance
                  << " square distance ent3 -> ent4: " << d
                  << " square horizontal distance ent3 -> ent4: " << hd
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

}

int main()
{
    {
        Location testloc;

        assert(testloc.m_loc == 0);
        assert(!testloc.pos().isValid());
        assert(!testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());

        testloc.modifyBBox();
    }

    {
        Location testloc(0);

        assert(testloc.m_loc == 0);
        assert(!testloc.pos().isValid());
        assert(!testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());
    }

    {
        Point3D testPos;
        Location testloc(0, testPos);

        assert(!testPos.isValid());

        assert(testloc.m_loc == 0);
        assert(!testloc.pos().isValid());
        assert(!testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());
    }

    {
        Point3D testPos(0,0,0);
        Location testloc(0, testPos);

        assert(testPos.isValid());

        assert(testloc.m_loc == 0);
        assert(testloc.pos().isValid());
        assert(!testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());
    }

    {
        Point3D testPos(0,0,0);
        Vector3D testVel;
        Location testloc(0, testPos, testVel);

        assert(testPos.isValid());
        assert(!testVel.isValid());

        assert(testloc.m_loc == 0);
        assert(testloc.pos().isValid());
        assert(!testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());
    }

    {
        Point3D testPos(0,0,0);
        Vector3D testVel(0,0,0);
        Location testloc(0, testPos, testVel);

        assert(testPos.isValid());
        assert(testVel.isValid());

        assert(testloc.m_loc == 0);
        assert(testloc.pos().isValid());
        assert(testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());

        Quaternion testOrientation(1, 0, 0, 0);
        assert(testOrientation.isValid());
        testloc.m_orientation = testOrientation;
        assert(testloc.orientation().isValid());
    }

    {
        Location testloc;

        testloc.m_bBox = BBox(Point3D(0,0,0), Point3D(1,1,1));

        assert(testloc.m_loc == 0);
        assert(!testloc.pos().isValid());
        assert(!testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());
        assert(testloc.bBox().isValid());

        testloc.setBBox(BBox(Point3D(1,1,1), Point3D(2,2,2)));

        assert(testloc.squareBoxSize() == 3.f);
        assert(testloc.boxSize() == sqrtf(3.f));

        assert(testloc.squareRadius() == 12.f);
        assert(testloc.radius() == sqrtf(12.f));

        // Check cached values have been changed
    }

    // Coverage for addToFoo()
    {
        Location testLoc;

        Atlas::Message::MapType msg;
        Atlas::Objects::Entity::Anonymous ent;

        testLoc.addToMessage(msg);
        testLoc.addToEntity(ent);

        Entity le1("1", 1);

        testLoc.m_loc = &le1;
        testLoc.m_pos = Point3D(0,0,0);
        testLoc.m_velocity = Vector3D(1,0,0);
        testLoc.m_orientation = Quaternion(1, 0, 0, 0);
        testLoc.m_bBox = BBox(Point3D(-1,-1,-1), Point3D(1,1,1));

        testLoc.addToMessage(msg);
        testLoc.addToEntity(ent);

        (void)testLoc.asEntity();

        {
            Location readLocFromMessage;

            readLocFromMessage.readFromMessage(msg);

            assert(msg["pos"].isList());
            assert(msg["pos"].asList().size() == 3);

            // Make the list too long
            msg["pos"].asList().push_back(1);
            assert(msg["pos"].asList().size() == 4);
            readLocFromMessage.readFromMessage(msg);
            msg["pos"].asList().pop_back();
            assert(msg["pos"].asList().size() == 3);
            // Now it is back to the right size

            // Make the first item in the list a string
            msg["pos"].asList().front() = "string";
            assert(msg["pos"].asList().front().isString());
            readLocFromMessage.readFromMessage(msg);
            msg["pos"].asList().front() = 0.f;
            assert(msg["pos"].asList().front().isNum());
            // Now it is back to the right type

            // Make the list too long
            msg["orientation"].asList().push_back(1);
            assert(msg["orientation"].asList().size() == 5);
            readLocFromMessage.readFromMessage(msg);
            msg["orientation"].asList().pop_back();
            assert(msg["orientation"].asList().size() == 4);
            // Now it is back to the right size

            // Make the first item in the list a string
            msg["orientation"].asList().front() = "string";
            assert(msg["orientation"].asList().front().isString());
            readLocFromMessage.readFromMessage(msg);
            msg["orientation"].asList().front() = 1.f;
            assert(msg["orientation"].asList().front().isNum());
            // Now it is back to the right type

            // Make it not a list
            msg["pos"] = "string";
            assert(msg["pos"].isString());
            msg["orientation"] = "string";
            assert(msg["orientation"].isString());
            readLocFromMessage.readFromMessage(msg);
        }

        {
            Location readLocFromEntity;

            readLocFromEntity.readFromEntity(ent);

            assert(!ent->isDefaultPos());
            assert(ent->getPos().size() == 3);
            assert(!ent->isDefaultVelocity());
            assert(ent->getVelocity().size() == 3);

            readLocFromEntity.readFromEntity(ent);

            Atlas::Message::Element orientation;
            bool res = ent->copyAttr("orientation", orientation);
            assert(res == 0);

            // Make the list too long
            orientation.asList().push_back(1);
            assert(orientation.asList().size() == 5);
            ent->setAttr("orientation", orientation);
            readLocFromEntity.readFromEntity(ent);
            orientation.asList().pop_back();
            assert(orientation.asList().size() == 4);
            // Now it is back to the right size

            // Make the first item in the list a string
            orientation.asList().front() = "string";
            assert(orientation.asList().front().isString());
            ent->setAttr("orientation", orientation);
            readLocFromEntity.readFromEntity(ent);
            orientation.asList().front() = 1.f;
            assert(orientation.asList().front().isNum());
            // Now it is back to the right type

            // Make it not a list
            orientation = "string";
            assert(orientation.isString());
            ent->setAttr("orientation", orientation);
            readLocFromEntity.readFromEntity(ent);
        }
    }

    testDistanceFunctions();

    return 0;
}

// stubs

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
