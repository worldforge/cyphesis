#include <physics/Vector3D.h>

#include <streambuf.h> // Simplest source of NULL

#include "Location.h"

Location::Location() : parent(NULL) { }

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
