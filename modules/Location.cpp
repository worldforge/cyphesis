// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "rulesets/Entity.h"

using Atlas::Message::Object;

const Vector3D Location::getXyz() const
{
    if (m_loc) {
        return Vector3D(m_pos) += m_loc->getXyz();
    } else {
        return Vector3D(0,0,0);
    }
}

const Vector3D Location::getXyz(Entity * ent) const
{
    if (m_loc == ent) {
        return Vector3D(m_pos);
    } else if (m_loc == NULL) {
        return Vector3D(0,0,0);
    } else {
        return Vector3D(m_pos) += m_loc->location.getXyz(ent);
    }
}

void Location::addToObject(Object::MapType & omap) const
{
    if (m_loc!=NULL) {
        omap["loc"] = Object(m_loc->getId());
    } else {
        omap["loc"] = Object("");
    }
    if (m_pos.isValid()) {
        omap["pos"] = m_pos.asObject();
    }
    if (m_velocity.isValid()) {
        omap["velocity"] = m_velocity.asObject();
    }
    if (m_orientation.isValid()) {
        omap["orientation"] = m_orientation.asObject();
    }
    if (m_bBox.isValid()) {
        omap["bbox"] = m_bBox.asList();
    }
}

bool Location::distanceLeft(const Location & other, Vector3D & c) const {
    if (m_loc == other.m_loc) {
        c -= m_pos;
        return true;
    } else if (m_loc == NULL) {
        return false;
    } else {
        bool ret = m_loc->location.distanceLeft(other,c);
        if (ret) {
            c -= m_pos;
        }
        return ret;
    }
}

bool Location::distanceRight(const Location & other, Vector3D & c) const {
    // In an intact system, other->m_loc should never be NULL or invalid
    if (distanceLeft(other,c) || distanceRight(other.m_loc->location,c)) {
        c += other.m_pos;
        return true;
    }
    return false;
}
