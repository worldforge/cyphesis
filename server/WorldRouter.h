// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_WORLD_ROUTER_H
#define SERVER_WORLD_ROUTER_H

#include "common/BaseWorld.h"
#include "common/globals.h"
#include "common/serialno.h"

extern "C" {
    #include <sys/time.h>
    #include <unistd.h>
}

class WorldRouter;
class Entity;
class World;

class WorldRouter : public BaseWorld {
  private:
    OpQueue operationQueue;
    time_t initTime;
    EntitySet objectList;
    EntitySet perceptives;
    EntitySet omnipresentList;
    int nextId;

    int getSerialNo() {
        return opSerialNo();
    }

    void addOperationToQueue(RootOperation & op, const BaseEntity *);
    RootOperation * getOperationFromQueue();
    const EntitySet & broadcastList(const RootOperation & op) const;
    inline void updateTime();
    const std::string getNewId(const std::string & name);
    void deliverTo(const RootOperation & op, Entity * e);
  public:
    explicit WorldRouter();
    virtual ~WorldRouter();

    int idle();
    Entity * addObject(Entity * obj, bool setup = true);
    Entity * addObject(const std::string &,
                       const Atlas::Message::Object::MapType &,
                       const std::string & id = std::string());
    void delObject(Entity * obj);
    void setSerialno(OpVector &);
    void setSerialnoOp(RootOperation &);

    virtual OpVector message(RootOperation & op, const Entity * obj);
    virtual OpVector message(const RootOperation & op);
    virtual OpVector operation(const RootOperation & op);

    virtual OpVector LookOperation(const Look & op);

    virtual Entity * findByName(const std::string & name);
    virtual Entity * findByType(const std::string & type);
};

#endif // SERVER_WORLD_ROUTER_H
