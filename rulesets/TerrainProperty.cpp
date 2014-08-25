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

#include "common/BaseWorld.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/compose.hpp"
#include "common/custom.h"
#include "common/TypeNode.h"
#include "common/Nourish.h"

#include "modules/TerrainContext.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/Surface.h>
#include <Mercator/TerrainMod.h>
#include <Mercator/TileShader.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>

#include <wfmath/intersect.h>

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sstream>

#include <cassert>

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

/// \brief TerrainProperty constructor
TerrainProperty::TerrainProperty() :
      m_data(*new Mercator::Terrain(Mercator::Terrain::SHADED)),
      m_tileShader(*new Mercator::TileShader)

{
    m_tileShader.addShader(new Mercator::FillShader(), ROCK);
    m_tileShader.addShader(new Mercator::BandShader(-2.f, 1.5f), SAND);
    m_tileShader.addShader(new Mercator::GrassShader(1.f, 80.f, .5f, 1.f), GRASS);
    m_tileShader.addShader(new Mercator::DepthShader(0.f, -10.f), SILT);
    m_tileShader.addShader(new Mercator::HighShader(110.f), SNOW);
    m_data.addShader(&m_tileShader, 0);
}

TerrainProperty::~TerrainProperty()
{
    delete &m_data;
    delete &m_tileShader;
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
    Pointstore::const_iterator Iend = points.end();
    for (Pointstore::const_iterator I = points.begin(); I != Iend; ++I) {
        const Pointcolumn & pointcol = I->second;
        Pointcolumn::const_iterator J = pointcol.begin();
        Pointcolumn::const_iterator Jend = pointcol.end();
        for (; J != Jend; ++J) {
            std::stringstream key;
            key << I->first << "x" << J->first;
            ListType & point = (terrain[key.str()] = ListType(3)).asList();
            point[0] = (FloatType)(I->first);
            point[1] = (FloatType)(J->first);
            point[2] = (FloatType)(J->second.height());
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

    MapType::const_iterator I = t.find("points");
    if (I != t.end() && I->second.isMap()) {
        const MapType & points = I->second.asMap();
        MapType::const_iterator Iend = points.end();
        for (MapType::const_iterator I = points.begin(); I != Iend; ++I) {
            if (!I->second.isList()) {
                continue;
            }
            const ListType & point = I->second.asList();
            if (point.size() != 3) {
                continue;
            }

            int x = (int)point[0].asNum();
            int y = (int)point[1].asNum();

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
            
            m_data.setBasePoint(x, y, point[2].asNum());
            // FIXME Add support for roughness and falloff, as done
            // by damien in equator and FIXMEd out by me

            
        }
    }

    I = t.find("surfaces");
    if (I != t.end() && I->second.isList()) {
        m_surfaces = I->second.List();
    }

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

void TerrainProperty::addMod(const Mercator::TerrainMod *mod) const
{
    m_data.addMod(mod);
}

void TerrainProperty::updateMod(const Mercator::TerrainMod *mod) const
{
    m_data.updateMod(mod);
}

void TerrainProperty::removeMod(const Mercator::TerrainMod *mod) const
{
    m_data.removeMod(mod);
}

void TerrainProperty::clearMods(float x, float y)
{
    Mercator::Segment *s = m_data.getSegment(x,y);
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
    Mercator::Segment * s = m_data.getSegment(x, y);
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
    Mercator::Segment * segment = m_data.getSegment(x, y);
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
    Mercator::Segment * seg = m_data.getSegment(pos.x(), pos.y());
    if (seg == 0) {
        return;
    }
    const Mercator::ModList & seg_mods = seg->getMods();
    Mercator::ModList::const_iterator I = seg_mods.begin();
    Mercator::ModList::const_iterator Iend = seg_mods.end();
    for (; I != Iend; ++I) {
        const Mercator::TerrainMod * mod = *I;
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

