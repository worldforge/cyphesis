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
    if ((o.AsMap().find("objtype") != o.AsMap().end()) &&
        ((*o.AsMap().find("objtype")).second.IsString())) {
        if ((*o.AsMap().find("objtype")).second.AsString() == "object") {
    	    obj = new RootEntity;
        } else if ((*o.AsMap().find("objtype")).second.AsString() == "op") {
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

}
