// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_BASE_WORLD_H
#define COMMON_BASE_WORLD_H

class WorldRouter;
class ServerRouting;
class Entity;
class World;

#include "OOGThing.h"

class BaseWorld : public OOGThing {
  private:
    // Private and un-implemented to prevent slicing
    BaseWorld(const BaseWorld &);
    const BaseWorld & operator=(const BaseWorld &);
  protected:
    double realTime;
    EntitySet objectList;
    // int nextId;
    EntityDict eobjects;

    // std::string getId(std::string & name);
  public:
    Entity & gameWorld;

    explicit BaseWorld(Entity &);
    virtual ~BaseWorld();

    virtual Entity * addObject(Entity * obj) = 0;
    virtual Entity * addObject(const std::string &,
                               const Atlas::Message::Object::MapType &,
                               const std::string & id = std::string()) = 0;
    // void delObject(Entity * obj);

    Entity * getObject(const std::string & fid) {
        return eobjects[fid];
    }

    const EntityDict & getObjects() const {
        return eobjects;
    }

    Entity * findObject(const std::string & fid) {
        return eobjects[fid];
    }

    const double & getTime() const {
        return realTime;
    }

    virtual OpVector message(RootOperation & op, const Entity * obj) = 0;
    virtual OpVector message(const RootOperation & op) = 0;
    virtual OpVector operation(const RootOperation & op) = 0;

    virtual OpVector lookOperation(const Look & op) = 0;
};

#endif // COMMON_BASE_WORLD_H
