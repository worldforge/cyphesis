// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <rulesets/Entity.h>

using Atlas::Message::Object;

const Vector3D Location::getXyz() const
{
    if (ref) {
        return Vector3D(coords) += ref->getXyz();
    } else {
        return Vector3D(0,0,0);
    }
}

const Vector3D Location::getXyz(Entity * ent) const
{
    if (ref == ent) {
        return Vector3D(coords);
    } else if (ref == NULL) {
        return Vector3D(0,0,0);
    } else {
        return Vector3D(coords) += ref->location.getXyz(ent);
    }
}

void Location::addToObject(Object::MapType & omap) const
{
    if (ref!=NULL) {
        omap["loc"] = Object(ref->getId());
    } else {
        omap["loc"] = Object("");
    }
    if (coords) {
        omap["pos"] = coords.asObject();
    }
    if (velocity) {
        omap["velocity"] = velocity.asObject();
    }
    if (orientation) {
        omap["orientation"] = orientation.asObject();
    }
    if (bBox) {
        omap["bbox"] = bBox.asList();
    }
}

bool Location::distanceLeft(const Location & other, Vector3D & c) const {
    if (ref == other.ref) {
        c -= coords;
        return true;
    } else if (ref == NULL) {
        return false;
    } else {
        bool ret = ref->location.distanceLeft(other,c);
        if (ret) {
            c -= coords;
        }
        return ret;
    }
}

bool Location::distanceRight(const Location & other, Vector3D & c) const {
    // In an intact system, other->ref should never be NULL or invalid
    if (distanceLeft(other,c) || distanceRight(other.ref->location,c)) {
        c += other.coords;
        return true;
    }
    return false;
}
