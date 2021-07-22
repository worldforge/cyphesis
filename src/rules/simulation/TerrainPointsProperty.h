// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2020 Erik Ogenvik
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


#ifndef RULESETS_TERRAIN_POINTS_PROPERTY_H
#define RULESETS_TERRAIN_POINTS_PROPERTY_H


#include "common/Property.h"

/// \brief Class to handle Entity terrain_points property.
/// This property defines the geometry of the terrain, using Mercator Basepoints.
/// It's in map format to allow updating of singular points through the Modifers system ("append" in most cases).
///
/// \ingroup PropertyClasses
class TerrainPointsProperty : public Property<Atlas::Message::MapType>
{

    public:

        static constexpr const char* property_name = "terrain_points";
        static constexpr const char* property_atlastype = "map";

        TerrainPointsProperty() = default;
        TerrainPointsProperty* copy() const override;

        void apply(LocatedEntity& entity) override;

    protected:
        TerrainPointsProperty(const TerrainPointsProperty& rhs) = default;

};

#endif // RULESETS_TERRAIN_POINTS_PROPERTY_H
