// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

#include "rules/simulation/BaseWorld.h"
#include "common/log.h"

#include <Atlas/Objects/RootOperation.h>

#include <sigc++/functors/ptr_fun.h>

#include <cstdlib>

#include <cassert>

static void test_function(Atlas::Objects::Operation::RootOperation)
{
}

#include "../TestWorld.h"
#include "rules/simulation/Entity.h"

class MyTestWorld : public TestWorld {
  public:
    explicit MyTestWorld(Ref<LocatedEntity> gw) : TestWorld(gw) {
        m_eobjects[gw->getIntId()] = gw;
    }

    void addEntity(const Ref<LocatedEntity>& ent, const Ref<LocatedEntity>& parent) override {
        m_eobjects[ent->getIntId()] = ent;
    }
};

int main()
{
    // We have to use the MyTestWorld class, as it implements the functions
    // missing from BaseWorld interface.

    {
        // Test constructor
        Ref<Entity> wrld(new Entity(1));
        MyTestWorld tw(wrld);
    }

    {
        // Test destructor
        Ref<Entity> wrld(new Entity(1));
        BaseWorld * tw = new MyTestWorld(wrld);

        delete tw;
    }

    {
        // Test constructor sets singleton pointer
        Ref<Entity> wrld(new Entity(1));
        MyTestWorld tw(wrld);

        assert(&BaseWorld::instance() == &tw);
    }

    {
        // Test constructor installs reference to world entity
        Ref<Entity> wrld(new Entity(1));
        MyTestWorld tw(wrld);

        assert(tw.m_gw == wrld);
    }

    {
        // Test retrieving non existent entity by string ID is ok
        Ref<Entity> wrld(new Entity(1));
        MyTestWorld tw(wrld);

        assert(!tw.getEntity("2"));
    }

    {
        // Test retrieving existent entity by string ID is ok
        Ref<Entity> wrld(new Entity(1));
        MyTestWorld tw(wrld);

        LocatedEntity * tc = new Entity(2);

        tw.addEntity(tc, wrld);

        assert(tw.getEntity("2") == tc);
    }

    {
        // Test retrieving existent entity by integer ID is ok
        Ref<Entity> wrld(new Entity(1));
        MyTestWorld tw(wrld);

        LocatedEntity * tc = new Entity(2);

        tw.addEntity(tc, wrld);

        assert(tw.getEntity(2) == tc);
    }

    {
        // Test retrieving non existent entity by integer ID is ok
        Ref<Entity> wrld(new Entity(1));
        MyTestWorld tw(wrld);

        assert(!tw.getEntity(2));
    }

    {
        // Test retrieving reference to all entities is okay and empty
        Ref<Entity> wrld(new Entity(1));
        MyTestWorld tw(wrld);

        assert(tw.getEntities().size() == 1);
    }

    {
        // Test getting the time
        Ref<Entity> wrld(new Entity(1));
        MyTestWorld tw(wrld);

        tw.getTime();
    }

    {
        // Test getting the uptime
        Ref<Entity> wrld(new Entity(1));
        MyTestWorld tw(wrld);

        tw.upTime();
    }

    {
        // Test connecting to the dispatch signal
        Ref<Entity> wrld(new Entity(1));
        MyTestWorld tw(wrld);

        tw.Dispatching.connect(sigc::ptr_fun(&test_function));
    }

    return 0;
}

#include "../stubs/common/stubid.h"
#include "../stubs/common/stublog.h"


int timeoffset = 0;

#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/common/stubRouter.h"
