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

class BaseWorld {
  private:
    const std::string m_id;

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

    Entity * getObject(const std::string & fid) const {
        EntityDict::const_iterator I = m_eobjects.find(fid);
        if (I != m_eobjects.end()) {
            return I->second;
        } else {
            return NULL;
        }
    }

    const EntityDict & getObjects() const {
        return m_eobjects;
    }

    const double & getTime() const {
        return m_realTime;
    }

    const double upTime() const {
        return m_realTime - timeoffset;
    }

    virtual bool idle() = 0;
    virtual Entity * addObject(Entity * obj, bool setup = true) = 0;
    virtual Entity * addNewObject(const std::string &,
                                  const Atlas::Message::MapType &) = 0;
    virtual void setSerialnoOp(Atlas::Objects::Operation::RootOperation &) = 0;
    virtual void message(Atlas::Objects::Operation::RootOperation &,
                         const Entity * obj) = 0;
    virtual Entity * findByName(const std::string & name) = 0;
    virtual Entity * findByType(const std::string & type) = 0;
    virtual float constrainHeight(Entity *, const Point3D &) = 0;
    virtual void addPerceptive(const std::string & id) = 0;


    SigC::Signal1<void, Atlas::Objects::Operation::RootOperation *> Dispatching;
};

#endif // COMMON_BASE_WORLD_H
