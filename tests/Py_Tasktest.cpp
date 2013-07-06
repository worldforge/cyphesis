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
#include "rulesets/Py_Task.h"
#include "rulesets/Py_Thing.h"
#include "rulesets/Character.h"
#include "rulesets/Task.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyTask * o)
{
    if (PyTask_Check(o)) {
#ifdef CYPHESIS_DEBUG
        o->m_task = NULL;
#endif // NDEBUG
    } else if (PyLocatedEntity_Check(o)) {
#ifdef CYPHESIS_DEBUG
        ((PyEntity*)o)->m_entity.l = 0;
#endif // NDEBUG
    } else {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef sabotage_methods[] = {
    {"null", (PyCFunction)null_wrapper,                 METH_O},
    {NULL,          NULL}                       /* Sentinel */
};

static void setup_test_functions()
{
    PyObject * sabotage = Py_InitModule("sabotage", sabotage_methods);
    assert(sabotage != 0);
}

int main()
{
    init_python_api("6715c02a-cc63-497b-988d-453579eae35d");

    setup_test_functions();

    PyTask * task = newPyTask();
    assert(task != 0);

    run_python_string("from server import Task");
    expect_python_error("Task()", PyExc_TypeError);
    expect_python_error("Task(1)", PyExc_TypeError);
    expect_python_error("Task('1')", PyExc_TypeError);
    run_python_string("from server import Character");
    run_python_string("c=Character('1')");
    run_python_string("t=Task(c)");
    run_python_string("Task(t)");
    run_python_string("t==Task(c)");
    run_python_string("assert t.character == c");
    run_python_string("print t.progress");
    run_python_string("print t.rate");
    expect_python_error("print t.foo", PyExc_AttributeError);
    run_python_string("t.progress = 0");
    run_python_string("t.progress = 0.5");
    expect_python_error("t.progress = '1'", PyExc_TypeError);
    run_python_string("t.rate = 0");
    run_python_string("t.rate = 0.5");
    expect_python_error("t.rate = '1'", PyExc_TypeError);
    // The raw wrapper object has no dict for arbitrary attributes
    expect_python_error("t.foo = 1", PyExc_AttributeError);
    expect_python_error("t.foo = 1.1", PyExc_AttributeError);
    expect_python_error("t.foo = 'foois1'", PyExc_AttributeError);
    expect_python_error("assert t.foo == 'foois1'", PyExc_AttributeError);

    run_python_string("class TaskSubclass(Task): pass");
    run_python_string("t2=TaskSubclass(c)");
    // The subclass should have a dict offset
    run_python_string("t2.foo = 1");
    run_python_string("t2.foo = 1.1");
    run_python_string("t2.foo = 'foois1'");
    run_python_string("assert t2.foo == 'foois1'");
    

    // Tasks do not permit wrappers of core server objects
    // to be stored directly.
    expect_python_error("t.foo = Character('2')", PyExc_TypeError);
    run_python_string("import server");
    expect_python_error("t.foo = server.LocatedEntity('2')", PyExc_TypeError);
    expect_python_error("t.foo = server.Thing('2')", PyExc_TypeError);

    run_python_string("assert not t.obsolete()");
    run_python_string("print t.count()");
    run_python_string("print t.new_tick()");
    run_python_string("print t.next_tick(1)");
    run_python_string("print t.next_tick(1.1)");
    expect_python_error("print t.next_tick('1')", PyExc_TypeError);
    run_python_string("t.irrelevant()");
    run_python_string("assert t.obsolete()");

#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");
    // Hit the assert checks.
    run_python_string("irrelevant_methd=t.irrelevant");
    run_python_string("obsolete_methd=t.obsolete");
    run_python_string("count_methd=t.count");
    run_python_string("new_tick_methd=t.new_tick");
    run_python_string("next_tick_methd=t.next_tick");

    run_python_string("sabotage.null(t)");

    expect_python_error("irrelevant_methd()", PyExc_AssertionError);
    expect_python_error("obsolete_methd()", PyExc_AssertionError);
    expect_python_error("count_methd()", PyExc_AssertionError);
    expect_python_error("new_tick_methd()", PyExc_AssertionError);
    expect_python_error("next_tick_methd(1.1)", PyExc_AssertionError);

    expect_python_error("t.progress", PyExc_AssertionError);
    expect_python_error("t.progress = 0", PyExc_AssertionError);
    expect_python_error("t==Task(c)", PyExc_AssertionError);
    expect_python_error("Task(t)", PyExc_AssertionError);

    run_python_string("c2=Character('2')");
    run_python_string("sabotage.null(c2)");
    expect_python_error("t=Task(c2)", PyExc_AssertionError);
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
