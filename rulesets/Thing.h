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
  public:

    Thing();
    virtual ~Thing();

    OpVector sendWorld(RootOperation * op) const {
        return world->message(*op, this);
    }

    void scriptSubscribe(const std::string &);

    virtual OpVector SetupOperation(const Setup & op);
    virtual OpVector ActionOperation(const Action & op);
    virtual OpVector CreateOperation(const Create & op);
    virtual OpVector DeleteOperation(const Delete & op);
    virtual OpVector FireOperation(const Fire & op);
    virtual OpVector MoveOperation(const Move & op);
    virtual OpVector SetOperation(const Set & op);
    virtual OpVector LookOperation(const Look & op);
};

#endif // RULESETS_THING_H
