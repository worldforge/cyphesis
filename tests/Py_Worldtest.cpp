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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "python_testers.h"

#include "TestWorld.h"

#include "rulesets/Python_API.h"

#include "rulesets/Entity.h"

#include <cassert>

int main()
{
    init_python_api("cb44c6cc-64fa-46c4-83d1-f43c6a2bb56c");

    Entity wrld("0", 0);
    TestWorld tw(wrld);


    run_python_string("from server import World");
    run_python_string("w=World()");
    run_python_string("w.get_time()");
    run_python_string("w.get_object('0')");
    run_python_string("w.get_object('1')");
    expect_python_error("w.get_object(1)", PyExc_TypeError);
    run_python_string("w == World()");

    shutdown_python_api();
    return 0;
}

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}
