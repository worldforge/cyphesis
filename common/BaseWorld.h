// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_BASE_WORLD_H
#define COMMON_BASE_WORLD_H

#include "OOGThing.h"

#include "globals.h"

#include <sigc++/signal.h>

class Entity;

class BaseWorld : public OOGThing {
  private:
    // Private and un-implemented to prevent slicing
    BaseWorld(const BaseWorld &);
    const BaseWorld & operator=(const BaseWorld &);
  protected:
    double m_realTime;
    EntityDict m_eobjects;

  public:
    Entity & m_gameWorld;

    explicit BaseWorld(const std::string &, Entity &);
    virtual ~BaseWorld();

    Entity * getObject(const std::string & fid) {
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

    virtual void idle() = 0;
    virtual Entity * addObject(Entity * obj, bool setup = true) = 0;
    virtual Entity * addNewObject(const std::string &,
                                  const Atlas::Message::Element::MapType &) = 0;
    virtual void setSerialnoOp(RootOperation &) = 0;
    virtual OpVector message(RootOperation & op, const Entity * obj) = 0;
    virtual Entity * findByName(const std::string & name) = 0;
    virtual Entity * findByType(const std::string & type) = 0;

    SigC::Signal1<void, RootOperation *> Dispatching;
};

#endif // COMMON_BASE_WORLD_H
