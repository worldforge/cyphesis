// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
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

// $Id: World.cpp,v 1.117 2008-08-21 17:10:39 alriddoch Exp $

#include "World.h"

#include "TerrainProperty.h"
#include "CalendarProperty.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/inheritance.h"

#include "common/Eat.h"
#include "common/Nourish.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/TileShader.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>
#include <Mercator/Surface.h>

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

typedef enum { ROCK = 0, SAND = 1, GRASS = 2, SILT = 3, SNOW = 4} Surface;

/// \brief Constructor for the World entity
World::World(const std::string & id, long intId) :
       Identified(id, intId),
       World_parent(id, intId),
           m_terrain(*new Mercator::Terrain(Mercator::Terrain::SHADED)),
           m_tileShader(*new Mercator::TileShader)
{
    m_properties["terrain"] = new TerrainProperty(m_terrain, m_modifiedTerrain,
                                                  m_modifiedTerrain, 0);
    m_properties["calendar"] = new CalendarProperty(0);

    m_tileShader.addShader(new Mercator::FillShader(), ROCK);
    m_tileShader.addShader(new Mercator::BandShader(-2.f, 1.5f), SAND);
    m_tileShader.addShader(new Mercator::GrassShader(1.f, 80.f, .5f, 1.f), GRASS);
    m_tileShader.addShader(new Mercator::DepthShader(0.f, -10.f), SILT);
    m_tileShader.addShader(new Mercator::HighShader(110.f), SNOW);
    m_terrain.addShader(&m_tileShader, 0);
}

World::~World()
{
    delete &m_terrain;
    delete &m_tileShader;
}

/// \brief Calculate the terrain height at the given x,y coordinates
float World::getHeight(float x, float y)
{
    Mercator::Segment * s = m_terrain.getSegment(x, y);
    if (s != 0 && !s->isValid()) {
        s->populate();
    }
    return m_terrain.get(x, y);
}

/// \brief Get a number encoding the surface type at the given x,y coordinates
///
/// @param pos the x,y coordinates of the point on the terrain
/// @param material a reference to the integer to be used to store the
/// material identifier at this location.
int World::getSurface(const Point3D & pos, int & material)
{
    float x = pos.x(),
          y = pos.y();
    Mercator::Segment * segment = m_terrain.getSegment(x, y);
    if (segment == 0) {
        debug(std::cerr << "No terrain at this point" << std::endl << std::flush;);
        return -1;
    }
    if (!segment->isValid()) {
        segment->populate();
    }
    x = x - segment->getXRef();
    y = y - segment->getYRef();
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

void World::EatOperation(const Operation & op, OpVector & res)
{
    const std::string & from_id = op->getFrom();
    Entity * from = BaseWorld::instance().getEntity(from_id);
    if (from == 0) {
        log(ERROR, "World got eat op from non-existant entity.");
        return;
    }

    Point3D from_pos = relativePos(m_location, from->m_location);
    int material;
    if (getSurface(from_pos, material) != 0) {
        debug(std::cout << "no surface hit" << std::endl << std::flush;);
        return;
    }
   
    const TypeNode * from_type = from->getType();
    if (Inheritance::instance().isTypeOf(from_type, "plant")) {
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
            nour_arg->setAttr("mass", log(mass.Float() + 1));
            nourish->setArgs1(nour_arg);
            res.push_back(nourish);
        }
    } else if (Inheritance::instance().isTypeOf(from_type, "character")) {
        log(NOTICE, "Eat coming from an animal.");
        if (material == GRASS) {
            debug(std::cout << "From grass" << std::endl << std::flush;);
        }
    }
}

void World::LookOperation(const Operation & op, OpVector & res)
{
    // We must be the top level entity
    assert(m_location.m_loc == 0);
    // We must contains something, or where the hell did the look come from?
    assert(m_contains != 0);
    // Let the worldrouter know we have been looked at.

    debug(std::cout << "World::Operation(Look)" << std::endl << std::flush;);
    const std::string & from_id = op->getFrom();
    Entity * from = BaseWorld::instance().getEntity(from_id);
    if (from == 0) {
        log(ERROR, "Look op has invalid from");
        return;
    }

    // Register the entity with the world router as perceptive.
    BaseWorld::instance().addPerceptive(from);

    Sight s;

    Anonymous sarg;
    addToEntity(sarg);
    s->setArgs1(sarg);

    // FIXME integrate setting terrain with setting contains.

    if (m_contains != 0) {
        std::list<std::string> & contlist = sarg->modifyContains();
        contlist.clear();
        LocatedEntitySet::const_iterator Iend = m_contains->end();
        LocatedEntitySet::const_iterator I = m_contains->begin();
        for (; I != Iend; ++I) {
            float fromSquSize = (*I)->m_location.squareBoxSize();
            float dist = squareDistance((*I)->m_location, from->m_location);
            float view_factor = fromSquSize / dist;
            if (view_factor > consts::square_sight_factor) {
                contlist.push_back((*I)->getId());
            }
        }
        if (contlist.empty()) {
            debug(std::cout << "WARNING: contains empty." << std::endl << std::flush;);
            sarg->removeAttr("contains");
        }
    }

    s->setTo(op->getFrom());
    res.push_back(s);
}

void World::MoveOperation(const Operation & op, OpVector & res)
{
    assert(m_location.m_loc == 0);
    // Can't move the world.
}

void World::DeleteOperation(const Operation & op, OpVector & res)
{
    assert(m_location.m_loc == 0);
    // Deleting has no effect.
}
