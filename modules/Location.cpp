// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include <Atlas/Message/Object.h>

#include <physics/Vector3D.h>

#include <streambuf.h> // Simplest source of NULL

#include "Location.h"

#include <rulesets/Entity.h>

using Atlas::Message::Object;

Vector3D Location::getXyz() const {
    if (ref && coords) {
        return coords + ref->getXyz();
    } else {
        return Vector3D(0,0,0);
    }
}

void Location::addToObject(Object & obj) const
{
    Object::MapType & omap = obj.AsMap();
    if (ref!=NULL) {
        omap["loc"] = Object(ref->fullid);
    } else {
        omap["loc"] = Object("");
    }
    if (coords) {
        omap["pos"] = coords.asObject();
    }
    if (velocity) {
        omap["velocity"] = velocity.asObject();
    }
    if (face) {
        omap["face"] = face.asObject();
    }
    if (bbox) {
        omap["bbox"] = bbox.asObject();
    }
}
