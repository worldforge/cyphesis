// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_THING_H
#define RULESETS_THING_H

#include "Entity.h"

#include <common/BaseWorld.h>

// This is the base class from which all physical or tangiable in-game objects
// inherit. It can be used itself to represent any in-game object for which
// any special behavior can be described by a script.

class Thing : public Entity {
  protected:
    bool perceptive;
  public:

    Thing();
    virtual ~Thing();

    oplist sendWorld(RootOperation * op) const {
        return world->message(*op, this);
    }

    virtual oplist SetupOperation(const Setup & op);
    virtual oplist ActionOperation(const Action & op);
    virtual oplist CreateOperation(const Create & op);
    virtual oplist DeleteOperation(const Delete & op);
    virtual oplist FireOperation(const Fire & op);
    virtual oplist MoveOperation(const Move & op);
    virtual oplist SetOperation(const Set & op);
    virtual oplist LookOperation(const Look & op);
};

#endif // RULESETS_THING_H
