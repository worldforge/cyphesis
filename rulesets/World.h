// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_WORLD_H
#define RULESETS_WORLD_H

#include "Thing.h"

// This is the in game object used to represent the world. I added this
// because I was not happy with the way the old object model used an
// out of game object (WorldRouter) to represent the world.

class World : public Thing {
  public:
    World();
    virtual ~World();

    virtual OpVector LookOperation(const Look & op);
    virtual OpVector FireOperation(const Fire & op);
    virtual OpVector DeleteOperation(const Delete & op);
    virtual OpVector MoveOperation(const Move & op);
    virtual OpVector SetOperation(const Set & op);
};

#endif // RULESETS_WORLD_H
