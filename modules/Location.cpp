// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <rulesets/Entity.h>

using Atlas::Message::Object;

Vector3D Location::getXyz() const {
    if (ref && coords) {
        return coords + ref->getXyz();
    } else {
        return Vector3D(0,0,0);
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
