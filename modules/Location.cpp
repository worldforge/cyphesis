#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include <Atlas/Message/Object.h>

#include <physics/Vector3D.h>

#include <streambuf.h> // Simplest source of NULL

#include "Location.h"

#include <common/BaseEntity.h>

using Atlas::Message::Object;

void Location::addObject(Object * obj) const
{
    Object::MapType & omap = obj->AsMap();
    if (parent!=NULL) {
        omap["loc"] = Object(parent->fullid);
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
}
