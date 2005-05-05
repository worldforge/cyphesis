// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "World.h"

#include "TerrainProperty.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/TileShader.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>
#include <Mercator/Surface.h>

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation/Sight.h>

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;
using Atlas::Objects::Operation::Sight;

typedef enum { ROCK = 0, SAND = 1, GRASS = 2, MUD = 3, SNOW = 4} Surface;

/// \brief Constructor for the World entity
World::World(const std::string & id) : World_parent(id),
             m_terrain(*new Mercator::Terrain(Mercator::Terrain::SHADED))
{
    subscribe("set", OP_SET);
    subscribe("delve", OP_OTHER);

    m_properties["terrain"] = new TerrainProperty(m_terrain, m_modifiedTerrain,
                                                  m_modifiedTerrain, a_terrain);

    Mercator::TileShader * tileShader = new Mercator::TileShader;
    tileShader->addShader(new Mercator::FillShader(), ROCK);
    tileShader->addShader(new Mercator::BandShader(-2.f, 1.5f), SAND);
    tileShader->addShader(new Mercator::GrassShader(1.f, 80.f, .5f, 1.f),GRASS);
    tileShader->addShader(new Mercator::DepthShader(0.f, -10.f), MUD);
    tileShader->addShader(new Mercator::HighShader(110.f), SNOW);
    m_terrain.addShader(tileShader, 0);
}

World::~World()
{
    delete &m_terrain;
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

void World::delveOperation(const Operation & op, OpVector & res)
{
    // FIXME
    std::cout << "Got delve operation" << std::endl << std::flush;

    if (op.getArgs().empty() || !op.getArgs().front().isMap()) {
        // FIXME Would it be more useful to send the error back where the
        // op came from?
        error(op, "Delve op has no args", res, getId());
        return;
    }
    const MapType & arg = op.getArgs().front().asMap();

    MapType::const_iterator Iend = arg.end();
    MapType::const_iterator I = arg.find("pos");
    if (I == Iend) {
        // FIXME Would it be more useful to send the error back where the
        // op came from?
        error(op, "Delve op to world has no POS in args", res, getId());
        return;
    }
    if (!I->second.isList()) {
        // FIXME Would it be more useful to send the error back where the
        // op came from?
        error(op, "Delve op to world has non list POS in args", res, getId());
        return;
    }
    WFMath::Point<3> delve_pos;
    // FIXME This data is non yet taint checked.
    delve_pos.fromAtlas(I->second.asList());
    std::cout << "Got delve on world at " << delve_pos
              << std::endl << std::flush;
    float x = delve_pos.x(),
          y = delve_pos.y();
    Mercator::Segment * segment = m_terrain.getSegment(x, y);
    if (segment == 0) {
        std::cerr << "No terrain at this point" << std::endl << std::flush;
        return;
    }
    if (!segment->isValid()) {
        segment->populate();
    }
    x = x - segment->getResolution() * segment->getXRef();
    y = y - segment->getResolution() * segment->getYRef();
    const Mercator::Segment::Surfacestore & surfaces = segment->getSurfaces();
    WFMath::Vector<3> normal;
    float height = -23;
    segment->getHeightAndNormal(x, y, height, normal);
    std::cout << "At the point " << x << "," << y << " of the segment the height is " << height << std::endl;
    std::cout << "The segment has " << surfaces.size() << std::endl << std::flush;
    if (surfaces.size() == 0) {
        log(ERROR, "The terrain has no surface data");
        return;
    }
    Mercator::Surface & tileSurface = *surfaces.begin()->second;
    if (!tileSurface.isValid()) {
        tileSurface.populate();
    }
    int material = tileSurface((int)x, (int)y, 0);
    std::cout << "The material at this point is " << material
              << std::endl << std::flush;
}

void World::LookOperation(const Operation & op, OpVector & res)
{
    // Let the worldrouter know we have been looked at.
    assert(m_world != 0);
    m_world->addPerceptive(op.getFrom());

    debug(std::cout << "World::Operation(Look)" << std::endl << std::flush;);
    const EntityDict & eobjects = m_world->getEntities();
    const std::string & from = op.getFrom();
    EntityDict::const_iterator J = eobjects.find(from);
    if (J == eobjects.end()) {
        debug(std::cout << "ERROR: Op has invalid from" << std::endl
                        << std::flush;);
        return World_parent::LookOperation(op, res);
    }
    if (!consts::enable_ranges) {
        debug(std::cout << "WARNING: Sight ranges disabled." << std::endl
                        << std::flush;);
        return World_parent::LookOperation(op, res);
    }

    Sight * s = new Sight();

    ListType & sargs = s->getArgs();
    sargs.push_back(MapType());
    MapType & omap = sargs.front().asMap();

    omap["id"] = getId();
    omap["parents"] = ListType(1, "world");
    omap["objtype"] = "obj";
    // FIXME integrate setting terrain with setting contains.

    TerrainProperty tp(m_terrain, m_modifiedTerrain,
                       m_createdTerrain, a_terrain);
    tp.add("terrain", omap);

    Entity * lookFrom = J->second;
    ListType & contlist = (omap["contains"] = ListType()).asList();
    EntitySet::const_iterator Iend = m_contains.end();
    for (EntitySet::const_iterator I = m_contains.begin(); I != Iend; ++I) {
        float fromSquSize = boxSquareSize((*I)->m_location.m_bBox);
        float dist = squareDistance((*I)->m_location, lookFrom->m_location);
        float view_factor = fromSquSize / dist;
        if (view_factor > consts::square_sight_factor) {
            contlist.push_back((*I)->getId());
        }
    }
    if (contlist.empty()) {
        debug(std::cout << "WARNING: contains empty." << std::endl << std::flush;);
        omap.erase("contains");
    }

    s->setTo(op.getFrom());
    res.push_back(s);
}

void World::BurnOperation(const Operation & op, OpVector & res)
{
    // Can't burn the world.
}

void World::MoveOperation(const Operation & op, OpVector & res)
{
    // Can't move the world.
}

void World::DeleteOperation(const Operation & op, OpVector & res)
{
    // Deleting has no effect.
}

void World::SetOperation(const Operation & op, OpVector & res)
{
    // This is the same as Thing::Operation(Set), except world does not
    // get deleted if its status goes below 0.
    m_seq++;
    const ListType & args = op.getArgs();
    if (args.empty() || !args.front().isMap()) {
       return;
    }
    const MapType & ent = args.front().asMap();
    merge(ent);
    Operation * s = new Sight();
    s->setArgs(ListType(1,op.asObject()));
    res.push_back(s);

    if (m_update_flags != 0) {
        updated.emit();
    }
}

void World::OtherOperation(const Operation & op, OpVector & res)
{
    const std::string & type = op.getParents().front().asString();

    if (type == "delve") {
        delveOperation(op, res);
    }
}

