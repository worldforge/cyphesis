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
            m_solid(true), m_loc(rf)
{
}

Location::Location(Entity * rf, const Point3D & crds) :
            m_solid(true), m_loc(rf), m_pos(crds)
{
}

Location::Location(Entity * rf, const Point3D& crds, const Vector3D& vel) :
            m_solid(true), m_loc(rf), m_pos(crds), m_velocity(vel)
{
}

const Point3D Location::getXyz() const
{
    if (m_loc != 0) {
        return m_pos.toParentCoords(m_loc->m_location.getXyz(),
                                    m_loc->m_location.m_orientation);
    } else {
        return Point3D(0,0,0);
    }
}

void Location::addToMessage(MapType & omap) const
{
    if (m_loc!=NULL) {
        omap["loc"] = m_loc->getId();
    } else {
        omap["loc"] = "";
    }
    if (m_pos.isValid()) {
        omap["pos"] = m_pos.toAtlas();
    }
    if (m_velocity.isValid()) {
        omap["velocity"] = m_velocity.toAtlas();
    }
    if (m_orientation.isValid()) {
        omap["orientation"] = m_orientation.toAtlas();
    }
    if (m_bBox.isValid()) {
        omap["bbox"] = m_bBox.toAtlas();
    }
}

bool Location::distanceLeft(const Location & other, Point3D & c) const
{
    if (m_loc == other.m_loc) {
        // c = (c - m_pos).rotate(m_orientation.inVerse();
        c = c.toLocalCoords(m_pos, m_orientation);
        return true;
    } else if (m_loc == NULL) {
        return false;
    } else {
        bool ret = m_loc->m_location.distanceLeft(other,c);
        if (ret) {
            c = c.toLocalCoords(m_pos, m_orientation);
        }
        return ret;
    }
}

bool Location::distanceRight(const Location & other, Point3D & c) const
{
    // In an intact system, other->m_loc should never be NULL or invalid
    if (distanceLeft(other,c) || distanceRight(other.m_loc->m_location,c)) {
        // c = c + m_pos.rotate(m_orientation);
        c = other.m_pos.toParentCoords(c, m_orientation.inverse());
        return true;
    }
    return false;
}
