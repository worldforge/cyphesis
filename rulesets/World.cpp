// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "World.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"

#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>

static const bool debug_flag = false;

World::World(const std::string & id) : Thing(id)
{
    subscribe("set", OP_SET);
}

World::~World()
{
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
