// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef WORLD_ROUTER_H
#define WORLD_ROUTER_H

#include <fstream.h>
#include <sys/time.h>
#include <unistd.h>

class WorldRouter;
class ServerRouting;
class Thing;

#include "Routing.h"
#include "WorldTime.h"

#include <common/WorldInfo.h>

typedef std::list<RootOperation *> opqueue;

class WorldRouter : public Routing {
    double realTime;
    opqueue operationQueue;
    list_t objectList;
    time_t initTime;

    void addOperationToQueue(RootOperation & op, const BaseEntity *);
    RootOperation * getOperationFromQueue();
    string getId(string & name);
    const list_t & broadcastList(const RootOperation & op) const;
    oplist operation(const RootOperation * op);
  public:
    ServerRouting & server;
    int nextId;
    list_t perceptives;
    list_t omnipresentList;

    WorldRouter(ServerRouting & server);
    virtual ~WorldRouter() { }

    int idle();

    Thing * addObject(Thing * obj);
    Thing * addObject(const string &, const Atlas::Message::Object &,
                       const string & id = string());
    void delObject(BaseEntity * obj);
        
    void updateTime() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        double tmp_time = (double)(tv.tv_sec - initTime) + (double)tv.tv_usec/1000000;
        world_info::time = double(tmp_time);
        realTime = world_info::time;
    }

    double upTime() {
        return realTime - initTime;
    }

    virtual oplist message(RootOperation & op, const BaseEntity * obj);
    virtual oplist message(const RootOperation & op);
    virtual oplist operation(const RootOperation & op);
    virtual oplist Operation(const Look & op);
};

#endif /* WORLD_ROUTER_H */
