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


#include "rulesets/LocatedEntity.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Anonymous.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

Location::Location() :
    m_simple(true), m_solid(true),
    m_boxSize(consts::minBoxSize),
    m_squareBoxSize(consts::minSqrBoxSize),
    m_loc(0)
{
}

Location::Location(LocatedEntity * rf) :
    m_simple(true), m_solid(true),
    m_boxSize(consts::minBoxSize),
    m_squareBoxSize(consts::minSqrBoxSize),
    m_loc(rf)
{
}

Location::Location(LocatedEntity * rf, const Point3D & pos) :
    m_simple(true), m_solid(true),
    m_boxSize(consts::minBoxSize),
    m_squareBoxSize(consts::minSqrBoxSize),
    m_loc(rf), m_pos(pos)
{
}

Location::Location(LocatedEntity * rf,
                   const Point3D& pos,
                   const Vector3D& velocity) :
    m_simple(true), m_solid(true),
    m_boxSize(consts::minBoxSize),
    m_squareBoxSize(consts::minSqrBoxSize),
    m_loc(rf), m_pos(pos), m_velocity(velocity)
{
}

void Location::addToMessage(MapType & omap) const
{
    if (m_loc!=NULL) {
        omap["loc"] = m_loc->getId();
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
    if (bBox().isValid()) {
        omap["bbox"] = bBox().toAtlas();
    }
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
    if (m_loc!=NULL) {
        ent->setLoc(m_loc->getId());
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
    if (bBox().isValid()) {
        ent->setAttr("bbox", bBox().toAtlas());
    }
}

int Location::readFromMessage(const MapType & msg)
{
    try {
        MapType::const_iterator I = msg.find("pos");
        MapType::const_iterator Iend = msg.end();
        if (I != Iend) {
            const Element & pos = I->second;
            if (pos.isList() && pos.List().size() == 3) {
                m_pos.fromAtlas(pos);
            } else {
                log(ERROR, "Malformed POS data");
            }
        }
        I = msg.find("orientation");
        if (I != Iend) {
            const Element & orientation = I->second;
            if (orientation.isList() && orientation.List().size() == 4) {
                m_orientation.fromAtlas(orientation);
            } else {
                log(ERROR, "Malformed ORIENTATION data");
            }
        }
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "Location::readFromMessage: Bad location data");
        return -1;
    }
    return 0;
}

int Location::readFromEntity(const Atlas::Objects::Entity::RootEntity & ent)
{
    debug( std::cout << "Location::readFromEntity" << std::endl << std::flush;);
    try {
        if (ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
            fromStdVector(m_pos, ent->getPos());
        }
        if (ent->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            fromStdVector(m_velocity, ent->getVelocity());
        }
        Element orientation;
        if (ent->copyAttr("orientation", orientation) == 0) {
            if (orientation.isList() && orientation.List().size() == 4) {
                m_orientation.fromAtlas(orientation);
            } else {
                log(ERROR, "Malformed ORIENTATION data");
            }
        }
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "Location::readFromEntity: Bad location data");
        return -1;
    }
    return 0;
}

void Location::modifyBBox()
{
    if (!m_bBox.isValid()) {
        return;
    }

    m_squareBoxSize = square(m_bBox.highCorner().x() - m_bBox.lowCorner().x()) +
                      square(m_bBox.highCorner().y() - m_bBox.lowCorner().y()) +
                      square(m_bBox.highCorner().z() - m_bBox.lowCorner().z());
    m_boxSize = std::sqrt(m_squareBoxSize);

    m_squareRadius = std::max(square(m_bBox.lowCorner().x()) +  
                              square(m_bBox.lowCorner().y()) +  
                              square(m_bBox.lowCorner().z()),
                              square(m_bBox.highCorner().x()) +  
                              square(m_bBox.highCorner().y()) +  
                              square(m_bBox.highCorner().z()));
    m_radius = std::sqrt(m_squareRadius);
}

void Location::setVisibility(float v)
{
    m_boxSize = v;
    m_squareBoxSize = v * v;
    // TODO m_radius and m_squareRadius? Unused everywhere for now.
}

const Atlas::Objects::Root Location::asEntity() const
{
    Anonymous ret;
    addToEntity(ret);
    return ret;
}

static bool distanceFromAncestor(const Location & self,
                                 const Location & other, Point3D & c)
{
    if (&self == &other) {
        return true;
    }

    if (other.m_loc == NULL) {
        return false;
    }

    if (other.orientation().isValid()) {
        c = c.toParentCoords(other.m_pos, other.orientation());
    } else {
        static const Quaternion identity((Quaternion::Identity()));
        c = c.toParentCoords(other.m_pos, identity);
    }

    return distanceFromAncestor(self, other.m_loc->m_location, c);
}

static bool distanceToAncestor(const Location & self,
                               const Location & other, Point3D & c)
{
    c.setToOrigin();
    if (distanceFromAncestor(self, other, c)) {
        return true;
    } else if ((self.m_loc != 0) &&
               distanceToAncestor(self.m_loc->m_location, other, c)) {
        if (self.orientation().isValid()) {
            c = c.toLocalCoords(self.m_pos, self.orientation());
        } else {
            static const Quaternion identity(1, 0, 0, 0);
            c = c.toLocalCoords(self.m_pos, identity);
        }
        return true;
    }
    log(ERROR, "Broken entity hierarchy doing distance calculation");
    if (self.m_loc != 0) {
        std::cerr << "Self(" << self.m_loc->getId() << "," << self.m_loc << ")"
                  << std::endl << std::flush;
    }
    if (other.m_loc != 0) {
        std::cerr << "Other(" << other.m_loc->getId() << "," << other.m_loc << ")"
                  << std::endl << std::flush;
    }
     
    return false;
}

/// \brief Determine the vector distance from self to other.
///
/// @param self Location of an entity
/// @param other Location of an entity the distance of which is to be
///        determined
/// @return The vector distance from self to other.
/// The distance calculated is a vector relative to the parent of the
/// entity who's location is given by self. This is useful for determing
/// both the scalar distance to another entity, and a direction vector
/// that can be used to determine the direction for motion if it
/// is necessary to head toward the other entity.
const Vector3D distanceTo(const Location & self, const Location & other)
{
    static Point3D origin(0,0,0);
    Point3D pos;
    distanceToAncestor(self, other, pos);
    Vector3D dist = pos - origin;
    if (self.orientation().isValid()) {
        dist.rotate(self.orientation());
    }
    return dist;
}

/// \brief Determine the postion of other relative to self.
///
/// @param self Location of an entity
/// @param other Location of an entity this position of which is to be
///        determined
/// @return The position of other.
/// The position calculated is relative to the entity who's location is given
/// by self. The calculation is very similar to distanceTo() but an extra
/// step is ommited.
const Point3D relativePos(const Location & self, const Location & other)
{
    Point3D pos;
    distanceToAncestor(self, other, pos);
    return pos;
}

float squareDistance(const Location & self, const Location & other)
{
    Point3D dist;
    distanceToAncestor(self, other, dist);
    return sqrMag(dist);
}

float squareHorizontalDistance(const Location & self, const Location & other)
{
    Point3D dist;
    distanceToAncestor(self, other, dist);
    dist.z() = 0.f;
    return sqrMag(dist);
}

std::ostream & operator<<(std::ostream& s, Location& v)
{
    s << "{";
    if (v.m_loc != 0) {
        s << v.m_loc->getId();
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
