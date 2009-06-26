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

//#include "PropertyCoverage.h"
#include "TestWorld.h"

#include "rulesets/Entity.h"
#include "rulesets/Character.h"
#include "rulesets/TasksProperty.h"
#include "rulesets/TaskScript.h"

#include <Atlas/Message/Element.h>

using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Message::Element;

int main()
{
    TasksProperty * ap = new TasksProperty;

//    PropertyCoverage pc(ap);
//    pc.basicCoverage();

    Entity * const tlve = new Entity("0", 0);
    BaseWorld * const wrld = new TestWorld(*tlve);
    Character * const ent = new Character("1", 1);
    Task * const task = new TaskScript(*ent);

    ent->m_location.m_loc = tlve;
    ent->m_location.m_pos = Point3D(1,0,0);

    tlve->m_contains = new LocatedEntitySet;
    tlve->m_contains->insert(ent);

    task->progress() = .1;
    task->rate() = .1;
    ent->setTask(task);

    Element val;

    MapType map;
    map["one"] = 23;
    map["two"] = 23.;
    map["three"] = "twenty_three";
    map["four"] = ListType(1, 23);
    map["five"] = ListType(1, 23.);
    map["six"] = ListType(1, "twenty_three");

    ap->get(val);

    ap->set(ListType(1, map));

    ap->install(ent);

    ap->get(val);

    ap->set(ListType(1, map));

    ap->set(23);
    ap->set(ListType(1, 23));

    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}
