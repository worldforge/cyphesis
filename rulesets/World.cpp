// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "World.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"

#include <Mercator/Terrain.h>

#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>

static const bool debug_flag = false;

void World::getTerrain(Element::MapType & t) const
{
    Element::ListType & terrain = (t["points"] = Element::ListType()).asList();

    const Mercator::Terrain::Pointstore & points = m_terrain.getPoints();
    Mercator::Terrain::Pointstore::const_iterator I = points.begin();
    for(; I != points.end(); ++I) {
        const Mercator::Terrain::Pointcolumn & pointcol = I->second;
        Mercator::Terrain::Pointcolumn::const_iterator J = pointcol.begin();
        for (; J != pointcol.end(); ++J) {
            terrain.push_back(Element::ListType(3));
            Element::ListType & point = terrain.back().asList();
            point[0] = (Element::FloatType)(I->first/* * 64*/);
            point[1] = (Element::FloatType)(J->first/* * 64*/);
            point[2] = (Element::FloatType)(J->second.height());
        }
    }
}

void World::setTerrain(const Element::MapType & t)
{
    Element::MapType::const_iterator I = t.find("points");
    if ((I != t.end()) && (I->second.isList())) {
        const Element::ListType & points = I->second.asList();
        Element::ListType::const_iterator I = points.begin();
        for(; I != points.end(); ++I) {
            if (!I->isList()) {
                continue;
            }
            const Element::ListType & point = I->asList();
            if (point.size() != 3) {
                continue;
            }
            // int x = (int)floor(point[0].asNum() / m_terrain.getRes());
            // int y = (int)floor(point[1].asNum() / m_terrain.getRes());
            int x = (int)point[0].asNum();
            int y = (int)point[1].asNum();
            m_terrain.setBasePoint(x, y, point[2].asNum());
        }
    }
}

World::World(const std::string & id) : World_parent(id),
                                       m_terrain(*new Mercator::Terrain())
{
    subscribe("set", OP_SET);

    for (int i = -5; i < 6; ++i) {
        for (int j = -5; j < 6; ++j) {
            if (i == -5 || i == 5 || j == -5 || j == 5) {
                m_terrain.setBasePoint(i, j, -10);
            } else {
                m_terrain.setBasePoint(i, j, 3);
            }
        }
    }
    
    // FIXME Just for testin
    m_terrain.setBasePoint(-1, -1, -6.8);
    m_terrain.setBasePoint(0, -1, -7.8);
    m_terrain.setBasePoint(-1, 0, -6.8);
    m_terrain.setBasePoint(-1, 1, -4.8);
    m_terrain.setBasePoint(1, -1, -5.8);
    m_terrain.setBasePoint(0, 0, 12.8);
    m_terrain.setBasePoint(1, 0, 23.1);
    m_terrain.setBasePoint(0, 1, 4.2);
    m_terrain.setBasePoint(1, 1, 44.7);
}

World::~World()
{
    delete &m_terrain;
}

bool World::get(const std::string & aname, Element & attr) const
{
    if (aname == "terrain") {
        attr = Element::MapType();
        getTerrain(attr.asMap());
        return true;
    }
    return World_parent::get(aname, attr);
}

void World::set(const std::string & aname, const Element & attr)
{
    if ((aname == "terrain") && attr.isMap()) {
        setTerrain(attr.asMap());
    } else {
        World_parent::set(aname, attr);
    }
}

void World::addToObject(Element::MapType & omap) const
{
    getTerrain((omap["terrain"] = Element::MapType()).asMap());
    World_parent::addToObject(omap);
}

OpVector World::LookOperation(const Look & op)
{
    // Let the worldrouter know we have been looked at.
    m_world->LookOperation(op);

    debug(std::cout << "World::Operation(Look)" << std::endl << std::flush;);
    const EntityDict & eobjects = m_world->getObjects();
    const std::string & from = op.getFrom();
    EntityDict::const_iterator J = eobjects.find(from);
    if (J == eobjects.end()) {
        debug(std::cout << "FATAL: Op has invalid from" << std::endl
                        << std::flush;);
        return World_parent::LookOperation(op);
    }
    if (!consts::enable_ranges) {
        debug(std::cout << "WARNING: Sight ranges disabled." << std::endl
                        << std::flush;);
        return World_parent::LookOperation(op);
    }

    Sight * s = new Sight(Sight::Instantiate());

    Element::ListType & sargs = s->getArgs();
    sargs.push_back(Element::MapType());
    Element::MapType & omap = sargs.front().asMap();

    omap["id"] = getId();
    omap["parents"] = Element::ListType(1, "world");
    omap["objtype"] = "object";
    // FIXME integrate setting terrain with setting contains.
    getTerrain((omap["terrain"] = Element::MapType()).asMap());
    Entity * opFrom = J->second;
    const Vector3D & fromLoc = opFrom->getXyz();
    Element::ListType & contlist = (omap["contains"] = Element(Element::ListType())).asList();
    EntitySet::const_iterator I = m_contains.begin();
    for(; I != m_contains.end(); I++) {
        if ((*I)->m_location.inRange(fromLoc, consts::sight_range)) {
            contlist.push_back(Element((*I)->getId()));
        }
    }
    if (contlist.empty()) {
        debug(std::cout << "WARNING: contains empty." << std::endl << std::flush;);
        omap.erase("contains");
    }

    s->setTo(op.getFrom());
    return OpVector(1,s);
}

OpVector World::BurnOperation(const Burn & op)
{
    // Can't burn the world.
    return OpVector();
}

OpVector World::MoveOperation(const Move & op)
{
    // Can't move the world.
    return OpVector();
}

OpVector World::DeleteOperation(const Delete & op)
{
    // Deleting has no effect.
    return OpVector();
}

OpVector World::SetOperation(const Set & op)
{
    // This is the same as Thing::Operation(Set), except world does not
    // get deleted if its status goes below 0.
    m_seq++;
    const Element::ListType & args = op.getArgs();
    if (args.empty()) {
       return OpVector();
    }
    try {
        const Element::MapType & ent = args.front().asMap();
        Element::MapType::const_iterator I;
        for (I = ent.begin(); I != ent.end(); I++) {
            set(I->first, I->second);
        }
        RootOperation * s = new Sight(Sight::Instantiate());
        s->setArgs(Element::ListType(1,op.asObject()));
        if (m_update_flags != 0) {
            updated.emit();
        }
        return OpVector(1,s);
    }
    catch (Atlas::Message::WrongTypeException) {
        log(ERROR, "EXCEPTION: Malformed set operation");
        return error(op, "Malformed set operation", getId());
    }
    return OpVector();
}
