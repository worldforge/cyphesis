// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef LOCATION_H
#define LOCATION_H

#include <physics/Vector3D.h>

class BaseEntity;

class Location {
public:
    BaseEntity * ref;
    Vector3D coords;  // Coords relative to ref entity
    Vector3D velocity;// Veclociy vector, relative to ref entity.
    Vector3D face;    // Bad way of representing direction entity faces.
    Vector3D bbox;    // Sizes here are distance from median to edge.
    Vector3D bmedian; // If this is not set, then bbox is centered around bbox
                      // which is equivalient to the SW corner is at coords.

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
            return pos.inBox(coords, distance);
        } else {
            const Vector3D & median = bmedian ? bmedian : bbox;
            return pos.inBox(coords + median, bbox + distance);
        }
    }

    bool hit(const Location & other) const {
        if (!(bbox && other.bbox)) { return false; }
        const Vector3D & m = bmedian ? bmedian : bbox;
        const Vector3D & om = other.bmedian ? other.bmedian : other.bbox;
        return coords.hitBox(m, bbox, other.coords + om, other.bbox);
    }

    double hitTime(const Location & other) const {
        if (!other.bbox) { return -1; }
        const Vector3D & m = bmedian ? bmedian : bbox ? bbox : Vector3D(0,0,0);
        const Vector3D & om = other.bmedian ? other.bmedian : other.bbox;
        if (other.velocity) {
            return coords.hitTime(m, bbox, velocity, other.velocity,
                                  other.coords + om, other.bbox);
        }
        return coords.hitTime(m, bbox, velocity, other.coords + om, other.bbox);
    }

    friend ostream & operator<<(ostream& s, Location& v);
};

#endif /* LOCATION_H */
