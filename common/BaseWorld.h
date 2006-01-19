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
#include "types.h"

#include "physics/Vector3D.h"

#include <Atlas/Message/Element.h>

#include <sigc++/signal.h>

#include <cassert>

class Entity;
class Task;
class Character;

/// \brief Base class for game world object.
///
/// This base class provides the common features required by cyphesis
/// for the object which encapsulates the game world. Other classes
/// inherit from this provide the core game world system.
class BaseWorld {
  private:
    // Private and un-implemented to prevent slicing
    BaseWorld(const BaseWorld &);
    const BaseWorld & operator=(const BaseWorld &);
  protected:
    double m_realTime;
    EntityDict m_eobjects;

    explicit BaseWorld(Entity &);
  public:
    Entity & m_gameWorld;

    virtual ~BaseWorld();

    Entity * getEntity(const std::string & id) const;

    Entity * getEntity(long id) const {
        EntityDict::const_iterator I = m_eobjects.find(id);
        if (I != m_eobjects.end()) {
            assert(I->second != 0);
            return I->second;
        } else {
            return 0;
        }
    }

    const EntityDict & getEntities() const {
        return m_eobjects;
    }

    const double & getTime() const {
        return m_realTime;
    }

    const double upTime() const {
        return m_realTime - timeoffset;
    }

    virtual bool idle(int, int) = 0;
    virtual Entity * addEntity(Entity * obj, bool setup = true) = 0;
    virtual Entity * addNewEntity(const std::string &,
                                  const Atlas::Objects::Entity::RootEntity &) = 0;
    virtual Task * newTask(const std::string &, Character &) = 0;
    virtual void message(const Operation &, Entity & obj) = 0;
    virtual Entity * findByName(const std::string & name) = 0;
    virtual Entity * findByType(const std::string & type) = 0;
    virtual float constrainHeight(Entity *, const Point3D &,
                                  const std::string &) = 0;
    virtual void addPerceptive(const std::string & id) = 0;


    SigC::Signal1<void, Operation> Dispatching;
};

#endif // COMMON_BASE_WORLD_H
