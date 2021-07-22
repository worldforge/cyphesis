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
#include <common/PropertyInstanceState.h>

#include <Mercator/Terrain.h>
#include <Mercator/TileShader.h>

#include <set>
#include <memory>
#include <boost/optional.hpp>

namespace Mercator {
    class Terrain;

    class TerrainMod;

    class TileShader;
}

/// \brief Class to handle Entity terrain property
/// This property only defines the surfaces. The separate TerrainPointsProperty defines the actual terrain geometry.
/// \ingroup PropertyClasses
class TerrainProperty : public Property<Atlas::Message::ListType>
{
    protected:

        struct State
        {
            std::unique_ptr<Mercator::Terrain> terrain;
            std::unique_ptr<Mercator::TileShader> tileShader;
            std::vector<std::string> surfaceNames;
        };

        std::pair<std::unique_ptr<Mercator::TileShader>, std::vector<std::string>> createShaders(const Atlas::Message::ListType& surfaceList) const;

        TerrainProperty(const TerrainProperty& rhs) = default;

        void applyToState(LocatedEntity& entity, State& state) const;

        static PropertyInstanceState<State> sInstanceState;

    public:

        static constexpr const char* property_name = "terrain";

        TerrainProperty() = default;

        void install(LocatedEntity& owner, const std::string& name) override;

        void remove(LocatedEntity& owner, const std::string& name) override;

        TerrainProperty* copy() const override;

        void apply(LocatedEntity& entity) override;

        bool getHeightAndNormal(LocatedEntity& entity, float x, float z, float&, Vector3D&) const;

        bool getHeight(LocatedEntity& entity, float x, float z, float&) const;

        boost::optional<int> getSurface(LocatedEntity& entity, float x, float z) const;

        /// \brief Find the mods at a given location
        ///
        /// @param pos the x,y coordinates of a point on the terrain
        /// @param mods a reference to the list to be returned
        boost::optional<std::vector<LocatedEntity*>> findMods(LocatedEntity& entity, float x, float z) const;

        Mercator::Terrain& getData(const LocatedEntity& entity);

        Mercator::Terrain& getData(const LocatedEntity& entity) const;

        const std::vector<std::string>& getSurfaceNames(const LocatedEntity& entity) const;



};

#endif // RULESETS_TERRAIN_PROPERTY_H
