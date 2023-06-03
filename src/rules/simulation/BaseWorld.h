// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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


#ifndef RULESETS_BASE_WORLD_H
#define RULESETS_BASE_WORLD_H

#include "common/globals.h"
#include "common/Singleton.h"

#include "modules/Ref.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <sigc++/signal.h>
#include <ctime>
#include <boost/noncopyable.hpp>

#include <chrono>
#include <set>
#include <functional>

class LocatedEntity;

class Location;

typedef std::map<long, Ref<LocatedEntity>> EntityRefDict;

/// \brief Base class for game world manager object.
///
/// This base class provides the common features required by cyphesis
/// for the object which encapsulates the game world. Other classes
/// inherit from this provide the core game world system.
class BaseWorld : public Singleton<BaseWorld>
{
    public:
        typedef std::function<std::chrono::steady_clock::duration()> TimeProviderFnType;

    protected:

        TimeProviderFnType m_timeProviderFn;
        /// The system time when the server was started.
        std::chrono::steady_clock::time_point m_initTime;

        /// \brief Dictionary of all the objects in the world.
        ///
        /// Pointers to all in-game entities in the world are stored keyed to
        /// their integer ID.
        EntityRefDict m_eobjects;

        /// \brief Whether the base world is suspended or not.
        ///
        /// If this is set to true, the world is "suspended". In this state no
        /// Tick ops are sent.
        /// This is useful for when a world author wants to edit the world
        /// without the simulation altering it.
        bool m_isSuspended;

        std::map<std::string, LocatedEntity*> m_entityAliases;

        explicit BaseWorld(TimeProviderFnType timeProviderFn);

        /// \brief Called when the world is resumed.
        virtual void resumeWorld()
        {}

    public:

        ~BaseWorld() override;

        /**
         * Shuts down the simulation and frees up all entities.
         */
        virtual void shutdown();

        Ref<LocatedEntity> getEntity(const std::string& id) const;

        Ref<LocatedEntity> getEntity(long id) const;

        /// \brief Read only accessor for the in-game objects dictionary.
        const EntityRefDict& getEntities() const
        {
            return m_eobjects;
        }

        void registerAlias(std::string alias, LocatedEntity& entity);

        void deregisterAlias(const std::string& alias, LocatedEntity& entity);

        LocatedEntity* getAliasEntity(const std::string& alias) const;

        /// \brief Read only accessor for the in-game time.
        std::chrono::steady_clock::duration getTime() const;

        float getTimeAsSeconds() const;

        /// \brief Get the time the world has been running since the server started.
        double upTime() const
        {
            return getTimeAsSeconds();
        }

        /// \brief Gets whether the world is suspended or not.
        const bool& getIsSuspended() const
        {
            return m_isSuspended;
        }

        /// \brief Sets whether the world is suspended or not.
        /// If this is set to true, the world is "suspended". In this state no
        /// Tick ops are sent.
        /// This is useful for when a world author wants to edit the world
        /// without the simulation altering it.
        void setIsSuspended(bool suspended);

        /// \brief Add a new entity to the world.
        virtual void addEntity(const Ref<LocatedEntity>& obj, const Ref<LocatedEntity>& parent) = 0;

        /// \brief Create a new entity and add to the world.
        virtual Ref<LocatedEntity> addNewEntity(const std::string& type,
                                                const Atlas::Objects::Entity::RootEntity&) = 0;

        /// \brief Deletes an entity from the world.
        virtual void delEntity(LocatedEntity* obj) = 0;

        virtual const std::set<std::string>& getSpawnEntities() const = 0;

        virtual void registerSpawner(const std::string& id) = 0;

        virtual void unregisterSpawner(const std::string& id) = 0;

        /// \brief Pass an operation to the world.
        virtual void message(Atlas::Objects::Operation::RootOperation,
                             LocatedEntity& obj) = 0;

        /// \brief Find an entity of the given name.
        virtual Ref<LocatedEntity> findByName(const std::string& name) = 0;

        /// \brief Find an entity of the given type.
        virtual Ref<LocatedEntity> findByType(const std::string& type) = 0;

        /// \brief Signal that an operation is being dispatched.
        sigc::signal<void(Atlas::Objects::Operation::RootOperation)> Dispatching;
};

#endif // RULESETS_BASE_WORLD_H
