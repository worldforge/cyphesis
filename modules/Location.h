#ifndef LOCATION_H
#define LOCATION_H

#include <physics/Vector3D.h>

#define USE_OLD_LOC 0

class BaseEntity;

class Location {
public:
    BaseEntity * parent;
    Vector3D coords;
    Vector3D velocity;
    Vector3D face;

    Location();
    Location(BaseEntity * parent, Vector3D coords);
    Location(BaseEntity * parent, Vector3D coords, Vector3D velocity);
    Location(BaseEntity * parent, Vector3D coords, Vector3D velocity, Vector3D face);

    bool operator!() const;
    //void operator=(const Location&);

    void addObject(Atlas::Message::Object *);

    friend ostream & operator<<(ostream& s, Location& v);
};

#endif /* LOCATION_H */
