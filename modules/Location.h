// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MODULES_LOCATION_H
#define MODULES_LOCATION_H

#include <physics/BBox.h>
#include <physics/Quaternion.h>

class Entity;

class Location {
  protected:
    bool distanceLeft(const Location & other, Vector3D & c) const;
    bool distanceRight(const Location & other, Vector3D & c) const;
  public:
    Entity * ref;
    Vector3D coords;   // Coords relative to ref entity
    Vector3D velocity; // Veclociy vector, relative to ref entity.
    Quaternion orientation;

    BBox bBox;
    bool solid;

    Location() : ref(NULL), solid(true) { }
    Location(Entity * rf, const Vector3D& crds) :
            ref(rf), coords(crds), solid(true) { }
    Location(Entity * rf, const Vector3D& crds, const Vector3D& vel) :
            ref(rf), coords(crds), velocity(vel), solid(true) { }

    bool isValid() const {
        return ((ref != NULL) && coords.isValid());
    }

    const Vector3D getXyz() const;
    const Vector3D getXyz(Entity *) const;

    void addToObject(Atlas::Message::Object::MapType & ent) const;

    const Vector3D distanceTo(const Location & other) const {
        Vector3D dist(0,0,0);
        if (distanceRight(other, dist)) {
           dist.set();
        } else {
           std::cerr << "DISTANCETO returned false" << std::endl << std::flush;
           return Vector3D();
        }
        return dist;
    }

    bool inRange(const Location & other, const double range) const {
        if (!bBox.isValid()) {
            //return loc.getXyz().inBox(getXyz(), Vector3D(distance));

            // return other.getXyz().in(getXyz(), range);
            return distanceTo(other).in(range);
        } else {
            // const Vector3D & median = bmedian ? bmedian : bbox;
            // return loc.getXyz().inBox(getXyz() + median, bbox + distance);

            //const Vector3D xyz = getXyz();
            //return loc.getXyz().inBox(bBox.nearPoint() + xyz,
                                      //bBox.farPoint() + xyz);

            return bBox.contains(distanceTo(other), range);
        }
    }

    bool inRange(const Vector3D & pos, const double range) const {
        if (!coords.isValid()) { return false; }
        if (!bBox.isValid()) {
            // return pos.inBox(coords, Vector3D(distance));

            return pos.in(coords, range);
        } else {
            // const Vector3D & median = bmedian ? bmedian : bbox;
            // return pos.inBox(coords + median, bbox + distance);

            //return pos.inBox(coords + bBox.nearPoint(),
                             //coords + bBox.farPoint());

            Vector3D rpos = pos;
            rpos -= coords;
            return bBox.contains(rpos, range);
        }
    }

    bool hit(const Location & o) const {
        if (!(bBox.isValid() && o.bBox.isValid())) { return false; }
        // const Vector3D & m = bmedian ? bmedian : bbox;
        // const Vector3D & om = other.bmedian ? other.bmedian : other.bbox;
        // return coords.hitBox(m, bbox, other.coords + om, other.bbox);
        BBox us(bBox), them(o.bBox);
        us += coords;
        them += o.coords;
        return us.hit(them);
    }

    double timeToHit(const Location & o, int & axis) const {
        if (!o.bBox.isValid()) { return -1; }
        if (o.velocity.isValid() && (o.velocity.relMag() > 0.0001)) {
            // We don't currently have a viable way of making this work
            // so I am just saying that two moving entities cannot collide
            // Short term this should not be a problem
            return -1;
        }
        BBox us(bBox), them(o.bBox);
        us += coords;
        them += o.coords;
        return us.timeToHit(velocity, them, axis);
    }

    double timeToExit(const Location & o) const {
        if (!o.bBox.isValid()) { return -1; }
        // It is assumed that o is the location of our current parent entity
        // so o.bBox has the same terms of reference as we do.
        BBox us(bBox);
        us += coords;
        return us.timeToExit(velocity, o.bBox);
    }

    friend std::ostream & operator<<(std::ostream& s, Location& v);
};

#endif // MODULES_LOCATION_H
