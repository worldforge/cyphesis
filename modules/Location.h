// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef LOCATION_H
#define LOCATION_H

#include <physics/Vector3D.h>

class BaseEntity;

typedef enum sphape {
    SH_C_BOX,		// Box with coords at corner
    SH_M_BOX,		// Box with coords at centre
    SH_M_CYL		// Cylinder with coords at centre
} shape_t;

class Location {
public:
    BaseEntity * ref;
    Vector3D coords;
    Vector3D velocity;
    Vector3D face;
    Vector3D bbox;
    shape_t  shape;

    Location() : ref(NULL) { }
    Location(BaseEntity * rf, const Vector3D& crds) :
            ref(rf), coords(crds) { }
    Location(BaseEntity * rf, const Vector3D& crds, const Vector3D& vel) :
            ref(rf), coords(crds), velocity(vel) { }
    Location(BaseEntity * rf, const Vector3D& crds, const Vector3D& vel, const Vector3D& fce) :
            ref(rf), coords(crds), velocity(vel), face(fce) { }

    operator bool() const {
        return(ref!=NULL && coords);
    }

    void addObject(Atlas::Message::Object * obj) const;

    bool inRange(const Vector3D & pos, const double distance) const {
        if (!coords) { return false; }
        if (!bbox) {
            return ((coords.X() < (pos.X() + distance))
                   && (coords.X() > (pos.X() - distance))
                   && (coords.Y() < (pos.Y() + distance))
                   && (coords.Y() > (pos.Y() - distance)));
        } else if (shape == SH_M_BOX) {
            // We need a better check in this case. A true AABB interference
            // detection algorithm is needed, but I am too tired to work
            // one out
            return ((coords.X() < (pos.X() + distance))
                   && (coords.X() > (pos.X() - distance))
                   && (coords.Y() < (pos.Y() + distance))
                   && (coords.Y() > (pos.Y() - distance)));
        } else {
            // We need a better check in this case. A true AABB interference
            // detection algorithm is needed, but I am too tired to work
            // one out
            return ((coords.X() < (pos.X() + distance))
                   && (coords.X() > (pos.X() - distance))
                   && (coords.Y() < (pos.Y() + distance))
                   && (coords.Y() > (pos.Y() - distance)));
        }
    }

    friend ostream & operator<<(ostream& s, Location& v);
};

#endif /* LOCATION_H */
