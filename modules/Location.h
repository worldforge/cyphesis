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
  private:
    bool m_solid;
  public:
    Entity * m_loc;
    Point3D m_pos;   // Coords relative to m_loc entity
    Vector3D m_velocity; // Veclociy vector, relative to m_loc entity.
    Quaternion m_orientation;

    BBox m_bBox;

    Location();
    explicit Location(Entity * rf);
    explicit Location(Entity * rf, const Point3D & crds);
    explicit Location(Entity * rf, const Point3D & crds, const Vector3D & vel);

    bool isValid() const {
        return ((m_loc != NULL) && m_pos.isValid());
    }

    bool isSolid() const {
        return m_solid;
    }

    void setSolid(bool s = true) {
        m_solid = s;
    }

    const Point3D getXyz() const;
    const Point3D getXyz(Entity *) const;

    void addToMessage(Atlas::Message::MapType & ent) const;

    friend std::ostream & operator<<(std::ostream& s, Location& v);
};

const Point3D relativePos(const Location & self, const Location & other);

const float squareDistance(const Location & self, const Location & other);

inline const float distance(const Location & self, const Location & other)
{
    return sqrt(squareDistance(self, other));
}

#endif // MODULES_LOCATION_H
