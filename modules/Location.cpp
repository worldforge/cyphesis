#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include <Atlas/Message/Object.h>

#include <physics/Vector3D.h>

#include <streambuf.h> // Simplest source of NULL

#include "Location.h"

#include <common/BaseEntity.h>

Location::Location() : parent(NULL) { }

Location::Location(BaseEntity * parnt, Vector3D crds) :
        parent(parnt), coords(crds), face(1, 0, 0) { }

Location::Location(BaseEntity * parnt, Vector3D crds, Vector3D vel) :
        parent(parnt), coords(crds), velocity(vel), face(1, 0, 0) { }

Location::Location(BaseEntity * parnt, Vector3D crds, Vector3D vel, Vector3D fce) :
        parent(parnt), coords(crds), velocity(vel), face(fce) { }

bool Location::operator!() const
{
    cout << "operator! " << (parent==NULL && !coords) << endl << flush;
    return(parent==NULL && !coords);
}
#if 0
void Location::operator=(const Location& loc)
{
    cout << "operator= " << endl << flush;
    cout << "{" << loc.parent << "}" << endl << flush;
    parent=loc.parent;
    coords=loc.coords;
    velocity=loc.velocity;
    cout << "{" << parent << "}" << endl << flush;
}
#endif

using namespace Atlas;
using namespace Message;

void Location::addObject(Object * obj)
{
    Object::MapType & omap = obj->AsMap();
#if !USE_OLD_LOC
    if (parent!=NULL) {
        omap["loc"] = Object(parent->fullid);
    } else {
        omap["loc"] = Object("");
    }
    omap["pos"] = coords.asObject();
    omap["velocity"] = velocity.asObject();
    omap["face"] = face.asObject();
#else
    Object::MapType lmap;
    if (parent!=NULL) {
        lmap["ref"] = Object(parent->fullid);
    } else {
        lmap["ref"] = Object("");
    }
    lmap["coords"] = coords.asObject();
    lmap["velocity"] = velocity.asObject();
    omap["loc"] = Object(lmap);
#endif
}
