// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MODULES_LOCATION_H
#define MODULES_LOCATION_H

#include <physics/Vector3D.h>
#include <physics/BBox.h>
#include <physics/Quaternion.h>

#include <Atlas/Message/Element.h>

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

    friend std::ostream & operator<<(std::ostream& s, Location& v);
};

#endif // MODULES_LOCATION_H
