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

#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Objects::Entity::Anonymous;

int main()
{
    database_flag = false;

    WorldRouter * test_world = new WorldRouter;

    Entity * ent1 = test_world->addNewEntity("thing", Anonymous());
    assert(ent1 != 0);

    std::string id;
    long int_id = newId(id);

    Entity * ent2 = new Thing(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_loc = &test_world->m_gameWorld;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);

    delete test_world;

    return 0;
}
