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

    explicit Thing(const std::string & id);
    virtual ~Thing();

    // sendWorld bipasses serialno assignment, so you must ensure
    // that serialno is sorted. This allows client serialnos to get
    // in, so that client gets correct usefull refnos back
    OpVector sendWorld(RootOperation * op) const {
        return world->message(*op, this);
    }

    virtual OpVector SetupOperation(const Setup & op);
    virtual OpVector ActionOperation(const Action & op);
    virtual OpVector CreateOperation(const Create & op);
    virtual OpVector DeleteOperation(const Delete & op);
    virtual OpVector BurnOperation(const Burn & op);
    virtual OpVector MoveOperation(const Move & op);
    virtual OpVector SetOperation(const Set & op);
    virtual OpVector LookOperation(const Look & op);
};

#endif // RULESETS_THING_H
