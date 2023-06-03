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

#include "rules/simulation/BaseWorld.h"
#include "rules/simulation/EntityCreator.h"
#include "common/OperationsDispatcher.h"


#include <list>
#include <set>
#include <queue>


class Spawn;

class EntityBuilder;

/// \brief WorldRouter encapsulates the game world running in the server.
///
/// This class has one instance which manages the game world.
/// It maintains a list of all in-game (IG) objects in the server.
class WorldRouter : public BaseWorld
{
    private:

        ///Handles dispatching of operations.
        OperationsDispatcher<LocatedEntity> m_operationsDispatcher;
        /// An ordered queue of suspended operations to be dispatched when resumed.
        std::queue<OpQueEntry<LocatedEntity>> m_suspendedQueue;
        /// Count of in world entities
        int m_entityCount;

        /**
         * A set of the entities that are registered to be "spawn" entities.
         * These are the main entry into the world for new players, and are allowed to receive Create operations directly from Account instances.
         */
        std::set<std::string> m_spawnEntities;

        /// \brief The top level in-game entity in the world.
        Ref<LocatedEntity> m_baseEntity;
        EntityCreator& m_entityCreator;

    protected:
        /// \brief Determine if the broadcast is allowed.
        ///
        /// Check the type of operation, and work out if broadcasting is allowed.
        /// @return True if broadcasting is allowed.
        bool shouldBroadcastPerception(const Atlas::Objects::Operation::RootOperation&) const;

        void deliverTo(const Atlas::Objects::Operation::RootOperation&,
                       Ref<LocatedEntity>);

        void resumeWorld() override;

        void resolveDispatchTimeForOp(Atlas::Objects::Operation::RootOperationData& op);

    public:


        explicit WorldRouter(Ref<LocatedEntity> baseEntity,
                             EntityCreator& entityCreator,
                             TimeProviderFnType timeProviderFn);

        ~WorldRouter() override;

        void shutdown() override;

        Ref<LocatedEntity> getBaseEntity() const {
            return m_baseEntity;
        }

        void addEntity(const Ref<LocatedEntity>& obj, const Ref<LocatedEntity>& parent) override;

        Ref<LocatedEntity> addNewEntity(const std::string& type,
                                        const Atlas::Objects::Entity::RootEntity&) override;

        void delEntity(LocatedEntity* obj) override;

        const std::set<std::string>& getSpawnEntities() const override;

        void registerSpawner(const std::string& id) override;

        void unregisterSpawner(const std::string& id) override;

        void operation(const Atlas::Objects::Operation::RootOperation&,
                       Ref<LocatedEntity>);

        void message(Atlas::Objects::Operation::RootOperation,
                     LocatedEntity&) override;

        Ref<LocatedEntity> findByName(const std::string& name) override;

        Ref<LocatedEntity> findByType(const std::string& type) override;

        /// \brief Signal that a new Entity has been inserted.
        sigc::signal<void(LocatedEntity&)> inserted;

        friend class WorldRoutertest;

        friend struct WorldRouterintegration;

        OperationsDispatcher<LocatedEntity>& getOperationsHandler();

        /// Count of number of operations handled.
        int m_operationsCount;

};

#endif // SERVER_WORLD_ROUTER_H
