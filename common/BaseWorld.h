// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_BASE_WORLD_H
#define COMMON_BASE_WORLD_H

#include <sys/time.h>
#include <unistd.h>

class WorldRouter;
class ServerRouting;
class Entity;
class World;

#include <common/OOGThing.h>

class BaseWorld : public OOGThing {
  protected:
    double realTime;
    elist_t objectList;
    // int nextId;
    edict_t eobjects;

    // std::string getId(std::string & name);
  public:
    Entity & gameWorld;

    BaseWorld(Entity &);
    virtual ~BaseWorld();

    virtual Entity * addObject(Entity * obj) = 0;
    virtual Entity * addObject(const std::string &,
                               const Atlas::Message::Object &,
                               const std::string & id = std::string()) = 0;
    // void delObject(Entity * obj);

    Entity * getObject(const std::string & fid) {
        return eobjects[fid];
    }

    const edict_t & getObjects() {
        return eobjects;
    }

    Entity * findObject(const std::string & fid) {
        return eobjects[fid];
    }

    const double & getTime() const {
        return realTime;
    }

    virtual oplist message(RootOperation & op, const Entity * obj) = 0;
    virtual oplist message(const RootOperation & op) = 0;
    virtual oplist operation(const RootOperation & op) = 0;

    virtual oplist lookOperation(const Look & op) = 0;
};

#endif // COMMON_BASE_WORLD_H
