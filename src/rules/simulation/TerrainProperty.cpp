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


#include "TerrainProperty.h"
#include "rules/LocatedEntity.h"
#include "rules/Domain.h"

#include "common/debug.h"
#include "common/TypeNode.h"

#include "modules/TerrainContext.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/Surface.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>

#include <Atlas/Objects/Anonymous.h>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;
using Atlas::Objects::Entity::Anonymous;

typedef Mercator::Terrain::Pointstore Pointstore;
typedef Mercator::Terrain::Pointcolumn Pointcolumn;
PropertyInstanceState<TerrainProperty::State> TerrainProperty::sInstanceState;


void TerrainProperty::applyToState(LocatedEntity& entity, State& state) const
{

    auto& terrain = state.terrain;

    const Pointstore& base_points = terrain.getPoints();

    int minX = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int minY = std::numeric_limits<int>::max();
    int maxY = std::numeric_limits<int>::min();

    std::vector<WFMath::AxisBox<2>> changedAreas;

    auto I = m_data.find("points");
    if (I != m_data.end() && I->second.isMap()) {
        const MapType& points = I->second.asMap();
        auto Iend = points.end();
        for (auto pointsI = points.begin(); pointsI != Iend; ++pointsI) {
            if (!pointsI->second.isList()) {
                continue;
            }
            const ListType& point = pointsI->second.asList();
            if (point.size() < 3) {
                continue;
            }
            if (!point[0].isNum() || !point[1].isNum() || !point[2].isNum()) {
                continue;
            }

            int x = (int) point[0].asNum();
            int y = (int) point[1].asNum();
            double h = point[2].asNum();
            double roughness;
            double falloff;
            if (point.size() > 3) {
                roughness = point[3].asFloat();
            } else {
                roughness = Mercator::BasePoint::ROUGHNESS;
            }
            if (point.size() > 4) {
                falloff = point[4].asFloat();
            } else {
                falloff = Mercator::BasePoint::FALLOFF;
            }

            Mercator::BasePoint bp(h, roughness, falloff);

//            auto J = base_points.find(x);
//            if (J == base_points.end() ||
//                J->second.find(y) == J->second.end()) {
//                // Newly added point.
//                m_createdTerrain[x].insert(y);
//            } else {
//                // Modified point
//                PointSet::const_iterator K = m_createdTerrain.find(x);
//                if (K == m_createdTerrain.end() ||
//                    K->second.find(y) == K->second.end()) {
//                    // Already in database
//                    m_modifiedTerrain[x].insert(y);
//                }
//                // else do nothing, as its currently waiting to be added.
//            }


            minX = std::min(minX, x);
            maxX = std::max(maxX, x);
            minY = std::min(minY, y);
            maxY = std::max(maxY, y);

            terrain.setBasePoint(x, y, bp);

        }
    }

    if (minX != std::numeric_limits<int>::max()) {
        float spacing = terrain.getSpacing();
        WFMath::Point<2> minCorner((minX * spacing) - (spacing * 0.5f), (minY * spacing) - (spacing * 0.5f));
        WFMath::Point<2> maxCorner((maxX * spacing) + (spacing * 0.5f), (maxY * spacing) + (spacing * 0.5f));
        WFMath::AxisBox<2> changedArea(minCorner, maxCorner);
        changedAreas.push_back(changedArea);
    }

    I = m_data.find("surfaces");
    if (I != m_data.end() && I->second.isList()) {
        //Only alter shader if the definition has changed.
//        if (m_surfaces != I->second.List()) {
        auto shaderResult = createShaders(I->second.List());
        if (state.tileShader) {
            terrain.removeShader(state.tileShader.get(), 0);
        }
        state.tileShader = std::move(shaderResult.first);
        state.surfaceNames = std::move(shaderResult.second);
        if (state.tileShader) {
            terrain.addShader(state.tileShader.get(), 0);
        }
//            m_surfaces = I->second.List();
//        }
    }

    if (!changedAreas.empty()) {
        Domain* domain = entity.getDomain();
        if (domain) {
            domain->refreshTerrain(changedAreas);
        }
    }

}


void TerrainProperty::install(LocatedEntity* owner, const std::string& name)
{
    auto state = std::make_unique<TerrainProperty::State>(TerrainProperty::State{Mercator::Terrain(Mercator::Terrain::SHADED), {}});

    sInstanceState.addState(owner, std::move(state));
}

void TerrainProperty::remove(LocatedEntity* owner, const std::string& name)
{
    sInstanceState.removeState(owner);
}

void TerrainProperty::apply(LocatedEntity* entity)
{
    auto* state = sInstanceState.getState(entity);
    applyToState(*entity, *state);

}

