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
#include "LocatedEntity.h"
#include "Domain.h"

#include "common/BaseWorld.h"
#include "common/debug.h"
#include "common/custom.h"
#include "common/TypeNode.h"
#include "common/Nourish.h"

#include "modules/TerrainContext.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/Surface.h>
#include <Mercator/TileShader.h>
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
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Entity::Anonymous;

typedef Mercator::Terrain::Pointstore Pointstore;
typedef Mercator::Terrain::Pointcolumn Pointcolumn;

typedef enum { ROCK = 0, SAND = 1, GRASS = 2, SILT = 3, SNOW = 4} Surface;

TerrainProperty::TerrainProperty(const TerrainProperty& rhs) :
    m_data(*new Mercator::Terrain(Mercator::Terrain::SHADED)),
    m_tileShader(nullptr)
{
    //Copy all points.
    for (auto& pointColumn : rhs.m_data.getPoints()) {
        for (auto& point : pointColumn.second) {
            m_data.setBasePoint(pointColumn.first, point.first, point.second);
        }
    }

    //Copy surface if available, as well as surface data.
    if (!rhs.m_surfaces.empty()) {
        m_surfaces = rhs.m_surfaces;
        m_tileShader = createShaders(m_surfaces);
        m_data.addShader(m_tileShader, 0);
    }
}

/// \brief TerrainProperty constructor
TerrainProperty::TerrainProperty() :
      m_data(*new Mercator::Terrain(Mercator::Terrain::SHADED)),
      m_tileShader(nullptr)

{
}

TerrainProperty::~TerrainProperty()
{
    delete &m_data;
    delete m_tileShader;
}

void TerrainProperty::install(LocatedEntity *owner, const std::string &name)
{
    owner->installDelegate(Atlas::Objects::Operation::EAT_NO, name);
}

void TerrainProperty::remove(LocatedEntity *owner, const std::string & name)
{
    owner->removeDelegate(Atlas::Objects::Operation::EAT_NO, name);
}

int TerrainProperty::get(Element & ent) const
{
    MapType & t = (ent = MapType()).asMap();
    MapType & terrain = (t["points"] = MapType()).asMap();

    const Pointstore & points = m_data.getPoints();
    for (const auto& column : points) {
        for (const auto point : column.second) {
            const Mercator::BasePoint& bp = point.second;
            std::stringstream key;
            key << column.first << "x" << point.first;
            size_t size = 3;
            bool sendRoughness = false;
            bool sendFalloff = false;
            if (bp.falloff() != Mercator::BasePoint::FALLOFF) {
                size = 5;
                sendRoughness = true;
                sendFalloff = true;
            } else if (bp.roughness() != Mercator::BasePoint::ROUGHNESS) {
                size = 4;
                sendRoughness = true;
            }
            ListType & pointElem = (terrain[key.str()] = ListType(size)).List();
            pointElem[0] = (FloatType)(column.first);
            pointElem[1] = (FloatType)(point.first);
            pointElem[2] = (FloatType)(bp.height());
            if (sendRoughness) {
                pointElem[3] = bp.roughness();
            }
            if (sendFalloff) {
                pointElem[4] = bp.falloff();
            }
        }
    }

    t["surfaces"] = m_surfaces;
    return 0;
}

void TerrainProperty::set(const Element & ent)
{
    if (!ent.isMap()) {
        return;
    }
    const MapType & t = ent.asMap();
    debug(std::cout << "TerrainProperty::setTerrain()"
                    << std::endl << std::flush;);

    const Pointstore & base_points = m_data.getPoints();

    int minX = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int minY = std::numeric_limits<int>::max();
    int maxY = std::numeric_limits<int>::min();

    MapType::const_iterator I = t.find("points");
    if (I != t.end() && I->second.isMap()) {
        const MapType & points = I->second.asMap();
        MapType::const_iterator Iend = points.end();
        for (MapType::const_iterator I = points.begin(); I != Iend; ++I) {
            if (!I->second.isList()) {
                continue;
            }
            const ListType & point = I->second.asList();
            if (point.size() < 3) {
                continue;
            }
            if (!point[0].isNum() || !point[1].isNum() || !point[2].isNum()) {
                continue;
            }

            int x = (int)point[0].asNum();
            int y = (int)point[1].asNum();
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

            Pointstore::const_iterator J = base_points.find(x);
            if (J == base_points.end() ||
                J->second.find(y) == J->second.end()) {
                // Newly added point.
                m_createdTerrain[x].insert(y);
            } else {
                // Modified point
                PointSet::const_iterator K = m_createdTerrain.find(x);
                if (K == m_createdTerrain.end() ||
                    K->second.find(y) == K->second.end()) {
                    // Already in database
                    m_modifiedTerrain[x].insert(y);
                }
                // else do nothing, as its currently waiting to be added.
            }
            

            minX = std::min(minX, x);
            maxX = std::max(maxX, x);
            minY = std::min(minY, y);
            maxY = std::max(maxY, y);

            m_data.setBasePoint(x, y, bp);

        }
    }

    if (minX != std::numeric_limits<int>::max()) {
        float spacing = m_data.getSpacing();
        WFMath::Point<2> minCorner((minX * spacing) - (spacing * 0.5f), (minY * spacing)  - (spacing * 0.5f));
        WFMath::Point<2> maxCorner((maxX * spacing) + (spacing * 0.5f), (maxY * spacing) + (spacing * 0.5f));
        WFMath::AxisBox<2> changedArea(minCorner, maxCorner);
        m_changedAreas.push_back(changedArea);
    }

    I = t.find("surfaces");
    if (I != t.end() && I->second.isList()) {
        //Only alter shader if the definition has changed.
        if (m_surfaces != I->second.List()) {
            auto shader = createShaders(I->second.List());
            if (m_tileShader) {
                m_data.removeShader(m_tileShader, 0);
                delete m_tileShader;
            }
            m_tileShader = shader;
            if (shader) {
                m_data.addShader(shader, 0);
            }
            m_surfaces = I->second.List();
        }
    }

}

