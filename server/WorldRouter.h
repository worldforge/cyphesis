// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_WORLD_ROUTER_H
#define SERVER_WORLD_ROUTER_H

#include <sys/time.h>
#include <unistd.h>

class WorldRouter;
class ServerRouting;
class Entity;
class World;

#include <common/BaseWorld.h>
#include <common/globals.h>

class WorldRouter : public BaseWorld {
  private:
    opqueue operationQueue;
    time_t initTime;
    elist_t perceptives;
    elist_t omnipresentList;
    int nextId;

    void addOperationToQueue(RootOperation & op, const BaseEntity *);
    RootOperation * getOperationFromQueue();
    const elist_t & broadcastList(const RootOperation & op) const;
    oplist operation(const RootOperation * op);

    void updateTime() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        double tmp_time = (double)(tv.tv_sec - initTime) + (double)tv.tv_usec/1000000;
        realTime = tmp_time;
    }

    std::string getNewId(std::string & name);
  public:
    ServerRouting & server;

    WorldRouter(ServerRouting & server);
    virtual ~WorldRouter();

    int idle();
    Entity * addObject(Entity * obj);
    Entity * addObject(const std::string &, const Atlas::Message::Object &,
                       const std::string & id = std::string());
    void delObject(Entity * obj);

    const double upTime() const {
        return realTime - timeoffset;
    }

    virtual oplist message(RootOperation & op, const Entity * obj);
    virtual oplist message(const RootOperation & op);
    virtual oplist operation(const RootOperation & op);

    virtual oplist lookOperation(const Look & op);

};

#endif // SERVER_WORLD_ROUTER_H
