// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "World.h"

#include <common/log.h>

#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>

World::World(const std::string & id) : Thing(id)
{
    subscribe("set", OP_SET);
}

World::~World()
{
}

OpVector World::LookOperation(const Look & op)
{
    return world->LookOperation(op);
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
    const Fragment::ListType & args = op.GetArgs();
    if (args.empty()) {
       return OpVector();
    }
    try {
        const Fragment::MapType & ent = args.front().AsMap();
        Fragment::MapType::const_iterator I;
        for (I = ent.begin(); I != ent.end(); I++) {
            set(I->first, I->second);
        }
        RootOperation * s = new Sight(Sight::Instantiate());
        s->SetArgs(Fragment::ListType(1,op.AsObject()));
        OpVector res2(1,s);
        return res2;
    }
    catch (Atlas::Message::WrongTypeException) {
        log(ERROR, "EXCEPTION: Malformed set operation");
        return error(op, "Malformed set operation\n");
    }
    return OpVector();
}