void TerrainProperty::apply(LocatedEntity* entity) {

    if (!m_changedAreas.empty()) {
        Domain* domain = entity->getDomain();
        if (domain) {
            domain->refreshTerrain(m_changedAreas);
            m_changedAreas.clear();
        }
    }
}

Mercator::TileShader* TerrainProperty::createShaders(const Atlas::Message::ListType& surfaceList) {
    if (!surfaceList.empty()) {
        Mercator::TileShader* tileShader = new Mercator::TileShader();
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
            const std::string& name = nameI->second.String();


            int layer;
            if (name == "rock") {
                layer = ROCK;
            } else if (name == "sand") {
                layer = SAND;
            } else if (name == "grass") {
                layer = GRASS;
            } else if (name == "silt") {
                layer = SILT;
            } else if (name == "snow") {
                layer = SNOW;
            } else {
                log(WARNING, String::compose("Could not recognize surface with layer with name '%1'", name));
                continue;
            }



            Mercator::Shader::Parameters shaderParams;
            auto paramsI = surfaceMap.find("params");
            if (paramsI != surfaceMap.end() && paramsI->second.isMap()) {
                auto params = paramsI->second.Map();
                for (auto& entry : params) {
                    if (entry.second.isNum()) {
                        shaderParams.insert(std::make_pair(entry.first, (float)entry.second.asNum()));
                    } else {
                        log(WARNING, "'terrain.shaders...params' entry must be a map of floats..");
                    }
                }
            }

            auto& pattern = patternI->second.String();
            if (pattern == "fill") {
                tileShader->addShader(new Mercator::FillShader(shaderParams), layer);
            } else if (pattern == "band") {
                tileShader->addShader(new Mercator::BandShader(shaderParams), layer);
            } else if (pattern == "grass") {
                tileShader->addShader(new Mercator::GrassShader(shaderParams), layer);
            } else if (pattern == "depth") {
                tileShader->addShader(new Mercator::DepthShader(shaderParams), layer);
            } else if (pattern == "high") {
                tileShader->addShader(new Mercator::HighShader(shaderParams), layer);
            } else {
                log(WARNING, String::compose("Could not recognize surface with pattern '%1'", pattern));
                continue;
            }
        }
        return tileShader;
    }
    return nullptr;
}

TerrainProperty * TerrainProperty::copy() const
{
    return new TerrainProperty(*this);
}

HandlerResult TerrainProperty::operation(LocatedEntity * e,
        const Operation & op, OpVector & res)
{
    return eat_handler(e, op, res);
}

void TerrainProperty::addMod(long id, const Mercator::TerrainMod *mod) const
{
    m_data.updateMod(id, mod);
}

void TerrainProperty::updateMod(long id, const Mercator::TerrainMod *mod) const
{
    m_data.updateMod(id, mod);
}

void TerrainProperty::removeMod(long id) const
{
    m_data.updateMod(id, nullptr);
}

void TerrainProperty::clearMods(float x, float y)
{
    Mercator::Segment *s = m_data.getSegmentAtPos(x,y);
    if(s != NULL) {
        s->clearMods();
        //log(INFO, "Mods cleared!");
    } 
}

/// \brief Return the height and normal to the surface at the given point
bool TerrainProperty::getHeightAndNormal(float x,
                                         float y,
                                         float & height,
                                         Vector3D & normal) const
{
    Mercator::Segment * s = m_data.getSegmentAtPos(x, y);
    if (s != 0 && !s->isValid()) {
        s->populate();
    }
    return m_data.getHeightAndNormal(x, y, height, normal);
}

