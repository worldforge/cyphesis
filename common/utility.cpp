// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Entity/RootEntity.h>
#include <Atlas/Objects/Operation/Login.h>

#include "utility.h"

namespace utility {

Root * Object_asRoot(const Atlas::Message::Object & o)
{
    Root * obj;

    if (!o.IsMap()) return NULL;
    Object::MapType::const_iterator I = o.AsMap().find("objtype");
    if ((I != o.AsMap().end()) &&
        (I->second.IsString())) {
        if (I->second.AsString() == "object") {
    	    obj = new RootEntity;
        } else if (I->second.AsString() == "op") {
    	    obj = new RootOperation;
        } else {
            obj = new Root;
        }
    } else {
        obj = new Root;
    }
    for (Object::MapType::const_iterator I = o.AsMap().begin();
            I != o.AsMap().end(); I++) {
        obj->SetAttr(I->first, I->second);
    }
    return obj;
}

} // namespace utility
