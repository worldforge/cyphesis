// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

// $Id: TerrainModProperty.h,v 1.3 2008-08-17 20:22:40 alriddoch Exp $

#ifndef RULESETS_TERRAINMOD_PROPERTY_H
#define RULESETS_TERRAINMOD_PROPERTY_H

#include "common/Property.h"
#include "Entity.h"
#include "TerrainProperty.h"

#include <Mercator/TerrainMod.h>

namespace Mercator {
    class Terrain;
}


/// \brief Class to handle Entity terrain modifier property
/// \ingroup PropertyClasses
class TerrainModProperty : public PropertyBase {
  protected:

    /// \brief Collection of applied terrain mod data
    Atlas::Message::MapType  m_terrainmods;

    /// \brief A pointer to the modifier returned by a call to Terrain::addMod()
    Mercator::TerrainMod *m_modptr;

    /// \brief the entity owner of this property
    Entity * m_owner;

    TerrainProperty* getTerrain();

  public:

    explicit TerrainModProperty();

    virtual bool get(Atlas::Message::Element &) const;
    virtual void set(const Atlas::Message::Element &);
    virtual void add(const std::string & key, Atlas::Message::MapType & map) const;

    Mercator::TerrainMod * getModifier();
    void setPos(Point3D);
    void setup(Entity *);

    /// \brief Constructs a Mercator::TerrainMod from Atlas data
    Mercator::TerrainMod * parseModData(const Atlas::Message::Element &);
    /// \brief Constructs a Mercator::TerrainMod from Atlas data, but uses the given position
    Mercator::TerrainMod * parseModData(const Atlas::Message::Element &, Point3D);

    /// \brief Changes a modifier's position
    void move(Entity*, Point3D);
};


#endif // RULESETS_TERRAINMOD_PROPERTY_H
