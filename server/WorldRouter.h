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
    OpQueue operationQueue;
    time_t initTime;
    EntitySet perceptives;
    EntitySet omnipresentList;
    int nextId;

    void addOperationToQueue(RootOperation & op, const BaseEntity *);
    RootOperation * getOperationFromQueue();
    const EntitySet & broadcastList(const RootOperation & op) const;
    OpVector operation(const RootOperation * op);

    void updateTime() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        double tmp_time = (double)(tv.tv_sec - initTime) + (double)tv.tv_usec/1000000;
        realTime = tmp_time;
    }

    std::string getNewId(const std::string & name);
    void deliverTo(const RootOperation & op, Entity * e);
  public:
    ServerRouting & server;

    explicit WorldRouter(ServerRouting & server);
    virtual ~WorldRouter();

    int idle();
    Entity * addObject(Entity * obj);
    Entity * addObject(const std::string &,
                       const Atlas::Message::Object::MapType &,
                       const std::string & id = std::string());
    void delObject(Entity * obj);

    const double upTime() const {
        return realTime - timeoffset;
    }

    virtual OpVector message(RootOperation & op, const Entity * obj);
    virtual OpVector message(const RootOperation & op);
    virtual OpVector operation(const RootOperation & op);

    virtual OpVector lookOperation(const Look & op);

};

#endif // SERVER_WORLD_ROUTER_H