/// \brief Get a number encoding the surface type at the given x,y coordinates
///
/// @param pos the x,y coordinates of the point on the terrain
/// @param material a reference to the integer to be used to store the
/// material identifier at this location.
int TerrainProperty::getSurface(const Point3D & pos, int & material)
{
    float x = pos.x(),
          y = pos.y();
    Mercator::Segment * segment = m_data.getSegmentAtPos(x, y);
    if (segment == 0) {
        debug(std::cerr << "No terrain at this point" << std::endl << std::flush;);
        return -1;
    }
    if (!segment->isValid()) {
        segment->populate();
    }
    x -= segment->getXRef();
    y -= segment->getYRef();
    assert(x <= segment->getSize());
    assert(y <= segment->getSize());
    const Mercator::Segment::Surfacestore & surfaces = segment->getSurfaces();
    WFMath::Vector<3> normal;
    float height = -23;
    segment->getHeightAndNormal(x, y, height, normal);
    debug(std::cout << "At the point " << x << "," << y
                    << " of the segment the height is " << height << std::endl;
          std::cout << "The segment has " << surfaces.size()
                    << std::endl << std::flush;);
    if (surfaces.size() == 0) {
        log(ERROR, "The terrain has no surface data");
        return -1;
    }
    Mercator::Surface & tile_surface = *surfaces.begin()->second;
    if (!tile_surface.isValid()) {
        tile_surface.populate();
    }
    material = tile_surface((int)x, (int)y, 0);
    return 0;
}

/// \brief Find the mods at a given location
///
/// @param pos the x,y coordinates of a point on the terrain
/// @param mods a reference to the list to be returned
void TerrainProperty::findMods(const Point3D & pos,
                               std::vector<LocatedEntity *> & ret)
{
    Mercator::Segment * seg = m_data.getSegmentAtPos(pos.x(), pos.y());
    if (seg == 0) {
        return;
    }
    auto& seg_mods = seg->getMods();
    for (auto& entry : seg_mods) {
        const Mercator::TerrainMod * mod = entry.second;
        WFMath::AxisBox<2> mod_box = mod->bbox();
        if (pos.x() > mod_box.lowCorner().x() && pos.x() < mod_box.highCorner().x() &&
            pos.y() > mod_box.lowCorner().y() && pos.y() < mod_box.highCorner().y()) {
            Mercator::Effector::Context * c = mod->context();
            if (c == 0) {
                log(WARNING, "Terrrain mod with no context");
                continue;
            }
            debug(std::cout << "Context has id" << c->id() << std::endl;);
            TerrainContext * tc = dynamic_cast<TerrainContext *>(c);
            if (tc == 0) {
                log(WARNING, "Terrrain mod with non Cyphesis context");
                continue;
            }
            debug(std::cout << "Context has pointer " << tc->entity().get()
                            << std::endl;);
            ret.push_back(tc->entity().get());
        }
    }
}

HandlerResult TerrainProperty::eat_handler(LocatedEntity * e,
        const Operation & op, OpVector & res)
{
    const std::string & from_id = op->getFrom();
    LocatedEntity * from = BaseWorld::instance().getEntity(from_id);
    if (from == 0) {
        log(ERROR, String::compose("Terrain got eat op from non-existant "
                                   "entity %1.", from_id));
        return OPERATION_IGNORED;
    }


    Point3D from_pos = relativePos(e->m_location, from->m_location);
    int material;
    if (getSurface(from_pos, material) != 0) {
        debug(std::cout << "no surface hit" << std::endl << std::flush;);
        return OPERATION_IGNORED;
    }

    const TypeNode * from_type = from->getType();
    if (from_type->isTypeOf("plant")) {
        if (material == GRASS) {
            debug(std::cout << "From grass" << std::endl << std::flush;);
            Nourish nourish;
            nourish->setTo(from_id);
            Anonymous nour_arg;
            Element mass;
            from->getAttr("mass", mass);
            if (!mass.isFloat()) {
                mass = 0.;
            }
            // FIXME to do this right we need to know how long since the
            // last tick, so the from entity needs to tell us.
            nour_arg->setAttr("mass",
                              std::pow(mass.Float(), 0.5) /
                                      (60.0 * 24.0));
            nourish->setArgs1(nour_arg);
            res.push_back(nourish);
        }
    } else if (from_type->isTypeOf("character")) {
        log(NOTICE, "Eat coming from an animal.");
        if (material == GRASS) {
            debug(std::cout << "From grass" << std::endl << std::flush;);
        }
    }

    return OPERATION_HANDLED;
}

Mercator::Terrain& TerrainProperty::getData()
{
    return m_data;
}

Mercator::Terrain& TerrainProperty::getData() const
{
    return m_data;
}


