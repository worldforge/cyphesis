// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_WORLD_ROUTER_H
#define SERVER_WORLD_ROUTER_H

#include "common/BaseWorld.h"
#include "common/serialno.h"

#include <list>

extern "C" {
    #include <sys/time.h>
    #include <unistd.h>
}

class Entity;

typedef std::list<Atlas::Objects::Operation::RootOperation *> OpQueue;

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

    void addOperationToQueue(Atlas::Objects::Operation::RootOperation &,
                             const Entity *);
    Atlas::Objects::Operation::RootOperation * getOperationFromQueue();
    const EntitySet & broadcastList(const Atlas::Objects::Operation::RootOperation &) const;
    void updateTime();
    void deliverTo(const Atlas::Objects::Operation::RootOperation &,
                   Entity *);
    void deliverDeleteTo(const Atlas::Objects::Operation::RootOperation &,
                         Entity *);
  public:
    explicit WorldRouter();
    virtual ~WorldRouter();

    bool idle();
    Entity * addObject(Entity * obj, bool setup = true);
    Entity * addNewObject(const std::string &,
                          const Atlas::Message::MapType &);
    void delObject(Entity * obj);
    void setSerialnoOp(Atlas::Objects::Operation::RootOperation &);

    void operation(Atlas::Objects::Operation::RootOperation &);

    virtual void addPerceptive(const std::string &);
    virtual void message(Atlas::Objects::Operation::RootOperation &,
                         const Entity *);
    virtual Entity * findByName(const std::string & name);
    virtual Entity * findByType(const std::string & type);
    virtual float constrainHeight(Entity *, const Point3D &);
};

#endif // SERVER_WORLD_ROUTER_H
