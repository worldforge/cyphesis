// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifndef MODULES_LOCATION_H
#define MODULES_LOCATION_H

#include "physics/Vector3D.h"
#include "physics/BBox.h"
#include "physics/Quaternion.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <wfmath/axisbox.h>
#include <wfmath/point.h>
#include <wfmath/quaternion.h>

class LocatedEntity;

class Location {
  protected:
    bool m_simple;
    bool m_solid;

    double m_timeStamp;

    float m_boxSize; // Diagonal length across box
    float m_squareBoxSize;

    float m_radius; // Radius of bounding sphere of box
    float m_squareRadius;
  public:
    LocatedEntity * m_loc;
    Point3D m_pos;   // Coords relative to m_loc entity
    Vector3D m_velocity; // Veclociy vector, relative to m_loc entity.
    Quaternion m_orientation;

    BBox m_bBox;

    Location();
    explicit Location(LocatedEntity * rf);
    explicit Location(LocatedEntity * rf,
                      const Point3D & pos);
    explicit Location(LocatedEntity * rf,
                      const Point3D & pos,
                      const Vector3D & velocity);

    float boxSize() const { return m_boxSize; }
    float squareBoxSize() const { return m_squareBoxSize; }

    float radius() const { return m_radius; }
    float squareRadius() const { return m_squareRadius; }

    const Point3D & pos() const { return m_pos; }
    const Vector3D & velocity() const { return m_velocity; }
    const Quaternion & orientation() const { return m_orientation; }
    const BBox & bBox() const { return m_bBox; }

    bool isValid() const {
        return ((m_loc != NULL) && m_pos.isValid());
    }

    bool isSimple() const {
        return m_simple;
    }

    void setSimple(bool c = true) {
        m_simple = c;
    }

    bool isSolid() const {
        return m_solid;
    }

    void setSolid(bool s = true) {
        m_solid = s;
    }

    const double & timeStamp() const {
        return m_timeStamp;
    }

    void update(const double & time) {
        m_timeStamp = time;
    }

    void setBBox(const BBox & b) {
        m_bBox = b;
        modifyBBox();
    }

    void addToMessage(Atlas::Message::MapType & ent) const;
    void addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const;
    const Atlas::Objects::Root asEntity() const;

    int readFromMessage(const Atlas::Message::MapType & ent);
    int readFromEntity(const Atlas::Objects::Entity::RootEntity & ent);
    void modifyBBox();
    void setVisibility(float v);

    friend std::ostream & operator<<(std::ostream& s, Location& v);
};

const Vector3D distanceTo(const Location & self, const Location & other);

const Point3D relativePos(const Location & self, const Location & other);

float squareDistance(const Location & self, const Location & other);
float squareHorizontalDistance(const Location & self, const Location & other);

inline float distance(const Location & self, const Location & other)
{
    return std::sqrt(squareDistance(self, other));
}

#endif // MODULES_LOCATION_H