std::pair<std::unique_ptr<Mercator::TileShader>, std::vector<std::string>> TerrainProperty::createShaders(const Atlas::Message::ListType& surfaceList) const
{
    std::vector<std::string> surfaceNames;
    if (!surfaceList.empty()) {
        auto tileShader = std::make_unique<Mercator::TileShader>();
        int layer = 0;
        for (auto& surfaceElement : surfaceList) {
            if (!surfaceElement.isMap()) {
                continue;
            }
            auto& surfaceMap = surfaceElement.Map();

            auto patternI = surfaceMap.find("pattern");
            if (patternI == surfaceMap.end() || !patternI->second.isString()) {
                log(WARNING, "Surface has no 'pattern'.");
                continue;
            }

            auto nameI = surfaceMap.find("name");
            if (nameI == surfaceMap.end() || !nameI->second.isString()) {
                log(WARNING, "Surface has no 'name'.");
                continue;
            }
            surfaceNames.push_back(nameI->second.String());

            Mercator::Shader::Parameters shaderParams;
            auto paramsI = surfaceMap.find("params");
            if (paramsI != surfaceMap.end() && paramsI->second.isMap()) {
                auto params = paramsI->second.Map();
                for (auto& entry : params) {
                    if (entry.second.isNum()) {
                        shaderParams.insert(std::make_pair(entry.first, (float) entry.second.asNum()));
                    } else {
                        log(WARNING, "'terrain.shaders...params' entry must be a map of floats..");
                    }
                }
            }

            auto& pattern = patternI->second.String();
            Mercator::Shader* shader = nullptr;
            if (pattern == "fill") {
                shader = new Mercator::FillShader(shaderParams);
            } else if (pattern == "band") {
                shader = new Mercator::BandShader(shaderParams);
            } else if (pattern == "grass") {
                shader = new Mercator::GrassShader(shaderParams);
            } else if (pattern == "depth") {
                shader = new Mercator::DepthShader(shaderParams);
            } else if (pattern == "high") {
                shader = new Mercator::HighShader(shaderParams);
            }

            if (shader) {
                tileShader->addShader(shader, layer);
            } else {
                log(WARNING, String::compose("Could not recognize surface with pattern '%1'", pattern));
            }
            layer++;
        }
        return {std::move(tileShader), std::move(surfaceNames)};
    }
    return {};
}

TerrainProperty* TerrainProperty::copy() const
{
    return new TerrainProperty(*this);
}

/// \brief Return the height and normal to the surface at the given point
bool TerrainProperty::getHeightAndNormal(LocatedEntity& entity,
                                         float x,
                                         float y,
                                         float& height,
                                         Vector3D& normal) const
{
    auto& terrain = sInstanceState.getState(&entity)->terrain;
    auto s = terrain.getSegmentAtPos(x, y);
    if (s && !s->isValid()) {
        s->populate();
    }
    return terrain.getHeightAndNormal(x, y, height, normal);
}

bool TerrainProperty::getHeight(LocatedEntity& entity, float x, float y, float& height) const
{
    auto& terrain = sInstanceState.getState(&entity)->terrain;
    auto s = terrain.getSegmentAtPos(x, y);
    if (s && !s->isValid()) {
        s->populate();
    }
    return terrain.getHeight(x, y, height);
}


/// \brief Get a number encoding the surface type at the given x,z coordinates
///
/// @param pos the x,z coordinates of the point on the terrain
/// @param material a reference to the integer to be used to store the
/// material identifier at this location.
boost::optional<int> TerrainProperty::getSurface(LocatedEntity& entity, float x, float z) const
{
    auto& terrain = sInstanceState.getState(&entity)->terrain;
    Mercator::Segment* segment = terrain.getSegmentAtPos(x, z);
    if (segment == nullptr) {
        debug(std::cerr << "No terrain at this point" << std::endl << std::flush;);
        return boost::none;
    }
    if (!segment->isValid()) {
        segment->populate();
    }
    x -= segment->getXRef();
    z -= segment->getZRef();
    assert(x <= segment->getSize());
    assert(z <= segment->getSize());
    const Mercator::Segment::Surfacestore& surfaces = segment->getSurfaces();
    WFMath::Vector<3> normal;
    float height = -23;
    segment->getHeightAndNormal(x, z, height, normal);
    debug(std::cout << "At the point " << x << "," << z
                    << " of the segment the height is " << height << std::endl;
                  std::cout << "The segment has " << surfaces.size()
                            << std::endl << std::flush;);
    if (surfaces.empty()) {
        log(ERROR, "The terrain has no surface data");
        return boost::none;
    }
    Mercator::Surface& tile_surface = *surfaces.begin()->second;
    if (!tile_surface.isValid()) {
        tile_surface.populate();
    }
    return tile_surface((int) x, (int) z, 0);
}

boost::optional<std::vector<LocatedEntity*>> TerrainProperty::findMods(LocatedEntity& entity, float x, float z) const
{
    auto& terrain = sInstanceState.getState(&entity)->terrain;
    Mercator::Segment* seg = terrain.getSegmentAtPos(x, z);
    if (seg == nullptr) {
        return boost::none;
    }
    std::vector<LocatedEntity*> ret;
    auto& seg_mods = seg->getMods();
    for (auto& entry : seg_mods) {
        const Mercator::TerrainMod* mod = entry.second;
        WFMath::AxisBox<2> mod_box = mod->bbox();
        if (x > mod_box.lowCorner().x() && x < mod_box.highCorner().x() &&
            z > mod_box.lowCorner().y() && z < mod_box.highCorner().y()) {
            Mercator::Effector::Context* c = mod->context();
            if (c == nullptr) {
                log(WARNING, "Terrrain mod with no context");
                continue;
            }
            debug(std::cout << "Context has id" << c->id() << std::endl;);
            auto tc = dynamic_cast<TerrainContext*>(c);
            if (tc == nullptr) {
                log(WARNING, "Terrrain mod with non Cyphesis context");
                continue;
            }
            debug(std::cout << "Context has pointer " << tc->entity().get()
                            << std::endl;);
            ret.push_back(tc->entity().get());
        }
    }
    return ret;
}

Mercator::Terrain& TerrainProperty::getData(const LocatedEntity& entity)
{
    auto* state = sInstanceState.getState(&entity);
    return state->terrain;
}

Mercator::Terrain& TerrainProperty::getData(const LocatedEntity& entity) const
{
    auto* state = sInstanceState.getState(&entity);
    return state->terrain;
}

const std::vector<std::string>& TerrainProperty::getSurfaceNames(const LocatedEntity& entity) const
{
    return sInstanceState.getState(&entity)->surfaceNames;
}
