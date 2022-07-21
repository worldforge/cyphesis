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
#include <Mercator/ShaderFactory.h>

#include <Atlas/Objects/Anonymous.h>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;
using Atlas::Objects::Entity::Anonymous;

PropertyInstanceState<TerrainProperty::State> TerrainProperty::sInstanceState;


void TerrainProperty::applyToState(LocatedEntity& entity, State& state) const
{

    auto& terrain = *state.terrain;

    auto shaderResult = createShaders(m_data);
    if (state.tileShader) {
        terrain.removeShader(state.tileShader.get(), 0);
    }
    state.tileShader = std::move(shaderResult.first);
    state.surfaceNames = std::move(shaderResult.second);
    if (state.tileShader) {
        terrain.addShader(state.tileShader.get(), 0);
    }

}


void TerrainProperty::install(LocatedEntity& owner, const std::string& name)
{
    auto state = std::make_unique<TerrainProperty::State>(TerrainProperty::State{std::make_unique<Mercator::Terrain>(Mercator::Terrain::SHADED), {}});

    sInstanceState.addState(owner, std::move(state));
}

void TerrainProperty::remove(LocatedEntity& owner, const std::string& name)
{
    sInstanceState.removeState(owner);
}

void TerrainProperty::apply(LocatedEntity& entity)
{
    auto* state = sInstanceState.getState(entity);
    applyToState(entity, *state);

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
                        shaderParams.emplace(entry.first, (float) entry.second.asNum());
                    } else {
                        log(WARNING, "'terrain.shaders...params' entry must be a map of floats..");
                    }
                }
            }

            auto& pattern = patternI->second.String();
            auto shader = Mercator::ShaderFactories().newShader(pattern, shaderParams);

            if (shader) {
                tileShader->addShader(std::move(shader), layer);
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
    auto& terrain = *sInstanceState.getState(entity)->terrain;
    auto s = terrain.getSegmentAtPos(x, y);
    if (s && !s->isValid()) {
        s->populate();
    }
    return terrain.getHeightAndNormal(x, y, height, normal);
}

bool TerrainProperty::getHeight(LocatedEntity& entity, float x, float y, float& height) const
{
    auto& terrain = *sInstanceState.getState(entity)->terrain;
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
    auto& terrain = *sInstanceState.getState(entity)->terrain;
    auto segment = terrain.getSegmentAtPos(x, z);
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
    auto& surfaces = segment->getSurfaces();
    WFMath::Vector<3> normal;
    float height = -23;
    segment->getHeightAndNormal(x, z, height, normal);
    debug_print("At the point " << x << "," << z
                    << " of the segment the height is " << height << std::endl;
                  std::cout << "The segment has " << surfaces.size());
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
    auto& terrain = *sInstanceState.getState(entity)->terrain;
    auto seg = terrain.getSegmentAtPos(x, z);
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
            debug_print("Context has id" << c->m_id);
            auto tc = static_cast<TerrainContext*>(c);
            debug_print("Context has pointer " << tc->m_entity.get());
            ret.push_back(tc->m_entity.get());
        }
    }
    return ret;
}

Mercator::Terrain& TerrainProperty::getData(const LocatedEntity& entity)
{
    auto* state = sInstanceState.getState(entity);
    return *state->terrain;
}

Mercator::Terrain& TerrainProperty::getData(const LocatedEntity& entity) const
{
    auto* state = sInstanceState.getState(entity);
    return *state->terrain;
}

const std::vector<std::string>& TerrainProperty::getSurfaceNames(const LocatedEntity& entity) const
{
    return sInstanceState.getState(entity)->surfaceNames;
}
