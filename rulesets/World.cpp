// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "World.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/stringstream.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>

#include <wfmath/MersenneTwister.h>

#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::FloatType;

typedef Mercator::Terrain::Pointstore Pointstore;
typedef Mercator::Terrain::Pointcolumn Pointcolumn;

void World::getTerrain(MapType & t) const
{
    MapType & terrain = (t["points"] = MapType()).asMap();

    const Pointstore & points = m_terrain.getPoints();
    Pointstore::const_iterator I = points.begin();
    for(; I != points.end(); ++I) {
        const Pointcolumn & pointcol = I->second;
        Pointcolumn::const_iterator J = pointcol.begin();
        for (; J != pointcol.end(); ++J) {
            std::stringstream key;
            key << I->first << "x" << J->first;
            ListType & point = (terrain[key.str()] = ListType(3)).asList();
            point[0] = (FloatType)(I->first);
            point[1] = (FloatType)(J->first);
            point[2] = (FloatType)(J->second.height());
        }
    }
}

void World::setTerrain(const MapType & t)
{
    debug(std::cout << "World::setTerrain()" << std::endl << std::flush;);

    const Pointstore & basePoints = m_terrain.getPoints();

    MapType::const_iterator I = t.find("points");
    if ((I != t.end()) && (I->second.isMap())) {
        const MapType & points = I->second.asMap();
        MapType::const_iterator I = points.begin();
        for(; I != points.end(); ++I) {
            if (!I->second.isList()) {
                continue;
            }
            const ListType & point = I->second.asList();
            if (point.size() != 3) {
                continue;
            }

            int x = (int)point[0].asNum();
            int y = (int)point[1].asNum();

            Pointstore::const_iterator J = basePoints.find(x);
            if ((J == basePoints.end()) ||
                (J->second.find(y) == J->second.end())) {
                // Newly added point.
                m_createdTerrain[x].insert(y);
                std::cout << "New point" << std::endl << std::flush;
            } else {
                // Modified point
                PointSet::const_iterator K = m_createdTerrain.find(x);
                if ((K == m_createdTerrain.end()) ||
                    (K->second.find(y) == K->second.end())) {
                    // Already in database
                    m_modifiedTerrain[x].insert(y);
                    std::cout << "Changed point" << std::endl << std::flush;
                }
                // else do nothing, as its currently waiting to be added.
            }
            
            m_terrain.setBasePoint(x, y, point[2].asNum());
            // FIXME Add support for roughness and falloff, as done
            // by damien in equator and FIXMEd out by me

            
        }
    }
}

World::World(const std::string & id) : World_parent(id),
                                       m_terrain(*new Mercator::Terrain())
{
    subscribe("set", OP_SET);

#if 0 
    // FIXME Just for testin

    WFMath::MTRand rng;

    for (int i = -5; i < 6; ++i) {
        for (int j = -5; j < 6; ++j) {
            if (i == 5 || j == 5) {
                m_terrain.setBasePoint(i, j, 100 + rng() * 50);
            } else if (i == -5 || j == -5) {
                m_terrain.setBasePoint(i, j, -10 - rng() * 20);
            } else if ((i == 2 || i == 3) && (j == 2 || j == 3)) {
                m_terrain.setBasePoint(i, j, 20 + rng() * 5);
            } else if (i == 4 || j == 4) {
                m_terrain.setBasePoint(i, j, 30 + rng() * 50);
            } else if (i == -4 || j == -4) {
                m_terrain.setBasePoint(i, j, -2 + rng() * 5);
            } else {
                float mult = abs(i) + abs(j);
                m_terrain.setBasePoint(i, j, 1 + rng() * mult * 8);
            }
        }
    }
    
    m_terrain.setBasePoint(-1, -1, -16.8);
    m_terrain.setBasePoint(0, -1, -3.8);
    m_terrain.setBasePoint(-1, 0, -2.8);
    m_terrain.setBasePoint(-1, 1, 12.8);
    m_terrain.setBasePoint(1, -1, 15.8);
    m_terrain.setBasePoint(0, 0, 12.8);
    m_terrain.setBasePoint(1, 0, 23.1);
    m_terrain.setBasePoint(0, 1, 14.2);
    m_terrain.setBasePoint(1, 1, 19.7);
#endif
}

World::~World()
{
    delete &m_terrain;
}

float World::getHeight(float x, float y)
{
    Mercator::Segment * s = m_terrain.getSegment(x, y);
    if (s != 0 && !s->isValid()) {
        s->populate();
    }
    return m_terrain.get(x, y);
}

bool World::get(const std::string & aname, Element & attr) const
{
    if (aname == "terrain") {
        attr = MapType();
        getTerrain(attr.asMap());
        return true;
    }
    return World_parent::get(aname, attr);
}

void World::set(const std::string & aname, const Element & attr)
{
    if ((aname == "terrain") && attr.isMap()) {
        m_update_flags |= a_terrain;
        setTerrain(attr.asMap());
    } else {
        World_parent::set(aname, attr);
    }
}

void World::addToMessage(MapType & omap) const
{
    getTerrain((omap["terrain"] = MapType()).asMap());
    World_parent::addToMessage(omap);
}

void World::LookOperation(const Look & op, OpVector & res)
{
    // Let the worldrouter know we have been looked at.
    assert(m_world != 0);
    m_world->addPerceptive(op.getFrom());

    debug(std::cout << "World::Operation(Look)" << std::endl << std::flush;);
    const EntityDict & eobjects = m_world->getObjects();
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
    getTerrain((omap["terrain"] = MapType()).asMap());
    Entity * lookFrom = J->second;
    ListType & contlist = (omap["contains"] = ListType()).asList();
    EntitySet::const_iterator I = m_contains.begin();
    for(; I != m_contains.end(); I++) {
        float fromSquSize = boxSquareSize((*I)->m_location.m_bBox);
        Point3D d((*I)->m_location.relativePosition(lookFrom->m_location));
        float view_factor = fromSquSize / sqrMag(d);
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

void World::BurnOperation(const Burn & op, OpVector & res)
{
    // Can't burn the world.
}

void World::MoveOperation(const Move & op, OpVector & res)
{
    // Can't move the world.
}

void World::DeleteOperation(const Delete & op, OpVector & res)
{
    // Deleting has no effect.
}

void World::SetOperation(const Set & op, OpVector & res)
{
    // This is the same as Thing::Operation(Set), except world does not
    // get deleted if its status goes below 0.
    m_seq++;
    const ListType & args = op.getArgs();
    if (args.empty()) {
       return;
    }
    try {
        const MapType & ent = args.front().asMap();
        MapType::const_iterator I;
        for (I = ent.begin(); I != ent.end(); I++) {
            set(I->first, I->second);
        }
        RootOperation * s = new Sight();
        s->setArgs(ListType(1,op.asObject()));
        if (m_update_flags != 0) {
            updated.emit();
        }
        res.push_back(s);
    }
    catch (Atlas::Message::WrongTypeException) {
        log(ERROR, "EXCEPTION: Malformed set operation");
        error(op, "Malformed set operation", res, getId());
        return;
    }
}
