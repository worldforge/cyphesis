// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MODULES_LOCATION_H
#define MODULES_LOCATION_H

#include <physics/Vector3D.h>
#include <physics/BBox.h>
#include <physics/Quaternion.h>

class Entity;

class Location {
  protected:
    bool distanceLeft(const Location & other, Vector3D & c) const;
    bool distanceRight(const Location & other, Vector3D & c) const;
  public:
    Entity * m_loc;
    Vector3D m_pos;   // Coords relative to m_loc entity
    Vector3D m_velocity; // Veclociy vector, relative to m_loc entity.
    Quaternion m_orientation;

    BBox m_bBox;
    bool m_solid;

    Location() : m_loc(NULL), m_solid(true) { }
    Location(Entity * rf) :
            m_loc(rf), m_solid(true) { }
    Location(Entity * rf, const Vector3D& crds) :
            m_loc(rf), m_pos(crds), m_solid(true) { }
    Location(Entity * rf, const Vector3D& crds, const Vector3D& vel) :
            m_loc(rf), m_pos(crds), m_velocity(vel), m_solid(true) { }

    bool isValid() const {
        return ((m_loc != NULL) && m_pos.isValid());
    }

    const Vector3D getXyz() const;
    const Vector3D getXyz(Entity *) const;

    void addToObject(Atlas::Message::Element::MapType & ent) const;

    const Vector3D distanceTo(const Location & other) const {
        Vector3D dist(0,0,0);
        if (distanceRight(other, dist)) {
           dist.setValid();
        } else {
           return Vector3D();
        }
        return dist;
    }

    /// Is entity with location other within distance range of this location.
    bool inRange(const Location & other, const double range) const {
        if (!m_bBox.isValid()) {
            //return loc.getXyz().inBox(getXyz(), Vector3D(distance));

            // return other.getXyz().in(getXyz(), range);
            return in(distanceTo(other), range);
        } else {
            // const Vector3D & median = bmedian ? bmedian : bbox;
            // return loc.getXyz().inBox(getXyz() + median, bbox + distance);

            //const Vector3D xyz = getXyz();
            //return loc.getXyz().inBox(m_bBox.nearPoint() + xyz,
                                      //m_bBox.farPoint() + xyz);

            return boxContains(m_bBox, distanceTo(other), range);
        }
    }

    /// Is entity with position pos within distance range of this location.
    bool inRange(const Vector3D & pos, const double range) const {
        if (!m_pos.isValid()) { return false; }
        if (!m_bBox.isValid()) {
            // return pos.inBox(m_pos, Vector3D(distance));

            return in(pos, m_pos, range);
        } else {
            // const Vector3D & median = bmedian ? bmedian : bbox;
            // return pos.inBox(m_pos + median, bbox + distance);

            //return pos.inBox(m_pos + m_bBox.nearPoint(),
                             //m_pos + m_bBox.farPoint());

            Vector3D rpos = pos;
            rpos -= m_pos;
            return boxContains(m_bBox, rpos, range);
        }
    }

    bool hit(const Location & o) const {
        if (!(m_bBox.isValid() && o.m_bBox.isValid())) { return false; }
        // const Vector3D & m = bmedian ? bmedian : bbox;
        // const Vector3D & om = other.bmedian ? other.bmedian : other.bbox;
        // return m_pos.hitBox(m, bbox, other.m_pos + om, other.bbox);
        BBox us(m_bBox), them(o.m_bBox);
        us.shift(m_pos);
        them.shift(o.m_pos);
        return ::hit(us, them);
    }

    double timeToHit(const Location & o, int & axis) const {
        if (!o.m_bBox.isValid()) { return -1; }
        if (o.m_velocity.isValid() && (o.m_velocity.sqrMag() > 0.0001)) {
            // We don't currently have a viable way of making this work
            // so I am just saying that two moving entities cannot collide
            // Short term this should not be a problem
            return -1;
        }
        BBox us(m_bBox), them(o.m_bBox);
        us.shift(m_pos);
        them.shift(o.m_pos);
        return ::timeToHit(us, m_velocity, them, axis);
    }

    double timeToExit(const Location & o) const {
        if (!o.m_bBox.isValid()) { return -1; }
        // It is assumed that o is the location of our current parent entity
        // so o.m_bBox has the same terms of reference as we do.
        BBox us(m_bBox);
        us.shift(m_pos);
        return ::timeToExit(us, m_velocity, o.m_bBox);
    }

    friend std::ostream & operator<<(std::ostream& s, Location& v);
};

#endif // MODULES_LOCATION_H
