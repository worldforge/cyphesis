// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2008 Alistair Riddoch
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


#include "Location.h"
#include "LocatedEntity.h"

#include "common/debug.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Anonymous.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

Location::Location() :
        m_timeStamp(0),
        m_radius(0),
        m_squareRadius(0)
{
}

Location::Location(Ref<LocatedEntity> rf) :
        EntityLocation(std::move(rf)),
        m_timeStamp(0),
        m_radius(0),
        m_squareRadius(0)
{
}

Location::Location(Ref<LocatedEntity> rf, const Point3D& pos) :
        EntityLocation(std::move(rf), pos),
        m_timeStamp(0),
        m_radius(0),
        m_squareRadius(0)
{
}

Location::Location(Ref<LocatedEntity> rf,
                   const Point3D& pos,
                   Vector3D velocity) :
        EntityLocation(std::move(rf), pos),
        m_timeStamp(0),
        m_radius(0),
        m_squareRadius(0),
        m_velocity(velocity)
{
}

Location::Location(LocatedEntity* rf) :
        EntityLocation(rf),
        m_timeStamp(0),
        m_radius(0),
        m_squareRadius(0)
{
}

Location::Location(LocatedEntity* rf, const Point3D& pos) :
        EntityLocation(rf, pos),
        m_timeStamp(0),
        m_radius(0),
        m_squareRadius(0)
{
}

Location::Location(EntityLocation entityLocation) :
        EntityLocation(std::move(entityLocation)),
        m_timeStamp(0),
        m_radius(0),
        m_squareRadius(0)
{
}

Location::Location(LocatedEntity* rf,
                   const Point3D& pos,
                   Vector3D velocity) :
        EntityLocation(rf, pos),
        m_timeStamp(0),
        m_radius(0),
        m_squareRadius(0),
        m_velocity(velocity)
{
}

void Location::addToMessage(MapType& omap) const
{
    if (m_parent != nullptr) {
        omap["loc"] = m_parent->getId();
    }
    if (pos().isValid()) {
        omap["pos"] = pos().toAtlas();
    }
    if (velocity().isValid()) {
        omap["velocity"] = velocity().toAtlas();
    }
    if (orientation().isValid()) {
        omap["orientation"] = orientation().toAtlas();
    }
    if (m_angularVelocity.isValid()) {
        omap["angular"] = m_angularVelocity.toAtlas();
    }
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity& ent) const
{
    if (m_parent != nullptr) {
        ent->setLoc(m_parent->getId());
    }
    if (pos().isValid()) {
        ::addToEntity(pos(), ent->modifyPos());
    }
    if (velocity().isValid()) {
        ::addToEntity(velocity(), ent->modifyVelocity());
    }
    if (orientation().isValid()) {
        ent->setAttr("orientation", orientation().toAtlas());
    }
    if (m_angularVelocity.isValid()) {
        ent->setAttr("angular", m_angularVelocity.toAtlas());
    }
}

bool Location::readFromMessage(const MapType& msg)
{
    bool had_data = false;
    try {
        auto I = msg.find("pos");
        auto Iend = msg.end();
        if (I != Iend) {
            had_data = true;
            const Element& pos = I->second;
            if (pos.isList() && pos.List().size() == 3) {
                m_pos.fromAtlas(pos);
            } else {
                log(ERROR, "Malformed POS data");
            }
        }
        I = msg.find("velocity");
        if (I != Iend) {
            had_data = true;
            const Element& velocity = I->second;
            if (velocity.isList() && velocity.List().size() == 3) {
                m_velocity.fromAtlas(velocity);
            } else {
                log(ERROR, "Malformed velocity data");
            }
        }
        I = msg.find("orientation");
        if (I != Iend) {
            had_data = true;
            const Element& orientation = I->second;
            if (orientation.isList() && orientation.List().size() == 4) {
                m_orientation.fromAtlas(orientation);
            } else {
                log(ERROR, "Malformed ORIENTATION data");
            }
        }
        I = msg.find("angular");
        if (I != Iend) {
            had_data = true;
            const Element& angular = I->second;
            if (angular.isList() && angular.List().size() == 3) {
                m_angularVelocity.fromAtlas(angular);
            } else {
                log(ERROR, "Malformed angular velocity data");
            }
        }
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "Location::readFromMessage: Bad location data");
    }
    return had_data;
}

