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
#include <boost/optional.hpp>

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
    /// \brief The tile shader which represents all other shaders.
    Mercator::TileShader* m_tileShader;
    /// FIXME This should be a reference for consistency. Or could it
    /// even be stored in the mercator terrain entity.
    /// \brief Collection of surface data, cos I don't care!
    Atlas::Message::ListType m_surfaces;

    std::vector<std::string> m_surfaceNames;

    /// \brief Reference to variable storing the set of modified points
    PointSet m_modifiedTerrain;
    /// \brief Reference to variable storing the set of newly created points
    PointSet m_createdTerrain;

    std::vector<WFMath::AxisBox<2>> m_changedAreas;

    Mercator::TileShader* createShaders(const Atlas::Message::ListType& surfaceList);

  public:

    static constexpr const char* property_name = "terrain";
    static constexpr const char* property_atlastype = "map";

    TerrainProperty(const TerrainProperty& rhs);
    explicit TerrainProperty();

    ~TerrainProperty() override;

    int get(Atlas::Message::Element &) const override;
    void set(const Atlas::Message::Element &) override;
    TerrainProperty * copy() const override;
    void apply(LocatedEntity* entity) override;

    // Applies a Mercator::TerrainMod to the terrain
    void addMod(long id, const Mercator::TerrainMod *) const;
    // Removes all TerrainMods from a terrain segment
    void clearMods(float, float);
    // Removes a single TerrainMod from the terrain
    void updateMod(long id, const Mercator::TerrainMod *) const;
    // Removes a single TerrainMod from the terrain
    void removeMod(long id) const;

    bool getHeightAndNormal(float x, float z, float &, Vector3D &) const;
    bool getHeight(float x, float z, float &) const;
    boost::optional<int> getSurface(float x, float z) const;

    /// \brief Find the mods at a given location
    ///
    /// @param pos the x,y coordinates of a point on the terrain
    /// @param mods a reference to the list to be returned
    boost::optional<std::vector<LocatedEntity *>> findMods(float x, float z) const;

    Mercator::Terrain& getData();
    Mercator::Terrain& getData() const;

    const std::vector<std::string> getSurfaceNames() const {
        return m_surfaceNames;
    }

};

#endif // RULESETS_TERRAIN_PROPERTY_H
