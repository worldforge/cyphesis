#include <Atlas/Message/Object.h>

#include <physics/Vector3D.h>

#include <streambuf.h> // Simplest source of NULL

#include "Location.h"

Location::Location()
{
    Location(NULL, Vector3D(0,0,0), Vector3D(0,0,0));
}

Location::Location(BaseEntity * parent, Vector3D coords)
{
    Location(parent, coords, Vector3D(0,0,0));
}

Location::Location(BaseEntity * parent, Vector3D coords, Vector3D velocity) :
        parent(parent), coords(coords), velocity(velocity) { }

bool Location::operator!()
{
    return(parent==NULL && coords.mag()==0);
}

using namespace Atlas;
using namespace Message;

void Location::addObject(Object * obj)
{
    Object::MapType & omap = obj->AsMap();
    omap["loc"] = Object();
    omap["pos"] = coords.asObject();
    omap["velocity"] = velocity.asObject();
}
