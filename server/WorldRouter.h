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

/// \brief WorldRouter encapsulates the game world running in the server.
///
/// This class has one instance which manages the game world.
/// It maintains a list of all ih-game (IG) objects in the server.
/// It explicitly also maintains lists of perceptive and omnipresent
/// entities.
class WorldRouter : public BaseWorld {
  private:
    /// An ordered queue of operations yet to be dispatched.
    OpQueue m_operationQueue;
    /// The system time when the server was started.
    time_t m_initTime;
    /// List of all IG entities.
    EntitySet m_objectList;
    /// List of perceptive entities.
    EntitySet m_perceptives;
    /// List of omnipresent entities. Obsolete.
    EntitySet m_omnipresentList;

    /// Generate a new operation serial number.
    int newSerialNo() {
        return opSerialNo();
    }

    void addOperationToQueue(RootOperation & op, const Entity *);
    RootOperation * getOperationFromQueue();
    const EntitySet & broadcastList(const RootOperation & op) const;
    void updateTime();
    void deliverTo(const RootOperation & op, Entity * e);
    void deliverDeleteTo(const RootOperation & op, Entity * e);
  public:
    explicit WorldRouter();
    virtual ~WorldRouter();

    bool idle();
    Entity * addObject(Entity * obj, bool setup = true);
    Entity * addNewObject(const std::string &,
                          const MapType &);
    void delObject(Entity * obj);
    void setSerialnoOp(RootOperation &);

    void operation(RootOperation & op);

    virtual void addPerceptive(const std::string &);
    virtual void message(RootOperation & op, const Entity * obj);
    virtual Entity * findByName(const std::string & name);
    virtual Entity * findByType(const std::string & type);
    virtual float constrainHeight(Entity *, const Point3D &);
};

#endif // SERVER_WORLD_ROUTER_H
