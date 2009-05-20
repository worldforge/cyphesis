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

#include "tests/TestWorld.h"

#include "rulesets/Pedestrian.h"

#include "rulesets/Entity.h"
#include "rulesets/TerrainProperty.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

int main()
{
    Entity * e = new Entity("1", 1);
    Entity * wrld = new Entity("0", 0);
    wrld->setProperty("terrain", new TerrainProperty());
    e->m_location.m_loc = wrld;
    e->m_location.m_loc->makeContainer();
    assert(e->m_location.m_loc->m_contains != 0);
    e->m_location.m_loc->m_contains->insert(e);
    TestWorld test_world(*wrld);

    BaseWorld::instance().addEntity(e);

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
