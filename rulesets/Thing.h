// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef THING_H
#define THING_H

#include "Entity.h"

#include <server/WorldRouter.h>

// This is the base class from which all physical or tangiable in-game objects
// inherit. It can be used itself to represent any in-game object for which
// any special behavoir can be described by a script.

class Thing : public Entity {
  protected:
    bool perceptive;
  public:

    Thing();
    virtual ~Thing();

    oplist sendWorld(RootOperation * op) const {
        return world->message(*op, this);
    }

    virtual oplist Operation(const Setup & op);
    virtual oplist Operation(const Action & op);
    virtual oplist Operation(const Create & op);
    virtual oplist Operation(const Delete & op);
    virtual oplist Operation(const Fire & op);
    virtual oplist Operation(const Move & op);
    virtual oplist Operation(const Set & op);
    virtual oplist Operation(const Look & op);
};

#endif // THING_H
