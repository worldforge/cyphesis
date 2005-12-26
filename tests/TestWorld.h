// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef TESTS_TEST_WORLD_H
#define TESTS_TEST_WORLD_H

#include "common/BaseWorld.h"

/// Test implementation of the BaseWorld interface
class TestWorld : public BaseWorld {
  public:
    explicit TestWorld(Entity & gw) : BaseWorld(gw) { }

    virtual bool idle(int, int) { return false; }
    virtual Entity * addEntity(Entity * ent, bool setup = true) { return 0; }
    virtual Entity * addNewEntity(const std::string &,
                                  const Atlas::Objects::Entity::RootEntity &) {
        return 0;
    }
    virtual Task * newTask(const std::string &, Character &) { return 0; }
    virtual void message(const Operation & op, Entity & ent) { }
    virtual Entity * findByName(const std::string & name) { return 0; }
    virtual Entity * findByType(const std::string & type) { return 0; }
    virtual float constrainHeight(Entity*, const Point3D&,
                                  const std::string&) { return 0.f; }
    virtual void addPerceptive(const std::string &) { }
};

#endif // TESTS_TEST_WORLD_H
