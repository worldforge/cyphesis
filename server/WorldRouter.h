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

struct OpQueEntry;
#if 0
struct OpQueEntry {
    const Operation & op;
    Entity & from;

    explicit OpQueEntry(const Operation & o, Entity & f);
    OpQueEntry(const OpQueEntry & o);
    ~OpQueEntry();

    Operation & operator*() const {
        return *op.get();
    }

    Atlas::Objects::Operation::RootOperationData * operator->() const {
        return op.get();
    }
};
#endif

typedef std::list<OpQueEntry> OpQueue;

/// \brief WorldRouter encapsulates the game world running in the server.
///
/// This class has one instance which manages the game world.
/// It maintains a list of all ih-game (IG) objects in the server.
/// It explicitly also maintains lists of perceptive and omnipresent
/// entities.
class WorldRouter : public BaseWorld {
  private:
    /// An ordered queue of operations to be dispatched in the future
    OpQueue m_operationQueue;
    /// An ordered queue of operations to be dispatched now
    OpQueue m_immediateQueue;
    /// The system time when the server was started.
    time_t m_initTime;
    /// List of all IG entities.
    EntitySet m_objectList;
    /// List of perceptive entities.
    EntitySet m_perceptives;
    /// List of omnipresent entities. Obsolete.
    EntitySet m_omnipresentList;

    void addOperationToQueue(const Operation &, Entity &);
    Operation getOperationFromQueue();
    const EntitySet & broadcastList(const Operation &) const;
    void updateTime(int sec, int usec);
    void deliverTo(const Operation &, Entity &);
    void delEntity(Entity * obj);
  public:
    explicit WorldRouter();
    virtual ~WorldRouter();

    bool idle(int, int);
    Entity * addEntity(Entity * obj, bool setup = true);
    Entity * addNewEntity(const std::string &, const Atlas::Objects::Entity::RootEntity &);
    Task * newTask(const std::string &, Character &);

    void operation(const Operation &, Entity &);

    virtual void addPerceptive(const std::string &);
    virtual void message(const Operation &, Entity &);
    virtual Entity * findByName(const std::string & name);
    virtual Entity * findByType(const std::string & type);
    virtual float constrainHeight(Entity *, const Point3D &, const std::string &);
};

#endif // SERVER_WORLD_ROUTER_H
