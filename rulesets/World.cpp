// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#include "World.h"

#include "TerrainProperty.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/inheritance.h"

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
World::World(const std::string & id) : World_parent(id),
             m_terrain(*new Mercator::Terrain(Mercator::Terrain::SHADED)),
             m_tileShader(*new Mercator::TileShader)
{
    subscribe("eat", OP_EAT);
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
    if (op->getArgs().empty()) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Delve op has no args", res, getId());
        return;
    }
    const Root & arg = op->getArgs().front();
    RootEntity delve_arg = smart_dynamic_cast<RootEntity>(arg);
    if (!delve_arg.isValid()) {
        error(op, "Delve op has malformed args", res, getId());
        return;
    }
    if (!delve_arg->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Delve op to world has no POS in args", res, getId());
        return;
    }
    WFMath::Point<3> delve_pos;
    if (fromStdVector(delve_pos, delve_arg->getPos()) != 0) {
        error(op, "Delve op to world has bad POS in args", res, getId());
        return;
    }
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
            Create c;
            Anonymous carg;
            carg->setParents(std::list<std::string>(1, "boulder"));
            carg->setName("boulder");
            carg->setLoc(getId());
            carg->setPos(delve_arg->getPos());
            c->setArgs1(carg);
            c->setTo(getId());
            res.push_back(c);
        }
        break;
      case SNOW:
        {
            Create c;
            Anonymous carg;
            carg->setParents(std::list<std::string>(1, "ice"));
            carg->setName("ice");;
            carg->setLoc(getId());
            carg->setPos(delve_arg->getPos());
            c->setArgs1(carg);
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
    if (op->getArgs().empty()) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Dig op has no args", res, getId());
        return;
    }
    const Root & arg = op->getArgs().front();
    RootEntity dig_arg = smart_dynamic_cast<RootEntity>(arg);
    if (!dig_arg.isValid()) {
        error(op, "Dig op has malformed args", res, getId());
        return;
    }
    if (!dig_arg->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Dig op to world has no POS in args", res, getId());
        return;
    }
    WFMath::Point<3> dig_pos;
    if (fromStdVector(dig_pos, dig_arg->getPos()) != 0) {
        error(op, "Dig op to world has bad POS in args", res, getId());
        return;
    }
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
            Create c;
            Anonymous carg;
            carg->setParents(std::list<std::string>(1, "pile"));
            carg->setAttr("material", "sand");
            carg->setName("sand");
            carg->setLoc(getId());
            carg->setPos(dig_arg->getPos());
            c->setArgs1(carg);
            c->setTo(getId());
            res.push_back(c);
        }
        break;
      case GRASS:
        {
            Create c;
            Anonymous carg;
            carg->setParents(std::list<std::string>(1, "pile"));
            carg->setAttr("material", "earth");
            carg->setName("earth");
            carg->setLoc(getId());
            carg->setPos(dig_arg->getPos());
            c->setArgs1(carg);
            c->setTo(getId());
            res.push_back(c);
        }
        break;
      case SILT:
        {
            Create c;
            Anonymous carg;
            carg->setParents(std::list<std::string>(1, "pile"));
            carg->setAttr("material", "silt");
            carg->setName("silt");
            carg->setLoc(getId());
            carg->setPos(dig_arg->getPos());
            c->setArgs1(carg);
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
    if (op->getArgs().empty()) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Delve op has no args", res, getId());
        return;
    }
    const Root & arg = op->getArgs().front();
    RootEntity mow_arg = smart_dynamic_cast<RootEntity>(arg);
    if (!mow_arg.isValid()) {
        error(op, "Mow op has malformed args", res, getId());
        return;
    }
    if (!mow_arg->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
        // This op comes from a tool, so sending error back is kinda pointless
        error(op, "Mow op to world has no POS in args", res, getId());
        return;
    }
    WFMath::Point<3> mow_pos;
    if (fromStdVector(mow_pos, mow_arg->getPos()) != 0) {
        error(op, "Mow op to world has bad POS in args", res, getId());
        return;
    }
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
            Create c;
            Anonymous carg;
            carg->setParents(std::list<std::string>(1, "grass"));
            carg->setId(getId());
            carg->setPos(mow_arg->getPos());;
            c->setArgs1(carg);
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
    const std::string & from_id = op->getFrom();
    EntityDict::const_iterator I = m_world->getEntities().find(from_id);
    if (I == m_world->getEntities().end()) {
        log(ERROR, "World got eat op from non-existant entity.");
        return;
    }

    Entity * from = I->second;
    assert(from != 0);

    Point3D from_pos = relativePos(m_location, from->m_location);
    int material;
    if (getSurface(from_pos, material) != 0) {
        debug(std::cout << "no surface hit" << std::endl << std::flush;);
        return;
    }
   
    const std::string & from_type = from->getType();
    if (Inheritance::instance().isTypeOf(from_type, "plant")) {
        if (material == GRASS) {
            debug(std::cout << "From grass" << std::endl << std::flush;);
            Nourish nourish;
            nourish->setTo(from_id);
            Anonymous nour_arg;
            nour_arg->setAttr("mass", log(from->getMass() + 1));
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
    // Let the worldrouter know we have been looked at.
    assert(m_world != 0);
    m_world->addPerceptive(op->getFrom());

    if (!consts::enable_ranges) {
        debug(std::cout << "WARNING: Sight ranges disabled." << std::endl
                        << std::flush;);
        return World_parent::LookOperation(op, res);
    }

    debug(std::cout << "World::Operation(Look)" << std::endl << std::flush;);
    const EntityDict & eobjects = m_world->getEntities();
    const std::string & from = op->getFrom();
    EntityDict::const_iterator J = eobjects.find(from);
    if (J == eobjects.end()) {
        debug(std::cout << "ERROR: Op has invalid from" << std::endl
                        << std::flush;);
        return World_parent::LookOperation(op, res);
    }
    Entity * lookFrom = J->second;

    Sight s;

    Anonymous sarg;
    s->setArgs1(sarg);

    sarg->setId(getId());
    sarg->setParents(std::list<std::string>(1, "world"));
    sarg->setObjtype("obj");
    // FIXME integrate setting terrain with setting contains.

    TerrainProperty tp(m_terrain, m_modifiedTerrain,
                       m_createdTerrain, a_terrain);
    tp.add("terrain", sarg);

    ListType contlist;
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
    } else {
        sarg->setAttr("contains", contlist);
    }

    s->setTo(op->getFrom());
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
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
       error(op, "Set has no argument", res, getId());
       return;
    }
    const Root & ent = args.front();
    merge(ent->asMessage());
    Sight s;
    s->setArgs1(op);
    res.push_back(s);

    if (m_update_flags != 0) {
        updated.emit();
    }
}

void World::OtherOperation(const Operation & op, OpVector & res)
{
    const std::string & type = op->getParents().front();

    if (type == "delve") {
        delveOperation(op, res);
    } else if (type == "dig") {
        digOperation(op, res);
    } else if (type == "mow") {
        mowOperation(op, res);
    }
}

