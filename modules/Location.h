#ifndef LOCATION_H
#define LOCATION_H

#include <physics/Vector3D.h>

class BaseEntity;

class Location {
public:
    BaseEntity * parent;
    Vector3D coords;
    Vector3D velocity;
    Vector3D face;

    Location() : parent(NULL) { }
    Location(BaseEntity * parnt, const Vector3D& crds) :
            parent(parnt), coords(crds) { }
    Location(BaseEntity * parnt, const Vector3D& crds, const Vector3D& vel) :
            parent(parnt), coords(crds), velocity(vel) { }
    Location(BaseEntity * parnt, const Vector3D& crds, const Vector3D& vel, const Vector3D& fce) :
            parent(parnt), coords(crds), velocity(vel), face(fce) { }

    operator bool() const {
        return(parent!=NULL && coords);
    }

    void addObject(Atlas::Message::Object * obj) const;

    friend ostream & operator<<(ostream& s, Location& v);
};

#endif /* LOCATION_H */
