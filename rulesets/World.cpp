// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#include "World.h"

#include "TerrainProperty.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/inheritance.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/TileShader.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>
#include <Mercator/Surface.h>

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Sight.h>

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Sight;

typedef enum { ROCK = 0, SAND = 1, GRASS = 2, SILT = 3, SNOW = 4} Surface;

/// \brief Constructor for the World entity
World::World(const std::string & id) : World_parent(id),
             m_terrain(*new Mercator::Terrain(Mercator::Terrain::SHADED)),
             m_tileShader(*new Mercator::TileShader)
{
    subscribe("set", OP_SET);
    subscribe("delve", OP_OTHER);
    subscribe("dig", OP_OTHER);
    subscribe("mow", OP_OTHER);

    m_properties["terrain"] = new TerrainProperty(m_terrain, m_modifiedTerrain,
                                                  m_modifiedTerrain, a_terrain);

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
    x = x - segment->getResolution() * segment->getXRef();
    y = y - segment->getResolution() * segment->getYRef();
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
    Mercator::Surface & tileSurface = *surfaces.begin()->second;
    if (!tileSurface.isValid()) {
        tileSurface.populate();
    }
    material = tileSurface((int)x, (int)y, 0);
    return 0;
}

void World::delveOperation(const Operation & op, OpVector & res)
{
    if (op.getArgs().empty() || !op.getArgs().front().isMap()) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Delve op has no args", res, getId());
        return;
    }
    const MapType & arg = op.getArgs().front().asMap();

    MapType::const_iterator Iend = arg.end();
    MapType::const_iterator I = arg.find("pos");
    if (I == Iend) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Delve op to world has no POS in args", res, getId());
        return;
    }
    if (!I->second.isList()) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Delve op to world has non list POS in args", res, getId());
        return;
    }
    const ListType & delve_pos_attr = I->second.asList();
    WFMath::Point<3> delve_pos;
    // FIXME This data is non yet taint checked.
    delve_pos.fromAtlas(delve_pos_attr);
    debug(std::cout << "Got delve on world at " << delve_pos
                    << std::endl << std::flush;);
    int material;
    if (getSurface(delve_pos, material) != 0) {
        return;
    }
    debug(std::cout << "The material at this point is " << material
                    << std::endl << std::flush;);
    switch (material) {
      case ROCK:
        {
            Operation * c = new Create;
            ListType & args = c->getArgs();
            args.push_back(MapType());
            MapType & carg = args.back().asMap();
            carg["parents"] = ListType(1, "boulder");
            carg["name"] ="boulder";
            carg["loc"] = getId();
            carg["pos"] = delve_pos_attr;
            c->setTo(getId());
            res.push_back(c);
        }
        break;
      case SNOW:
        {
            Operation * c = new Create;
            ListType & args = c->getArgs();
            args.push_back(MapType());
            MapType & carg = args.back().asMap();
            carg["parents"] = ListType(1, "ice");
            carg["name"] ="ice";
            carg["loc"] = getId();
            carg["pos"] = delve_pos_attr;
            c->setTo(getId());
            res.push_back(c);
        }
        break;
      default:
        break;
    }
}

