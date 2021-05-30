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


#include "TerrainPointsProperty.h"
#include "rules/LocatedEntity.h"
#include "rules/Domain.h"

#include "TerrainProperty.h"

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

void TerrainPointsProperty::apply(LocatedEntity& entity)
{
    auto terrainProp = entity.modPropertyClassFixed<TerrainProperty>();
    if (terrainProp) {
        auto& terrain = terrainProp->getData(entity);

        auto& base_points = terrain.getPoints();

        int minX = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int minY = std::numeric_limits<int>::max();
        int maxY = std::numeric_limits<int>::min();

        std::vector<WFMath::AxisBox<2>> changedAreas;

        for (auto& entry : m_data) {
            auto& val = entry.second;

            if (!val.isList()) {
                continue;
            }
            const ListType& point = val.List();
            if (point.size() < 3) {
                log(WARNING, "Terrain point must be at least size 3.");
                continue;
            }
            if (!point[0].isNum() || !point[1].isNum() || !point[2].isNum()) {
                log(WARNING, "Terrain point must be all numbers.");
                continue;
            }

            int x = (int) point[0].asNum();
            int y = (int) point[1].asNum();
            double h = point[2].asNum();
            double roughness;
            double falloff;
            if (point.size() > 3) {
                if (!point[3].isNum()) {
                    log(WARNING, "Terrain point roughness must be a number.");
                    continue;
                }
                roughness = point[3].asNum();
            } else {
                roughness = Mercator::BasePoint::ROUGHNESS;
            }
            if (point.size() > 4) {
                if (!point[4].isNum()) {
                    log(WARNING, "Terrain point falloff must be a number.");
                    continue;
                }
                falloff = point[4].asNum();
            } else {
                falloff = Mercator::BasePoint::FALLOFF;
            }

            Mercator::BasePoint bp(h, roughness, falloff);

            auto J = base_points.find(x);
            if (J != base_points.end()) {
                auto K = J->second.find(y);
                if (K != J->second.end()) {
                    auto& existingPoint = K->second;
                    if (existingPoint == bp) {
                        //No change
                        continue;
                    }
                }
            }

            minX = std::min(minX, x);
            maxX = std::max(maxX, x);
            minY = std::min(minY, y);
            maxY = std::max(maxY, y);

            terrain.setBasePoint(x, y, bp);
        }


        if (minX != std::numeric_limits<int>::max()) {
            float spacing = terrain.getSpacing();
            WFMath::Point<2> minCorner(((minX - 1) * spacing), ((minY - 1) * spacing));
            WFMath::Point<2> maxCorner(((maxX + 1) * spacing), ((maxY + 1) * spacing));
            WFMath::AxisBox<2> changedArea(minCorner, maxCorner);
            changedAreas.push_back(changedArea);
        }
        if (!changedAreas.empty()) {
            Domain* domain = entity.getDomain();
            if (domain) {
                domain->refreshTerrain(changedAreas);
            }
        }
    }
}

TerrainPointsProperty* TerrainPointsProperty::copy() const
{
    return new TerrainPointsProperty(*this);
}
