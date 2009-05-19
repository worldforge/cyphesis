// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#ifndef SERVER_WORLD_ROUTER_H
#define SERVER_WORLD_ROUTER_H

#include "common/BaseWorld.h"
#include "common/OperationRouter.h"

#include <list>
#include <set>

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
typedef std::set<Entity *> EntitySet;

/// \brief WorldRouter encapsulates the game world running in the server.
///
/// This class has one instance which manages the game world.
/// It maintains a list of all ih-game (IG) objects in the server.
/// It explicitly also maintains lists of perceptive entities.
class WorldRouter : public BaseWorld {
  private:
    /// An ordered queue of operations to be dispatched in the future
    OpQueue m_operationQueue;
    /// An ordered queue of operations to be dispatched now
    OpQueue m_immediateQueue;
    /// The system time when the server was started.
    time_t m_initTime;
    /// List of perceptive entities.
    EntitySet m_perceptives;
    /// Count of in world entities
    int m_entityCount;

    void addOperationToQueue(const Operation &, Entity &);
    Operation getOperationFromQueue();
    bool broadcastPerception(const Operation &) const;
    void updateTime(int sec, int usec);
    void deliverTo(const Operation &, Entity &);
    void delEntity(Entity * obj);
  public:
    explicit WorldRouter();
    virtual ~WorldRouter();

    bool idle(int, int);
    Entity * addEntity(Entity * obj);
    Entity * addNewEntity(const std::string &, const Atlas::Objects::Entity::RootEntity &);
    Task * newTask(const std::string &, Character &);
    Task * activateTask(const std::string &, const std::string &,
                        const std::string &, Character &);

    void operation(const Operation &, Entity &);

    virtual void addPerceptive(Entity *);
    virtual void message(const Operation &, Entity &);
    virtual Entity * findByName(const std::string & name);
    virtual Entity * findByType(const std::string & type);

    /// \brief Signal that a new Entity has been inserted.
    sigc::signal<void, Entity *> inserted;
};

#endif // SERVER_WORLD_ROUTER_H
