// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef WORLD_H
#define WORLD_H

#include <string>

#include "Thing.h"

// This is the in game object used to represent the world. I added this
// because I was not happy with the way the old object model used an
// out of game object (WorldRouter) to represent the world.

class World : public Thing {
  public:
    virtual oplist LookOperation(const Look & op);
    virtual oplist FireOperation(const Fire & op);
    virtual oplist DeleteOperation(const Delete & op);
    virtual oplist MoveOperation(const Move & op);
    virtual oplist SetOperation(const Set & op);
};

#endif // WORLD_H
