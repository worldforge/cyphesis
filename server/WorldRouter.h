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
    OpQueue m_operationQueue;
    time_t m_initTime;
    EntitySet m_objectList;
    EntitySet m_perceptives;
    EntitySet m_omnipresentList;

    int getSerialNo() {
        return opSerialNo();
    }

    void addOperationToQueue(RootOperation & op, const BaseEntity *);
    RootOperation * getOperationFromQueue();
    const EntitySet & broadcastList(const RootOperation & op) const;
    inline void updateTime();
    void deliverTo(const RootOperation & op, Entity * e);
  public:
    explicit WorldRouter();
    virtual ~WorldRouter();

    bool idle();
    Entity * addObject(Entity * obj, bool setup = true);
    Entity * addNewObject(const std::string &,
                          const Atlas::Message::Element::MapType &);
    void delObject(Entity * obj);
    void setSerialno(OpVector &);
    void setSerialnoOp(RootOperation &);

    virtual OpVector message(RootOperation & op, const Entity * obj);
    virtual OpVector message(const RootOperation & op);
    virtual OpVector operation(const RootOperation & op);

    virtual OpVector LookOperation(const Look & op);

    virtual Entity * findByName(const std::string & name);
    virtual Entity * findByType(const std::string & type);
    virtual float constrainHeight(Entity *, const Vector3D &);
};

#endif // SERVER_WORLD_ROUTER_H
