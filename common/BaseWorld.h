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

// $Id$

#ifndef COMMON_BASE_WORLD_H
#define COMMON_BASE_WORLD_H

#include "globals.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <sigc++/signal.h>

class Character;
class Entity;
class Task;

typedef std::map<long, Entity *> EntityDict;

/// \brief Base class for game world manager object.
///
/// This base class provides the common features required by cyphesis
/// for the object which encapsulates the game world. Other classes
/// inherit from this provide the core game world system.
class BaseWorld {
  private:
    /// \brief Copy constructor private and un-implemented to prevent slicing
    BaseWorld(const BaseWorld &);
    /// \brief Assignment operator private and un-implemented to prevent slicing
    const BaseWorld & operator=(const BaseWorld &);

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

    explicit BaseWorld(Entity &);
  public:
    /// \brief The top level in-game entity in the world.
    Entity & m_gameWorld;

    virtual ~BaseWorld();

    /// \brief Singleton accessor for the World manager object.
    static BaseWorld & instance() {
        return *m_instance;
    }

    Entity * getEntity(const std::string & id) const;

    Entity * getEntity(long id) const;

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

    /// \brief Main world loop function.
    virtual bool idle(int, int) = 0;

    /// \brief Add a new entity to the world.
    virtual Entity * addEntity(Entity * obj) = 0;

    /// \brief Create a new entity and add to the world.
    virtual Entity * addNewEntity(const std::string & type,
                                  const Atlas::Objects::Entity::RootEntity &) = 0;

    virtual int createSpawnPoint(const Atlas::Message::MapType & data,
                                 Entity * ent) = 0;

    virtual int getSpawnList(Atlas::Message::ListType & data) = 0;

    virtual Entity * spawnNewEntity(const std::string & name,
                                    const std::string & type,
                                    const Atlas::Objects::Entity::RootEntity &) = 0;

    /// \brief Create a new task
    virtual Task * newTask(const std::string &, Character &) = 0;

    /// \brief Activate a new tast
    virtual Task * activateTask(const std::string &, const std::string &,
                                const std::string &, Character &) = 0;

    /// \brief Pass an operation to the world.
    virtual void message(const Atlas::Objects::Operation::RootOperation &,
                         Entity & obj) = 0;

    /// \brief Find an entity of the given name.
    virtual Entity * findByName(const std::string & name) = 0;

    /// \brief Find an entity of the given type.
    virtual Entity * findByType(const std::string & type) = 0;

    /// \brief Add an entity provided to the list of perceptive entities.
    virtual void addPerceptive(Entity *) = 0;

    /// \brief Signal that an operation is being dispatched.
    sigc::signal<void, Atlas::Objects::Operation::RootOperation> Dispatching;
};

#endif // COMMON_BASE_WORLD_H
