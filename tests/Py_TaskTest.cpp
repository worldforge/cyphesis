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
#else
#define CYPHESIS_DEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "python_testers.h"

#include "rulesets/Python_API.h"
#include "rulesets/Character.h"
#include "rulesets/Task.h"

#include <cassert>
#include <rulesets/python/CyPy_Task.h>


int main()
{
    init_python_api("6715c02a-cc63-497b-988d-453579eae35d");

    Entity entity("", 1);
    auto task = CyPy_Task::wrap(new Task(entity));
    assert(CyPy_Task::check(task));

    run_python_string("from server import Task");
    expect_python_error("Task()", PyExc_IndexError);
    expect_python_error("Task(1)", PyExc_TypeError);
    expect_python_error("Task('1')", PyExc_TypeError);
    run_python_string("from server import Character");
    run_python_string("c=Character('1')");
    run_python_string("t=Task(c)");
    run_python_string("Task(t)");
    run_python_string("t==Task(c)");
    run_python_string("assert t.character == c");
    run_python_string("print(t.progress)");
    run_python_string("print(t.rate)");
    expect_python_error("print(t.foo)", PyExc_AttributeError);
    run_python_string("t.progress = 0");
    run_python_string("t.progress = 0.5");
    expect_python_error("t.progress = '1'", PyExc_TypeError);
    run_python_string("t.rate = 0");
    run_python_string("t.rate = 0.5");
    expect_python_error("t.rate = '1'", PyExc_TypeError);
    run_python_string("t.foo = 1");
    run_python_string("t.foo = 1.1");
    run_python_string("t.foo = 'foois1'");
    run_python_string("assert t.foo == 'foois1'");

    run_python_string("class TaskSubclass(Task): pass");
    run_python_string("t2=TaskSubclass(c)");
    // The subclass should have a dict offset
    run_python_string("t2.foo = 1");
    run_python_string("t2.foo = 1.1");
    run_python_string("t2.foo = 'foois1'");
    run_python_string("assert t2.foo == 'foois1'");
    run_python_string("assert t!=Task(c)");


    // Tasks do not permit wrappers of core server objects
    // to be stored directly.
    expect_python_error("t.foo = Character('2')", PyExc_TypeError);
    run_python_string("import server");
    expect_python_error("t.foo = server.Entity('2')", PyExc_TypeError);
    expect_python_error("t.foo = server.Thing('2')", PyExc_TypeError);

    run_python_string("assert not t.obsolete()");
    run_python_string("print(t.count())");
    run_python_string("print(t.new_tick())");
    run_python_string("print(t.next_tick(1))");
    run_python_string("print(t.next_tick(1.1))");
    expect_python_error("print(t.next_tick('1'))", PyExc_TypeError);
    run_python_string("t.irrelevant()");
    run_python_string("assert t.obsolete()");


    shutdown_python_api();
    return 0;
}
