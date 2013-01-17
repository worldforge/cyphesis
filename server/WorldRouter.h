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

#include <list>
#include <set>

#include <ctime>

class Spawn;

struct OpQueEntry;

typedef std::list<OpQueEntry> OpQueue;
typedef std::set<LocatedEntity *> EntitySet;
typedef std::map<std::string, Spawn *> SpawnDict;

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
    std::time_t m_initTime;
    /// List of perceptive entities.
    EntitySet m_perceptives;
    /// Count of in world entities
    int m_entityCount;
    /// Map of spawns
    SpawnDict m_spawns;

  protected:
    void addOperationToQueue(const Atlas::Objects::Operation::RootOperation &,
                             LocatedEntity &);
    Atlas::Objects::Operation::RootOperation getOperationFromQueue();
    bool broadcastPerception(const Atlas::Objects::Operation::RootOperation &) const;
    void updateTime(const SystemTime &);
    void deliverTo(const Atlas::Objects::Operation::RootOperation &,
                   LocatedEntity &);
    void delEntity(LocatedEntity * obj);
  public:
    explicit WorldRouter(const SystemTime &);
    virtual ~WorldRouter();

    bool idle(const SystemTime &);
    LocatedEntity * addEntity(LocatedEntity * obj);
    LocatedEntity * addNewEntity(const std::string & type,
                                 const Atlas::Objects::Entity::RootEntity &);
    int createSpawnPoint(const Atlas::Message::MapType &, LocatedEntity *);
    int getSpawnList(Atlas::Message::ListType & data);
    LocatedEntity * spawnNewEntity(const std::string &,
                                   const std::string &,
                                   const Atlas::Objects::Entity::RootEntity &);
    Task * newTask(const std::string &, LocatedEntity &);
    Task * activateTask(const std::string &,
                        const std::string &,
                        LocatedEntity *,
                        LocatedEntity &);

    ArithmeticScript * newArithmetic(const std::string &,
                                     LocatedEntity *);

    void operation(const Atlas::Objects::Operation::RootOperation &,
                   LocatedEntity &);

    virtual void addPerceptive(LocatedEntity *);
    virtual void message(const Atlas::Objects::Operation::RootOperation &,
                         LocatedEntity &);
    virtual LocatedEntity * findByName(const std::string & name);
    virtual LocatedEntity * findByType(const std::string & type);

    /// \brief Signal that a new Entity has been inserted.
    sigc::signal<void, LocatedEntity *> inserted;

    friend class WorldRoutertest;
    friend class WorldRouterintegration;
};

#endif // SERVER_WORLD_ROUTER_H
