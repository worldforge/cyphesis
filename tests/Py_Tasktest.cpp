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

#include <Python.h>

#include "python_testers.h"

#include "rulesets/Python_API.h"
#include "rulesets/Py_Task.h"
#include "rulesets/Py_Thing.h"
#include "rulesets/Character.h"
#include "rulesets/TaskScript.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyTask * o)
{
    if (PyTask_Check(o)) {
#ifndef NDEBUG
        o->m_task = NULL;
#endif // NDEBUG
    } else if (PyCharacter_Check(o)) {
#ifndef NDEBUG
        ((PyEntity*)o)->m_entity.c = 0;
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
    init_python_api();

    setup_test_functions();

    PyTask * task = newPyTask();
    assert(task != 0);

    run_python_string("from server import Task");
    fail_python_string("Task()");
    fail_python_string("Task(1)");
    fail_python_string("Task('1')");
    run_python_string("from server import Character");
    run_python_string("c=Character('1')");
    run_python_string("t=Task(c)");
    run_python_string("Task(t)");
    run_python_string("t==Task(c)");
    run_python_string("print t.character");
    run_python_string("print t.progress");
    run_python_string("print t.rate");
    fail_python_string("print t.foo");
    run_python_string("t.progress = 0");
    run_python_string("t.progress = 0.5");
    fail_python_string("t.progress = '1'");
    run_python_string("t.rate = 0");
    run_python_string("t.rate = 0.5");
    fail_python_string("t.rate = '1'");
    run_python_string("t.foo = 1");
    run_python_string("t.foo = 1.1");
    run_python_string("t.foo = 'foois1'");
    run_python_string("print t.foo");
    run_python_string("print t.obsolete()");
    run_python_string("print t.count()");
    run_python_string("print t.new_tick()");
    run_python_string("print t.next_tick(1)");
    run_python_string("print t.next_tick(1.1)");
    fail_python_string("print t.next_tick('1')");
    run_python_string("t.irrelevant()");
    run_python_string("print t.obsolete()");

#ifndef NDEBUG
    run_python_string("import sabotage");
    // Hit the assert checks.
    run_python_string("irrelevant_methd=t.irrelevant");
    run_python_string("obsolete_methd=t.obsolete");
    run_python_string("count_methd=t.count");
    run_python_string("new_tick_methd=t.new_tick");
    run_python_string("next_tick_methd=t.next_tick");

    run_python_string("sabotage.null(t)");

    fail_python_string("irrelevant_methd()");
    fail_python_string("obsolete_methd()");
    fail_python_string("count_methd()");
    fail_python_string("new_tick_methd()");
    fail_python_string("next_tick_methd(1.1)");

    fail_python_string("t.progress");
    fail_python_string("t.progress = 0");
    fail_python_string("t==Task(c)");
    fail_python_string("Task(t)");

    run_python_string("c2=Character('2')");
    run_python_string("sabotage.null(c2)");
    fail_python_string("t=Task(c2)");
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
