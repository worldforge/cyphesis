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


#ifndef COMMON_BASE_WORLD_H
#define COMMON_BASE_WORLD_H

#include "globals.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <sigc++/signal.h>

class ArithmeticScript;
class LocatedEntity;
class SystemTime;
class Task;

typedef std::map<long, LocatedEntity *> EntityDict;

/// \brief Base class for game world manager object.
///
/// This base class provides the common features required by cyphesis
/// for the object which encapsulates the game world. Other classes
/// inherit from this provide the core game world system.
class BaseWorld {
  private:
    /// \brief Copy constructor deleted to prevent slicing
    BaseWorld(const BaseWorld &) = delete;
    /// \brief Assignment operator deleted to prevent slicing
    const BaseWorld & operator=(const BaseWorld &) = delete;

    /// \brief Singleton instance pointer for the World manager object.
    static BaseWorld * m_instance;
  protected:
    /// \brief The in-game time in seconds.
    ///
    /// Calculated from the out-of-game time by applying an offset stored
    /// at startup
    double m_realTime;

    /// \brief Dictionary of all the objects in the world.
    ///
    /// Pointers to all in-game entities in the world are stored keyed to
    /// their integer ID.
    EntityDict m_eobjects;

    /// \brief Whether the base world is suspended or not.
    ///
    /// If this is set to true, the world is "suspended". In this state no
    /// Tick ops are sent.
    /// This is useful for when a world author wants to edit the world
    /// without the simulation altering it.
    bool m_isSuspended;

    explicit BaseWorld(LocatedEntity &);

    /// \brief Called when the world is resumed.
    virtual void resumeWorld() {}

  public:
    /// \brief The top level in-game entity in the world.
    LocatedEntity & m_gameWorld;

    virtual ~BaseWorld();

    /// \brief Singleton accessor for the World manager object.
    static BaseWorld & instance() {
        return *m_instance;
    }

    LocatedEntity * getEntity(const std::string & id) const;

    LocatedEntity * getEntity(long id) const;

    /// \brief Read only accessor for the in-game objects dictionary.
    const EntityDict & getEntities() const {
        return m_eobjects;
    }

    /// \brief Read only accessor for the in-game time.
    const double & getTime() const {
        return m_realTime;
    }

    /// \brief Get the time the world has been running since the server started.
    const double upTime() const {
        return m_realTime - timeoffset;
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
    virtual bool idle(const SystemTime &) = 0;

    /// \brief Add a new entity to the world.
    virtual LocatedEntity * addEntity(LocatedEntity * obj) = 0;

    /// \brief Create a new entity and add to the world.
    virtual LocatedEntity * addNewEntity(const std::string & type,
                                  const Atlas::Objects::Entity::RootEntity &) = 0;

    virtual int createSpawnPoint(const Atlas::Message::MapType & data,
                                 LocatedEntity * ent) = 0;

    virtual int getSpawnList(Atlas::Message::ListType & data) = 0;

    virtual LocatedEntity * spawnNewEntity(
          const std::string & name,
          const std::string & type,
          const Atlas::Objects::Entity::RootEntity &) = 0;

    /// \brief Create a new task
    virtual Task * newTask(const std::string &, LocatedEntity &) = 0;

    /// \brief Activate a new tast
    virtual Task * activateTask(const std::string &, const std::string &,
                                LocatedEntity *, LocatedEntity &) = 0;

    /// \brief Create a new Arithmetic object
    virtual ArithmeticScript * newArithmetic(const std::string &,
                                             LocatedEntity *) = 0;

    /// \brief Pass an operation to the world.
    virtual void message(const Atlas::Objects::Operation::RootOperation &,
                         LocatedEntity & obj) = 0;

    /// \brief Find an entity of the given name.
    virtual LocatedEntity * findByName(const std::string & name) = 0;

    /// \brief Find an entity of the given type.
    virtual LocatedEntity * findByType(const std::string & type) = 0;

    /// \brief Add an entity provided to the list of perceptive entities.
    virtual void addPerceptive(LocatedEntity *) = 0;

    /// \brief Signal that an operation is being dispatched.
    sigc::signal<void, Atlas::Objects::Operation::RootOperation> Dispatching;
};

#endif // COMMON_BASE_WORLD_H
