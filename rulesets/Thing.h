// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef THING_H
#define THING_H

#include <string>

#include "Entity.h"

#include <server/WorldRouter.h>

class Player;
class Routing;
class MemMap;

class Thing : public Entity {
  public:
    bool perceptive;

    Thing();
    virtual ~Thing();

    oplist sendWorld(RootOperation * op) const {
        return world->message(*op, this);
    }

    virtual void addToObject(Atlas::Message::Object &) const;
    virtual oplist Operation(const Setup & op);
    virtual oplist Operation(const Create & op);
    virtual oplist Operation(const Delete & op);
    virtual oplist Operation(const Fire & op);
    virtual oplist Operation(const Move & op);
    virtual oplist Operation(const Set & op);
    virtual oplist Operation(const Look & op);
};

#endif /* THING_H */
