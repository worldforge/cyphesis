// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>

#include "World.h"

oplist World::LookOperation(const Look & op)
{
    return world->lookOperation(op);
}

oplist World::FireOperation(const Fire & op)
{
    // Can't burn the world.
    return oplist();
}

oplist World::MoveOperation(const Move & op)
{
    // Can't move the world.
    return oplist();
}

oplist World::DeleteOperation(const Delete & op)
{
    // Deleting has no effect.
    return oplist();
}

oplist World::SetOperation(const Set & op)
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
        s->SetArgs(Object::ListType(1,op.AsObject()));
        oplist res2(1,s);
        return res2;
    }
    catch (Atlas::Message::WrongTypeException) {
        std::cerr << "EXCEPTION: Malformed set operation\n";
        return error(op, "Malformed set operation\n");
    }
    return oplist();
}
