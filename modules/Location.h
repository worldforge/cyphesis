#ifndef LOCATION_H
#define LOCATION_H

#include <physics/Vector3D.h>

class BaseEntity;

class Location {
public:
    BaseEntity * parent;
    Vector3D coords;
    Vector3D velocity;

    Location();
    Location(BaseEntity * parent, Vector3D coords);
    Location(BaseEntity * parent, Vector3D coords, Vector3D velocity);

    bool operator!();
};

#endif /* LOCATION_H */