void World::digOperation(const Operation & op, OpVector & res)
{
    if (op.getArgs().empty() || !op.getArgs().front().isMap()) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Delve op has no args", res, getId());
        return;
    }
    const MapType & arg = op.getArgs().front().asMap();

    MapType::const_iterator Iend = arg.end();
    MapType::const_iterator I = arg.find("pos");
    if (I == Iend) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Delve op to world has no POS in args", res, getId());
        return;
    }
    if (!I->second.isList()) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Delve op to world has non list POS in args", res, getId());
        return;
    }
    const ListType & dig_pos_attr = I->second.asList();
    WFMath::Point<3> dig_pos;
    // FIXME This data is non yet taint checked.
    dig_pos.fromAtlas(dig_pos_attr);
    debug(std::cout << "Got dig on world at " << dig_pos
                    << std::endl << std::flush;);
    int material;
    if (getSurface(dig_pos, material) != 0) {
        return;
    }
    debug(std::cout << "The material at this point is " << material
                    << std::endl << std::flush;);
    switch (material) {
      case SAND:
        {
            Operation * c = new Create;
            ListType & args = c->getArgs();
            args.push_back(MapType());
            MapType & carg = args.back().asMap();
            carg["parents"] = ListType(1, "pile");
            carg["material"] = "sand";
            carg["name"] = "sand";
            carg["loc"] = getId();
            carg["pos"] = dig_pos_attr;
            c->setTo(getId());
            res.push_back(c);
        }
        break;
      case GRASS:
        {
            Operation * c = new Create;
            ListType & args = c->getArgs();
            args.push_back(MapType());
            MapType & carg = args.back().asMap();
            carg["parents"] = ListType(1, "pile");
            carg["material"] = "earth";
            carg["name"] = "earth";
            carg["loc"] = getId();
            carg["pos"] = dig_pos_attr;
            c->setTo(getId());
            res.push_back(c);
        }
        break;
      case SILT:
        {
            Operation * c = new Create;
            ListType & args = c->getArgs();
            args.push_back(MapType());
            MapType & carg = args.back().asMap();
            carg["parents"] = ListType(1, "pile");
            carg["material"] = "silt";
            carg["name"] = "silt";
            carg["loc"] = getId();
            carg["pos"] = dig_pos_attr;
            c->setTo(getId());
            res.push_back(c);
        }
        break;

      default:
        break;
    }
}

void World::mowOperation(const Operation & op, OpVector & res)
{
    if (op.getArgs().empty() || !op.getArgs().front().isMap()) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Delve op has no args", res, getId());
        return;
    }
    const MapType & arg = op.getArgs().front().asMap();

    MapType::const_iterator Iend = arg.end();
    MapType::const_iterator I = arg.find("pos");
    if (I == Iend) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Delve op to world has no POS in args", res, getId());
        return;
    }
    if (!I->second.isList()) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Delve op to world has non list POS in args", res, getId());
        return;
    }
    const ListType & mow_pos_attr = I->second.asList();
    WFMath::Point<3> mow_pos;
    // FIXME This data is non yet taint checked.
    mow_pos.fromAtlas(mow_pos_attr);
    debug(std::cout << "Got mow on world at " << mow_pos
                    << std::endl << std::flush;);
    int material;
    if (getSurface(mow_pos, material) != 0) {
        return;
    }
    debug(std::cout << "The material at this point is " << material
                    << std::endl << std::flush;);
    switch (material) {
      case GRASS:
        {
            Operation * c = new Create;
            ListType & args = c->getArgs();
            args.push_back(MapType());
            MapType & carg = args.back().asMap();
            carg["parents"] = ListType(1, "grass");
            carg["loc"] = getId();
            carg["pos"] = mow_pos_attr;
            c->setTo(getId());
            res.push_back(c);
        }
        break;
      default:
        break;
    }
}

void World::EatOperation(const Operation & op, OpVector & res)
{
    const std::string & from_id = op.getFrom();
    EntityDict::const_iterator I = m_world->getEntities().find(from_id);
    if (I == m_world->getEntities().end()) {
        log(ERROR, "World got eat op from non-existant entity.");
        return;
    }
    const std::string & from_type = I->second->getType();
    if (Inheritance::instance().isTypeOf(from_type, "plant")) {
        log(NOTICE, "Eat coming from a plant.");
    } else if (Inheritance::instance().isTypeOf(from_type, "character")) {
        log(NOTICE, "Eat coming from an animal.");
    }
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
    } else if (type == "dig") {
        digOperation(op, res);
    } else if (type == "mow") {
        mowOperation(op, res);
    }
}

