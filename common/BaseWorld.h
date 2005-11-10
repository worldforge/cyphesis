// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_BASE_WORLD_H
#define COMMON_BASE_WORLD_H

#include "globals.h"
#include "types.h"

#include "physics/Vector3D.h"

#include <Atlas/Message/Element.h>

#include <sigc++/signal.h>

class Entity;

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

    Entity * getEntity(const std::string & fid) const {
        EntityDict::const_iterator I = m_eobjects.find(fid);
        if (I != m_eobjects.end()) {
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
    virtual void message(const Operation &, Entity & obj) = 0;
    virtual Entity * findByName(const std::string & name) = 0;
    virtual Entity * findByType(const std::string & type) = 0;
    virtual float constrainHeight(Entity *, const Point3D &,
                                  const std::string &) = 0;
    virtual void addPerceptive(const std::string & id) = 0;


    SigC::Signal1<void, Operation> Dispatching;
};

#endif // COMMON_BASE_WORLD_H
