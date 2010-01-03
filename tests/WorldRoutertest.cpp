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

// $Id$

#include "server/WorldRouter.h"

#include "rulesets/Thing.h"

#include "common/globals.h"
#include "common/id.h"
#include "common/Tick.h"

#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Tick;

class TestWorldRouter : public WorldRouter
{
  public:
    Operation test_getOperationFromQueue() {
        return getOperationFromQueue();
    }

    void test_delEntity(Entity * e) {
        delEntity(e);
    }
};

int main()
{
    database_flag = false;

    TestWorldRouter * test_world = new TestWorldRouter;

    Entity * ent1 = test_world->addNewEntity("__no_such_type__", Anonymous());
    assert(ent1 == 0);

    ent1 = test_world->addNewEntity("thing", Anonymous());
    assert(ent1 != 0);

    std::string id;
    long int_id = newId(id);

    Entity * ent2 = new Thing(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_loc = &test_world->m_gameWorld;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);

    test_world->test_getOperationFromQueue();

    Tick tick;
    tick->setFutureSeconds(0);
    tick->setTo(ent2->getId());
    test_world->message(tick, *ent2);

    test_world->test_getOperationFromQueue();

    {
        Atlas::Message::MapType spawn_data;
        test_world->createSpawnPoint(spawn_data, ent2);

        spawn_data["name"] = 1;
        test_world->createSpawnPoint(spawn_data, ent2);

        spawn_data["name"] = "bob";
        test_world->createSpawnPoint(spawn_data, ent2);

        test_world->createSpawnPoint(spawn_data, ent2);
    }
    {
        Atlas::Message::ListType spawn_repr;
        test_world->getSpawnList(spawn_repr);
        assert(!spawn_repr.empty());
    }

    Entity * ent3 = test_world->spawnNewEntity("__no_spawn__",
                                               "character",
                                               Anonymous());
    assert(ent3 == 0);

    ent3 = test_world->spawnNewEntity("bob",
                                      "character",
                                      Anonymous());
    assert(ent3 == 0);

    {
        Atlas::Message::MapType spawn_data;
        spawn_data["name"] = "bob";
        spawn_data["character_types"] = Atlas::Message::ListType(1, "spiddler");
        test_world->createSpawnPoint(spawn_data, ent2);
    }

    ent3 = test_world->spawnNewEntity("bob",
                                      "spiddler",
                                      Anonymous());
    assert(ent3 == 0);

    {
        Atlas::Message::MapType spawn_data;
        spawn_data["name"] = "bob";
        spawn_data["character_types"] = Atlas::Message::ListType(1, "character");
        test_world->createSpawnPoint(spawn_data, ent2);
    }

    ent3 = test_world->spawnNewEntity("bob",
                                      "character",
                                      Anonymous());
    assert(ent3 != 0);

    {
        Atlas::Message::MapType spawn_data;
        spawn_data["name"] = "bob";
        spawn_data["character_types"] = Atlas::Message::ListType(1, "character");
        spawn_data["contains"] = Atlas::Message::ListType(1, "thing");
        test_world->createSpawnPoint(spawn_data, ent2);
    }

    Entity * ent4 = test_world->spawnNewEntity("bob",
                                               "character",
                                               Anonymous());
    assert(ent4 != 0);

    test_world->test_delEntity(&test_world->m_gameWorld);
    test_world->test_delEntity(ent4);
    ent4 = 0;

    delete test_world;

    return 0;
}
