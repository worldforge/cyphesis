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
    Element::ListType & terrain = (t["points"] = Element::ListType()).AsList();

    const Mercator::Terrain::Pointstore & points = m_terrain.getPoints();
    Mercator::Terrain::Pointstore::const_iterator I = points.begin();
    for(; I != points.end(); ++I) {
        const Mercator::Terrain::Pointcolumn & pointcol = I->second;
        Mercator::Terrain::Pointcolumn::const_iterator J = pointcol.begin();
        for (; J != pointcol.end(); ++J) {
            terrain.push_back(Element::ListType(3));
            Element::ListType & point = terrain.back().AsList();
            point[0] = (Element::FloatType)(I->first * 64);
            point[1] = (Element::FloatType)(J->first * 64);
            point[2] = (Element::FloatType)(J->second);
        }
    }
}

void World::setTerrain(const Element::MapType & t)
{
    Element::MapType::const_iterator I = t.find("points");
    if ((I != t.end()) && (I->second.IsList())) {
        const Element::ListType & points = I->second.AsList();
        Element::ListType::const_iterator I = points.begin();
        for(; I != points.end(); ++I) {
            if (!I->IsList()) {
                continue;
            }
            const Element::ListType & point = I->AsList();
            if (point.size() != 3) {
                continue;
            }
            int x = (int)floor(point[0].AsNum() / m_terrain.getRes());
            int y = (int)floor(point[1].AsNum() / m_terrain.getRes());
            m_terrain.setBasePoint(x, y, point[2].AsNum());
        }
    }
}

World::World(const std::string & id) : Thing(id), m_terrain(*new Mercator::Terrain())
{
    subscribe("set", OP_SET);

    // FIXME Just for testin
    m_terrain.setBasePoint(0, 0, 24.8);
    m_terrain.setBasePoint(1, 0, 27.1);
    m_terrain.setBasePoint(0, 1, 20.2);
    m_terrain.setBasePoint(1, 1, 28.7);
}

World::~World()
{
}

bool World::get(const std::string & aname, Element & attr) const
{
    if (aname == "terrain") {
        attr = Element::MapType();
        getTerrain(attr.AsMap());
        return true;
    }
    return Entity::get(aname, attr);
}

void World::set(const std::string & aname, const Element & attr)
{
    if ((aname == "terrain") && attr.IsMap()) {
        setTerrain(attr.AsMap());
    } else {
        Entity::set(aname, attr);
    }
}

void World::addToObject(Element::MapType & omap) const
{
    getTerrain((omap["terrain"] = Element::MapType()).AsMap());
    Entity::addToObject(omap);
}

OpVector World::LookOperation(const Look & op)
{
    // Let the worldrouter know we have been looked at.
    world->LookOperation(op);

    debug(std::cout << "World::Operation(Look)" << std::endl << std::flush;);
    const EntityDict & eobjects = world->getObjects();
    const std::string & from = op.GetFrom();
    EntityDict::const_iterator J = eobjects.find(from);
    if (J == eobjects.end()) {
        debug(std::cout << "FATAL: Op has invalid from" << std::endl
                        << std::flush;);
        return Entity::LookOperation(op);
    }
    if (!consts::enable_ranges) {
        debug(std::cout << "WARNING: Sight ranges disabled." << std::endl
                        << std::flush;);
        return Entity::LookOperation(op);
    }

    Sight * s = new Sight(Sight::Instantiate());

    Element::ListType & sargs = s->GetArgs();
    sargs.push_back(Element::MapType());
    Element::MapType & omap = sargs.front().AsMap();

    omap["id"] = getId();
    omap["parents"] = Element::ListType(1, "world");
    omap["objtype"] = "object";
    // FIXME integrate setting terrain with setting contains.
    getTerrain((omap["terrain"] = Element::MapType()).AsMap());
    Entity * opFrom = J->second;
    const Vector3D & fromLoc = opFrom->getXyz();
    Element::ListType & contlist = (omap["contains"] = Element(Element::ListType())).AsList();
    EntitySet::const_iterator I = contains.begin();
    for(; I != contains.end(); I++) {
        if ((*I)->location.inRange(fromLoc, consts::sight_range)) {
            contlist.push_back(Element((*I)->getId()));
        }
    }
    if (contlist.empty()) {
        debug(std::cout << "WARNING: contains empty." << std::endl << std::flush;);
        omap.erase("contains");
    }

    s->SetTo(op.GetFrom());
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
    seq++;
    const Element::ListType & args = op.GetArgs();
    if (args.empty()) {
       return OpVector();
    }
    try {
        const Element::MapType & ent = args.front().AsMap();
        Element::MapType::const_iterator I;
        for (I = ent.begin(); I != ent.end(); I++) {
            set(I->first, I->second);
        }
        RootOperation * s = new Sight(Sight::Instantiate());
        s->SetArgs(Element::ListType(1,op.AsObject()));
        if (update_flags != 0) {
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
