// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MODULES_LOCATION_H
#define MODULES_LOCATION_H

#include <physics/BBox.h>

class Entity;

class Location {
  public:
    Entity * ref;
    Vector3D coords;   // Coords relative to ref entity
    Vector3D velocity; // Veclociy vector, relative to ref entity.
    Vector3D face;     // Bad way of representing direction entity faces.
    BBox bBox;
    //Vector3D bbox;     // Sizes here are distance from median to edge.
    //Vector3D bmedian;  // If this is not set, then bbox is centered around bbox
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
        if (!bBox) {
            //return loc.getXyz().inBox(getXyz(), Vector3D(distance));

            return loc.getXyz().in(getXyz() - distance, getXyz() + distance);
        } else {
            // const Vector3D & median = bmedian ? bmedian : bbox;
            // return loc.getXyz().inBox(getXyz() + median, bbox + distance);

            //const Vector3D xyz = getXyz();
            //return loc.getXyz().inBox(bBox.nearPoint() + xyz,
                                      //bBox.farPoint() + xyz);

            return bBox.grow(distance).contains(loc.getXyz() - getXyz());
        }
    }

    bool inRange(const Vector3D & pos, const double distance) const {
        if (!coords) { return false; }
        if (!bBox) {
            // return pos.inBox(coords, Vector3D(distance));

            return pos.in(coords - distance, coords + distance);
        } else {
            // const Vector3D & median = bmedian ? bmedian : bbox;
            // return pos.inBox(coords + median, bbox + distance);

            //return pos.inBox(coords + bBox.nearPoint(),
                             //coords + bBox.farPoint());

            return bBox.grow(distance).contains(pos - coords);
        }
    }

    bool hit(const Location & o) const {
        if (!(bBox && o.bBox)) { return false; }
        // const Vector3D & m = bmedian ? bmedian : bbox;
        // const Vector3D & om = other.bmedian ? other.bmedian : other.bbox;
        // return coords.hitBox(m, bbox, other.coords + om, other.bbox);
        return (bBox + coords).hit(o.bBox + o.coords);
    }

    double timeToHit(const Location & o, int & axis) const {
        if (!o.bBox) { return -1; }
        if (o.velocity && (o.velocity.mag() > 0.01)) {
            // We don't currently have a viable way of making this work
            // so I am just saying that two moving entities cannot collide
            // Short term this should not be a problem
           return -1;
        }
        return (bBox + coords).timeToHit(velocity, o.bBox + o.coords, axis);
    }

    double timeToExit(const Location & o) const {
        if (!o.bBox) { return -1; }
        // It is assumed that o is the location of our current parent entity
        // so o.bBox has the same terms of reference as we do.
        return (bBox + coords).timeToExit(velocity, o.bBox);
    }

    friend std::ostream & operator<<(std::ostream& s, Location& v);
};

#endif // MODULES_LOCATION_H
