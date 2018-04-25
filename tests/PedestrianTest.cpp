// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "rulesets/Pedestrian.h"

#include "rulesets/Entity.h"
#include "rulesets/Domain.h"
#include "rulesets/TerrainProperty.h"

#include "common/BaseWorld.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

class TestWorld : public BaseWorld {
  public:
    explicit TestWorld(LocatedEntity & w) : BaseWorld(w) {
    }

    virtual bool idle() { return false; }
    virtual LocatedEntity * addEntity(LocatedEntity * ent) { 
        return 0;
    }
    virtual LocatedEntity * addNewEntity(const std::string &,
                                  const Atlas::Objects::Entity::RootEntity &) {
        return 0;
    }
    void delEntity(LocatedEntity * obj) {}
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         LocatedEntity *) { return 0; }
    int getSpawnList(Atlas::Message::ListType & data) { return 0; }
    int removeSpawnPoint(LocatedEntity *) {return 0; }
    LocatedEntity * spawnNewEntity(const std::string & name,
                            const std::string & type,
                            const Atlas::Objects::Entity::RootEntity & desc) {
        return addNewEntity(type, desc);
    }
    virtual int moveToSpawn(const std::string & name,
                            Location& location){return 0;}
    virtual Task * newTask(const std::string &, LocatedEntity &) { return 0; }
    virtual Task * activateTask(const std::string &, const std::string &,
                                LocatedEntity *, LocatedEntity &) { return 0; }
    virtual ArithmeticScript * newArithmetic(const std::string &, LocatedEntity *) {
        return 0;
    }
    virtual void message(const Atlas::Objects::Operation::RootOperation & op,
                         LocatedEntity & ent) { }
    virtual void messageToClients(const Atlas::Objects::Operation::RootOperation &) { }
    virtual LocatedEntity * findByName(const std::string & name) { return 0; }
    virtual LocatedEntity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(LocatedEntity *) { }
};

int main()
{
    Entity * e = new Entity("1", 1);
    Entity * wrld = new Entity("0", 0);

    e->m_location.m_loc = wrld;
    e->m_location.m_loc->makeContainer();
    assert(e->m_location.m_loc->m_contains != 0);
    e->m_location.m_loc->m_contains->insert(e);

    TestWorld test_world(*wrld);

    Pedestrian * p = new Pedestrian(*e);

    p->getTickAddition(Point3D(0,0,0), Vector3D(1,0,0));

    p->setTarget(Point3D(2,0,0));

    p->getTickAddition(Point3D(0,0,0), Vector3D(1,0,0));

    Location loc;
    p->getUpdatedLocation(loc);

    e->m_location.m_velocity = Vector3D(1,0,0);
    p->getUpdatedLocation(loc);

    e->m_location.m_pos = Point3D(1,0,0);
    p->getUpdatedLocation(loc);

    e->m_location.m_pos = Point3D(2,0,0);
    p->getUpdatedLocation(loc);

    e->m_location.m_pos = Point3D(3,0,0);
    p->getUpdatedLocation(loc);

    p->generateMove(loc);

    delete p;
    return 0;
}

// stubs

#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/common/stubRouter.h"
#include "stubs/rulesets/stubDomain.h"
#include "stubs/common/stubBaseWorld.h"
#include "stubs/modules/stubLocation.h"


float squareDistance(const Point3D & u, const Point3D & v)
{
    return 1.f;
}
