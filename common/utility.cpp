// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "utility.h"

#include <Atlas/Objects/Entity/RootEntity.h>
#include <Atlas/Objects/Operation/RootOperation.h>

using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

namespace utility {

Root * Object_asRoot(const Element & o)
{
    Root * obj;

    if (!o.isMap()) return NULL;
    const MapType & omap = o.asMap();
    MapType::const_iterator I = omap.find("objtype");
    MapType::const_iterator Iend = omap.end();
    if ((I != Iend) && (I->second.isString())) {
        if ((I->second.asString() == "object") ||
            (I->second.asString() == "obj")) {
            obj = new RootEntity;
        } else if (I->second.asString() == "op") {
            obj = new RootOperation;
        } else {
            obj = new Root;
        }
    } else {
        obj = new Root;
    }
    for (MapType::const_iterator I = omap.begin(); I != Iend; ++I) {
        obj->setAttr(I->first, I->second);
    }
    return obj;
}

bool Object_asOperation(const MapType & ent, RootOperation & op)
{
    MapType::const_iterator I = ent.find("objtype");
    MapType::const_iterator Iend = ent.end();
    if ((I == Iend) || (!I->second.isString()) ||
        (I->second.asString() != "op")) {
        return false;
    }
    for (I = ent.begin(); I != Iend; ++I) {
        op.setAttr(I->first, I->second);
    }
    return true;
}

} // namespace utility
