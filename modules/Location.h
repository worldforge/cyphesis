// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MODULES_LOCATION_H
#define MODULES_LOCATION_H

#include <physics/Vector3D.h>

class Entity;

class Location {
  public:
    Entity * ref;
    Vector3D coords;   // Coords relative to ref entity
    Vector3D velocity; // Veclociy vector, relative to ref entity.
    Vector3D face;     // Bad way of representing direction entity faces.
    Vector3D bbox;     // Sizes here are distance from median to edge.
    Vector3D bmedian;  // If this is not set, then bbox is centered around bbox
                       // which is equivalient to the SW corner is at coords.
    bool solid;

    Location() : ref(NULL), solid(true) { }
    Location(Entity * rf, const Vector3D& crds) :
            ref(rf), coords(crds), solid(true) { }
    Location(Entity * rf, const Vector3D& crds, const Vector3D& vel) :
            ref(rf), coords(crds), velocity(vel), solid(true) { }
    Location(Entity * rf, const Vector3D& crds, const Vector3D& vel,
                                                    const Vector3D& fce) :
            ref(rf), coords(crds), velocity(vel), face(fce), solid(true) { }

    operator bool() const {
        return (ref!=NULL && coords);
    }

    Vector3D getXyz() const;

    void addToObject(Atlas::Message::Object::MapType & ent) const;

    bool inRange(const Location & loc, const double distance) const {
        if (!bbox) {
            return loc.getXyz().inBox(getXyz(), distance);
        } else {
            const Vector3D & median = bmedian ? bmedian : bbox;
            return loc.getXyz().inBox(getXyz() + median, bbox + distance);
        }
    }

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

    double hitTime(const Location & other, int & axis) const {
        if (!other.bbox) { return -1; }
        const Vector3D & m = bmedian ? bmedian : bbox ? bbox : Vector3D(0,0,0);
        const Vector3D & om = other.bmedian ? other.bmedian : other.bbox;
        if ((other.velocity) && (other.velocity.mag() > 0.1)) {
            // We don't currently have a viable way of making this work
            // so I am just saying that two moving entities cannot collide
            // Short term this should not be a problem
            return -1;
            // return coords.hitTime(m, bbox, velocity, other.velocity,
                                  // other.coords + om, other.bbox, axis);
        }
        return coords.hitTime(m, bbox, velocity, other.coords + om, other.bbox, axis);
    }

    double inTime(const Location & other) const {
        if (!other.bbox) { return -1; }
        const Vector3D & m = bmedian ? bmedian : bbox;
        const Vector3D & om = other.bmedian ? other.bmedian : other.bbox;
        return coords.inTime(m, bbox, velocity, om, other.bbox);
    }

    friend std::ostream & operator<<(std::ostream& s, Location& v);
};

#endif // MODULES_LOCATION_H
