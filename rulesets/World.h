// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_WORLD_H
#define RULESETS_WORLD_H

#include "Thing.h"

namespace Mercator {
    class Terrain;
}

// This is the in game object used to represent the world. I added this
// because I was not happy with the way the old object model used an
// out of game object (WorldRouter) to represent the world.

class World : public Thing {
  protected:
    Mercator::Terrain & m_terrain;
  public:
    explicit World(const std::string & id);
    virtual ~World();

    virtual const Atlas::Message::Object get(const std::string &) const;
    virtual void set(const std::string &, const Atlas::Message::Object &);

    virtual void addToObject(Atlas::Message::Object::MapType &) const;

    virtual OpVector LookOperation(const Look & op);
    virtual OpVector BurnOperation(const Burn & op);
    virtual OpVector DeleteOperation(const Delete & op);
    virtual OpVector MoveOperation(const Move & op);
    virtual OpVector SetOperation(const Set & op);
};

#endif // RULESETS_WORLD_H
