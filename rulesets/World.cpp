// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>

#include <common/Fire.h>

#include "World.h"

oplist World::Operation(const Look & op)
{
    return world->lookOperation(op);
}

oplist World::Operation(const Fire & op)
{
    // Can't burn the world.
    return oplist();
}

oplist World::Operation(const Move & op)
{
    // Can't move the world.
    return oplist();
}

oplist World::Operation(const Delete & op)
{
    // Deleting has no effect.
    return oplist();
}

oplist World::Operation(const Set & op)
{
    // This is the same as Thing::Operation(Set), except world does not
    // get deleted if its status goes below 0.
    seq++;
    const Object::ListType & args=op.GetArgs();
    if (args.size() == 0) {
       return oplist();
    }
    try {
        const Object::MapType & ent = args.front().AsMap();
        Object::MapType::const_iterator I;
        for (I = ent.begin(); I != ent.end(); I++) {
            set(I->first, I->second);
        }
        RootOperation * s = new Sight(Sight::Instantiate());
        Object::ListType args2(1,op.AsObject());
        s->SetArgs(args2);
        oplist res2(1,s);
        return res2;
    }
    catch (Atlas::Message::WrongTypeException) {
        cerr << "EXCEPTION: Malformed set operation\n";
        return error(op, "Malformed set operation\n");
    }
    return oplist();
}
