// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_WORLD_ROUTER_H
#define SERVER_WORLD_ROUTER_H

#include "common/BaseWorld.h"

#include <list>

extern "C" {
    #include <sys/time.h>
    #include <unistd.h>
}

class Entity;

struct OpQueEntry {
    Operation & op;
    Entity & from;

    explicit OpQueEntry(Operation & o, Entity & f);
    OpQueEntry(const OpQueEntry & o);
    ~OpQueEntry();

    Operation & operator*() const {
        return op;
    }

    Operation * operator->() const {
        return &op;
    }
};

typedef std::list<OpQueEntry> OpQueue;

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

    void addOperationToQueue(Operation &, Entity &);
    Operation * getOperationFromQueue();
    const EntitySet & broadcastList(const Operation &) const;
    void updateTime(int sec, int usec);
    void deliverTo(const Operation &, Entity &);
    void deliverDeleteTo(const Operation &, Entity &);
    void delEntity(Entity * obj);
  public:
    explicit WorldRouter();
    virtual ~WorldRouter();

    bool idle(int, int);
    Entity * addEntity(Entity * obj, bool setup = true);
    Entity * addNewEntity(const std::string &, const Atlas::Message::MapType &);

    void operation(Operation &, Entity &);

    virtual void addPerceptive(const std::string &);
    virtual void message(Operation &, Entity &);
    virtual Entity * findByName(const std::string & name);
    virtual Entity * findByType(const std::string & type);
    virtual float constrainHeight(Entity *, const Point3D &);
};

#endif // SERVER_WORLD_ROUTER_H
