// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "rulesets/Entity.h"

#include <wfmath/atlasconv.h>

using Atlas::Message::Element;

Location::Location() : m_solid(true), m_loc(NULL)
{
}

Location::Location(Entity * rf) :
            m_solid(true), m_loc(rf), m_orientation(Quaternion().identity())
{
}

Location::Location(Entity * rf, const Point3D & crds) :
            m_solid(true), m_loc(rf), m_pos(crds),
            m_orientation(Quaternion().identity())
{
}

Location::Location(Entity * rf, const Point3D& crds, const Vector3D& vel) :
            m_solid(true), m_loc(rf), m_pos(crds), m_velocity(vel),
            m_orientation(Quaternion().identity())
{
}

const Point3D Location::getXyz() const
{
    if (m_loc != 0) {
        if (m_loc->m_location.m_orientation.isValid()) {
            return m_pos.toParentCoords(m_loc->m_location.getXyz(),
                                    m_loc->m_location.m_orientation);
        } else {
            return m_pos.toParentCoords(m_loc->m_location.getXyz());
        }
    } else {
        return Point3D(0,0,0);
    }
}

void Location::addToMessage(MapType & omap) const
{
    static const Quaternion identity(Quaternion().identity());
    if (m_loc!=NULL) {
        omap["loc"] = m_loc->getId();
    }
    if (m_pos.isValid()) {
        omap["pos"] = m_pos.toAtlas();
    }
    if (m_velocity.isValid()) {
        omap["velocity"] = m_velocity.toAtlas();
    }
    if (m_orientation != identity) {
        omap["orientation"] = m_orientation.toAtlas();
    }
    if (m_bBox.isValid()) {
        omap["bbox"] = m_bBox.toAtlas();
    }
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
    } else if (distanceToAncestor(self.m_loc->m_location, other, c)) {
        if (self.m_orientation.isValid()) {
            c = c.toLocalCoords(self.m_pos, self.m_orientation);
        } else {
            static const Quaternion identity(1, 0, 0, 0);
            c = c.toLocalCoords(self.m_pos, identity);
        }
        return true;
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