bool Location::readFromEntity(const Atlas::Objects::Entity::RootEntity& ent)
{
    bool had_data = false;
    debug_print("Location::readFromEntity")
    try {
        if (ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
            had_data = true;
            fromStdVector(m_pos, ent->getPos());
        }
        if (ent->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            had_data = true;
            fromStdVector(m_velocity, ent->getVelocity());
        }
        Element element;
        if (ent->copyAttr("orientation", element) == 0) {
            had_data = true;
            if (element.isList() && element.List().size() == 4) {
                m_orientation.fromAtlas(element);
            } else {
                log(ERROR, "Malformed ORIENTATION data.");
            }
        }
        if (ent->copyAttr("angular", element) == 0) {
            had_data = true;
            if (element.isList() && element.List().size() == 3 && element.List()[0].isNum() && element.List()[1].isNum() && element.List()[2].isNum()) {
                m_angularVelocity = WFMath::Vector<3>(static_cast<WFMath::CoordType>(element.List()[0].asNum()), static_cast<WFMath::CoordType>(element.List()[1].asNum()),
                                                      static_cast<WFMath::CoordType>(element.List()[2].asNum()));
            } else {
                log(ERROR, "Malformed angular velocity data.");
            }
        }
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "Location::readFromEntity: Bad location data");
    }
    return had_data;
}

void Location::modifyBBox()
{
    if (!m_bBox.isValid()) {
        return;
    }

    m_squareRadius = std::max(square(m_bBox.lowCorner().x()) +
                              square(m_bBox.lowCorner().y()) +
                              square(m_bBox.lowCorner().z()),
                              square(m_bBox.highCorner().x()) +
                              square(m_bBox.highCorner().y()) +
                              square(m_bBox.highCorner().z()));
    m_radius = std::sqrt(m_squareRadius);
}

static const Location* distanceToAncestor(const Location& self,
                                          const Location& other, Point3D& c)
{
//    c.setToOrigin();
//    const Location* ancestor = distanceFromAncestor(self, other, c);
//    if (ancestor) {
//        return ancestor;
//    } else if ((self.m_parent != nullptr)) {
//        if (!self.m_pos.isValid()) {
//            return nullptr;
//        }
//        ancestor = distanceToAncestor(self.m_parent->m_location, other, c);
//        if (ancestor) {
//            if (self.orientation().isValid()) {
//                c = c.toLocalCoords(self.m_pos, self.orientation());
//            } else {
//                c = c.toLocalCoords(self.m_pos, Quaternion::IDENTITY());
//            }
//            return ancestor;
//        }
//    }
//
//    c.setValid(false); //Mark distance as invalid since there's no connection between locations.

    return nullptr;
}

/// \brief Determine the vector distance from self to other.
///
/// @param self Location of an entity
/// @param other Location of an entity the distance of which is to be
///        determined
/// @return The vector distance from self to other.
/// The distance calculated is a vector relative to the parent of the
/// entity who's location is given by self. This is useful for determining
/// both the scalar distance to another entity, and a direction vector
/// that can be used to determine the direction for motion if it
/// is necessary to head toward the other entity.
Vector3D distanceTo(const Location& self, const Location& other)
{
    static Point3D origin(0, 0, 0);
    Point3D pos;
    distanceToAncestor(self, other, pos);
    Vector3D dist = pos - origin;
    if (self.orientation().isValid()) {
        dist.rotate(self.orientation());
    }
    return dist;
}

/// \brief Determine the position of other relative to self.
///
/// @param self Location of an entity
/// @param other Location of an entity this position of which is to be
///        determined
/// @return The position of other.
/// The position calculated is relative to the entity who's location is given
/// by self. The calculation is very similar to distanceTo() but an extra
/// step is omitted.
Point3D relativePos(const Location& self, const Location& other)
{
    Point3D pos;
    distanceToAncestor(self, other, pos);
    return pos;
}

boost::optional<WFMath::CoordType> squareDistance(const Location& self, const Location& other)
{
    Point3D dist;
    distanceToAncestor(self, other, dist);
    if (!dist.isValid()) {
        return boost::none;
    }
    return sqrMag(dist);
}

WFMath::CoordType squareDistanceWithAncestor(const Location& self, const Location& other, const Location** ancestor)
{
    Point3D dist;
    *ancestor = distanceToAncestor(self, other, dist);
    if (*ancestor) {
        return sqrMag(dist);
    }
    return 0.f;
}


boost::optional<WFMath::CoordType> squareHorizontalDistance(const Location& self, const Location& other)
{
    Point3D dist;
    distanceToAncestor(self, other, dist);
    if (!dist.isValid()) {
        return boost::none;
    }
    dist.y() = 0.f;
    return sqrMag(dist);
}

std::ostream& operator<<(std::ostream& s, Location& v)
{
    s << "{";
    if (v.m_parent != nullptr) {
        s << v.m_parent->getId();
    } else {
        s << "null";
    }
    if (v.pos().isValid()) {
        s << "," << v.pos();
        if (v.velocity().isValid()) {
            s << "," << v.velocity();
        }
    }
    return s << "}";
}
