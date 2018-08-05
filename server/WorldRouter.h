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


#ifndef SERVER_WORLD_ROUTER_H
#define SERVER_WORLD_ROUTER_H

#include "rulesets/BaseWorld.h"
#include "common/OperationsDispatcher.h"

#include <list>
#include <set>
#include <queue>


class Spawn;

typedef std::set<LocatedEntity *> EntitySet;
typedef std::map<std::string, std::pair<Spawn *, std::string>> SpawnDict;

/// \brief WorldRouter encapsulates the game world running in the server.
///
/// This class has one instance which manages the game world.
/// It maintains a list of all ih-game (IG) objects in the server.
/// It explicitly also maintains lists of perceptive entities.
class WorldRouter : public BaseWorld {
  private:

    ///Handles dispatching of operations.
    OperationsDispatcher<LocatedEntity> m_operationsDispatcher;
    /// An ordered queue of suspended operations to be dispatched when resumed.
    std::queue<OpQueEntry<LocatedEntity>> m_suspendedQueue;
    /// Count of in world entities
    int m_entityCount;
    /// Map of spawns
    SpawnDict m_spawns;

    /// \brief The top level in-game entity in the world.
    Ref<LocatedEntity> m_baseEntity;
  protected:
    /// \brief Determine if the broadcast is allowed.
    ///
    /// Check the type of operation, and work out if broadcasting is allowed.
    /// @return True if broadcasting is allowed.
    bool shouldBroadcastPerception(const Atlas::Objects::Operation::RootOperation &) const;
    void deliverTo(const Atlas::Objects::Operation::RootOperation &,
                   Ref<LocatedEntity>);
    void resumeWorld() override;
  public:
    explicit WorldRouter(const SystemTime & systemTime, Ref<LocatedEntity> baseEntity);

    ~WorldRouter() override;

    bool idle() override;

    /**
     * Gets the number of seconds until the next operation needs to be dispatched.
     * @return Seconds.
     */
    double secondsUntilNextOp() const;
    Ref<LocatedEntity> addEntity(const Ref<LocatedEntity>& obj) override;
    Ref<LocatedEntity> addNewEntity(const std::string & type,
                                 const Atlas::Objects::Entity::RootEntity &) override;
    void delEntity(LocatedEntity * obj) override;
    int createSpawnPoint(const Atlas::Message::MapType &, LocatedEntity *) override;
    int removeSpawnPoint(LocatedEntity * ent) override;
    int getSpawnList(Atlas::Message::ListType & data) override;
    Ref<LocatedEntity> spawnNewEntity(const std::string &,
                                   const std::string &,
                                   const Atlas::Objects::Entity::RootEntity &) override;
    int moveToSpawn(const std::string & name, Location& location) override;

    ArithmeticScript * newArithmetic(const std::string &,
                                     LocatedEntity *) override;

    void operation(const Atlas::Objects::Operation::RootOperation &,
                   Ref<LocatedEntity>);

    void addPerceptive(LocatedEntity *) override;
    void message(const Atlas::Objects::Operation::RootOperation &,
                         LocatedEntity &) override;

    void messageToClients(const Atlas::Objects::Operation::RootOperation &) override;

    Ref<LocatedEntity> findByName(const std::string & name) override;
    Ref<LocatedEntity> findByType(const std::string & type) override;

    /**
     * @brief Checks if the operation queues have been marked as dirty.
     *
     * This means that something has been added to them.
     * @return True if the queues are dirty.
     */
    bool isQueueDirty() const;

    /**
     * @brief Marks all queues as clean.
     */
    void markQueueAsClean();

    /// \brief Signal that a new Entity has been inserted.
    sigc::signal<void, LocatedEntity *> inserted;

    friend class WorldRoutertest;
    friend class WorldRouterintegration;

    LocatedEntity& getDefaultLocation() const override;
};

#endif // SERVER_WORLD_ROUTER_H
