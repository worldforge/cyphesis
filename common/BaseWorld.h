// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_BASE_WORLD_H
#define COMMON_BASE_WORLD_H

class Entity;

#include "OOGThing.h"

class BaseWorld : public OOGThing {
  private:
    // Private and un-implemented to prevent slicing
    BaseWorld(const BaseWorld &);
    const BaseWorld & operator=(const BaseWorld &);
  protected:
    double realTime;
    EntityDict eobjects;

  public:
    Entity & gameWorld;

    explicit BaseWorld(Entity &);
    virtual ~BaseWorld();

    Entity * getObject(const std::string & fid) {
        return eobjects[fid];
    }

    const EntityDict & getObjects() const {
        return eobjects;
    }

    const double & getTime() const {
        return realTime;
    }

    virtual Entity * addObject(const std::string &,
                               const Atlas::Message::Object::MapType &,
                               const std::string & id = std::string()) = 0;
    virtual void setSerialnoOp(RootOperation &) = 0;
    virtual OpVector message(RootOperation & op, const Entity * obj) = 0;
};

#endif // COMMON_BASE_WORLD_H
