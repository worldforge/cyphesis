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


#ifndef RULESETS_TERRAIN_PROPERTY_H
#define RULESETS_TERRAIN_PROPERTY_H

#include "physics/Vector3D.h"

#include "common/Property.h"

#include <set>

namespace Mercator {
    class Terrain;
    class TerrainMod;
    class TileShader;
}

typedef std::map<int, std::set<int> > PointSet;

/// \brief Class to handle Entity terrain property
/// \ingroup PropertyClasses
class TerrainProperty : public PropertyBase {
  protected:
    /// \brief Reference to variable holding the value of this Property
    Mercator::Terrain & m_data;
    /// \brief Reference to a variable holding the tile shader
    Mercator::TileShader & m_tileShader;
    /// FIXME This should be a reference for consistency. Or could it
    /// even be stored in the mercator terrain entity.
    /// \brief Collection of surface data, cos I don't care!
    Atlas::Message::ListType m_surfaces;

    /// \brief Reference to variable storing the set of modified points
    PointSet m_modifiedTerrain;
    /// \brief Reference to variable storing the set of newly created points
    PointSet m_createdTerrain;
  public:
    TerrainProperty();
    virtual ~TerrainProperty();

    virtual int get(Atlas::Message::Element &) const;
    virtual void set(const Atlas::Message::Element &);
    virtual TerrainProperty * copy() const;

    // Applies a Mercator::TerrainMod to the terrain
    void addMod(const Mercator::TerrainMod *) const;
    // Removes all TerrainMods from a terrain segment
    void clearMods(float, float);
    // Removes a single TerrainMod from the terrain
    void updateMod(const Mercator::TerrainMod *) const;
    // Removes a single TerrainMod from the terrain
    void removeMod(const Mercator::TerrainMod *) const;

    bool getHeightAndNormal(float x, float y, float &, Vector3D &) const;
    int getSurface(const Point3D &,  int &);

    void findMods(const Point3D &, std::vector<LocatedEntity *> &);
};

#endif // RULESETS_TERRAIN_PROPERTY_H
