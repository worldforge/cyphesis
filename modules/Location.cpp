// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#include "rulesets/Entity.h"

#include "common/log.h"
#include "common/debug.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

Location::Location() :
    m_simple(true), m_solid(true), m_loc(0)
{
}

Location::Location(Entity * rf) :
    m_simple(true), m_solid(true), m_loc(rf)
{
}

Location::Location(Entity * rf, const Point3D & crds) :
    m_simple(true), m_solid(true), m_loc(rf), m_pos(crds)
{
}

Location::Location(Entity * rf, const Point3D& crds, const Vector3D& vel) :
    m_simple(true), m_solid(true), m_loc(rf), m_pos(crds), m_velocity(vel)
{
}

void Location::addToMessage(Atlas::Message::MapType & omap) const
{
    if (m_loc!=NULL) {
        omap["loc"] = m_loc->getId();
    }
    if (m_pos.isValid()) {
        omap["pos"] = m_pos.toAtlas();
    }
    if (m_velocity.isValid()) {
        omap["velocity"] = m_velocity.toAtlas();
    }
    if (m_acceleration.isValid()) {
        omap["accel"] = m_acceleration.toAtlas();
    }
    if (m_orientation.isValid()) {
        omap["orientation"] = m_orientation.toAtlas();
    }
    if (m_angular.isValid()) {
        omap["angular"] = m_angular.toAtlas();
    }
    if (m_bBox.isValid()) {
        omap["bbox"] = m_bBox.toAtlas();
    }
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
    if (m_loc!=NULL) {
        ent->setLoc(m_loc->getId());
    }
    if (m_pos.isValid()) {
        ::addToEntity(m_pos, ent->modifyPos());
    }
    if (m_velocity.isValid()) {
        ::addToEntity(m_velocity, ent->modifyVelocity());
    }
    if (m_acceleration.isValid()) {
        ent->setAttr("accel", m_acceleration.toAtlas());
    }
    if (m_orientation.isValid()) {
        ent->setAttr("orientation", m_orientation.toAtlas());
    }
    if (m_angular.isValid()) {
        ent->setAttr("angular", m_angular.toAtlas());
    }
    if (m_bBox.isValid()) {
        ent->setAttr("bbox", m_bBox.toAtlas());
    }
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
        Atlas::Message::Element orientation;
        if (ent->copyAttr("orientation", orientation) == 0) {
            if (orientation.isList()) {
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

    if (other.m_orientation.isValid()) {
        c = c.toParentCoords(other.m_pos, other.m_orientation);
    } else {
        static const Quaternion identity(1, 0, 0, 0);
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
        if (self.m_orientation.isValid()) {
            c = c.toLocalCoords(self.m_pos, self.m_orientation);
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

const Vector3D distanceTo(const Location & self, const Location & other)
{
    static Point3D origin(0,0,0);
    Point3D pos;
    distanceToAncestor(self, other, pos);
    Vector3D dist = pos - origin;
    if (self.m_orientation.isValid()) {
        dist.rotate(self.m_orientation);
    }
    return dist;
}

const Point3D relativePos(const Location & self, const Location & other)
{
    Point3D pos;
    distanceToAncestor(self, other, pos);
    return pos;
}

const float squareDistance(const Location & self, const Location & other)
{
    Point3D dist;
    distanceToAncestor(self, other, dist);
    return sqrMag(dist);
}

const float squareHorizontalDistance(const Location & self,
                                     const Location & other)
{
    Point3D dist;
    distanceToAncestor(self, other, dist);
    dist.z() = 0.f;
    return sqrMag(dist);
}
