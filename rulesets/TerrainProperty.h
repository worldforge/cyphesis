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

// $Id$

#ifndef RULESETS_TERRAIN_PROPERTY_H
#define RULESETS_TERRAIN_PROPERTY_H

#include "physics/Vector3D.h"

#include "common/Property.h"

#include <Mercator/TerrainMod.h>

#include <set>

namespace Mercator {
    class Terrain;
}

typedef std::map<int, std::set<int> > PointSet;

/// \brief Class to handle Entity terrain property
/// \ingroup PropertyClasses
class TerrainProperty : public PropertyBase {
  protected:
    /// \brief Reference to variable holding the value of this Property
    Mercator::Terrain & m_data;
    /// FIXME This should be a reference for consistency. Or could it
    /// even be stored in the mercator terrain entity.
    /// \brief Collection of surface data, cos I don't care!
    Atlas::Message::ListType m_surfaces;

    /// \brief Reference to variable storing the set of modified points
    PointSet m_modifiedTerrain;
    /// \brief Reference to variable storing the set of newly created points
    PointSet m_createdTerrain;
  public:
    explicit TerrainProperty(Mercator::Terrain & data, unsigned int flags);

    virtual bool get(Atlas::Message::Element &) const;
    virtual void set(const Atlas::Message::Element &);

	// Applies a Mercator::TerrainMod to the terrain
    Mercator::TerrainMod* setMod(Mercator::TerrainMod *);
	// Removes all TerrainMods from a terrain segment
    void clearMods(float, float);
	// Removes a single TerrainMod from the terrain
    void removeMod(Mercator::TerrainMod *);

    float getHeight(float x, float y);
    int getSurface(const Point3D &,  int &);
};

#endif // RULESETS_TERRAIN_PROPERTY_H
