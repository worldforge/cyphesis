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

class ArithmeticScript;
class LocatedEntity;
class SystemTime;
class Location;

typedef std::map<long, Ref<LocatedEntity>> EntityRefDict;

/// \brief Base class for game world manager object.
///
/// This base class provides the common features required by cyphesis
/// for the object which encapsulates the game world. Other classes
/// inherit from this provide the core game world system.
class BaseWorld : public Singleton<BaseWorld> {

  protected:
    /// The system time when the server was started.
    std::time_t m_initTime;

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

    LocatedEntity* m_defaultLocation;

    LocatedEntity* m_limboLocation;

    explicit BaseWorld();

    /// \brief Called when the world is resumed.
    virtual void resumeWorld() {}

  public:

    ~BaseWorld() override = default;

    /**
     * Shuts down the simulation and frees up all entities.
     */
    virtual void shutdown();

    Ref<LocatedEntity> getEntity(const std::string & id) const;

    Ref<LocatedEntity> getEntity(long id) const;

    /// \brief Read only accessor for the in-game objects dictionary.
    const EntityRefDict & getEntities() const {
        return m_eobjects;
    }

    /// \brief Gets the default location.
    ///
    /// This is where entities will be created if nothing else is specified.
    /// This will either return the root gameworld, or another entity if
    /// setDefaultLocation() has been called.
    virtual LocatedEntity& getDefaultLocation() const = 0;

    /// \brief Sets the default location.
    ///
    /// If a null value is provided the default location will revert back
    /// to the root world.
    void setDefaultLocation(LocatedEntity* entity);

    /// \brief Gets the limbo location.
    ///
    /// This is where player entities will be moved when they are deleted without
    /// there being an active connection. This is to prevent the active world from
    /// filling up with inactive entities.
    /// \returns A valid limbo entity, or null if none is set.
    LocatedEntity* getLimboLocation() const;

    /// \brief Sets the limbo location.
    ///
    /// This is where player entities will be moved when they are deleted without
    /// there being an active connection. This is to prevent the active world from
    /// filling up with inactive entities.
    void setLimboLocation(LocatedEntity* entity);


    /// \brief Read only accessor for the in-game time.
    double getTime() const;

    /// \brief Get the time the world has been running since the server started.
    double upTime() const {
        return getTime() - timeoffset;
    }

    /// \brief Gets whether the world is suspended or not.
    const bool & getIsSuspended() const {
    	return m_isSuspended;
    }

    /// \brief Sets whether the world is suspended or not.
    /// If this is set to true, the world is "suspended". In this state no
    /// Tick ops are sent.
    /// This is useful for when a world author wants to edit the world
    /// without the simulation altering it.
    void setIsSuspended(bool suspended);

    /// \brief Main world loop function.
    virtual bool idle() = 0;

    /// \brief Add a new entity to the world.
    virtual Ref<LocatedEntity> addEntity(const Ref<LocatedEntity>& obj) = 0;

    /// \brief Create a new entity and add to the world.
    virtual Ref<LocatedEntity> addNewEntity(const std::string & type,
                                  const Atlas::Objects::Entity::RootEntity &) = 0;

    /// \brief Deletes an entity from the world.
    virtual void delEntity(LocatedEntity * obj) = 0;

    virtual int createSpawnPoint(const Atlas::Message::MapType & data,
                                 LocatedEntity * ent) = 0;

    /**
     * \brief Removes a previously registered spawn point.
     * @param ent The entity to which the spawn point was registered.
     * @return 0 if successful
     */
    virtual int removeSpawnPoint(LocatedEntity * ent) = 0;

    virtual int getSpawnList(Atlas::Message::ListType & data) = 0;

    virtual Ref<LocatedEntity> spawnNewEntity(
          const std::string & name,
          const std::string & type,
          const Atlas::Objects::Entity::RootEntity &) = 0;

    /**
     * \brief Moves the location to within the spawn specified by the name.
     * \param name The name of the spawn.
     * \param location The location which will be moved.
     * \return 0 if successful.
     */
    virtual int moveToSpawn(const std::string & name,
                            Location& location) = 0;

    /// \brief Create a new Arithmetic object
    virtual ArithmeticScript * newArithmetic(const std::string &,
                                             LocatedEntity *) = 0;

    /// \brief Pass an operation to the world.
    virtual void message(const Atlas::Objects::Operation::RootOperation &,
                         LocatedEntity & obj) = 0;

    /// \brief Sends a message to all connected clients.
    virtual void messageToClients(const Atlas::Objects::Operation::RootOperation &) = 0;

        /// \brief Find an entity of the given name.
    virtual Ref<LocatedEntity> findByName(const std::string & name) = 0;

    /// \brief Find an entity of the given type.
    virtual Ref<LocatedEntity> findByType(const std::string & type) = 0;

    /// \brief Signal that an operation is being dispatched.
    sigc::signal<void, Atlas::Objects::Operation::RootOperation> Dispatching;
};

#endif // RULESETS_BASE_WORLD_H
