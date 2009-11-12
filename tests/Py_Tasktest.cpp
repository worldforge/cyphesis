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

    assert(PyRun_SimpleString("from server import Task") == 0);
    assert(PyRun_SimpleString("Task()") == -1);
    assert(PyRun_SimpleString("Task(1)") == -1);
    assert(PyRun_SimpleString("Task('1')") == -1);
    assert(PyRun_SimpleString("from server import Character") == 0);
    assert(PyRun_SimpleString("c=Character('1')") == 0);
    assert(PyRun_SimpleString("t=Task(c)") == 0);
    assert(PyRun_SimpleString("Task(t)") == 0);
    assert(PyRun_SimpleString("t==Task(c)") == 0);
    assert(PyRun_SimpleString("print t.character") == 0);
    assert(PyRun_SimpleString("print t.progress") == 0);
    assert(PyRun_SimpleString("print t.rate") == 0);
    assert(PyRun_SimpleString("print t.foo") == -1);
    assert(PyRun_SimpleString("t.progress = 0") == 0);
    assert(PyRun_SimpleString("t.progress = 0.5") == 0);
    assert(PyRun_SimpleString("t.progress = '1'") == -1);
    assert(PyRun_SimpleString("t.rate = 0") == 0);
    assert(PyRun_SimpleString("t.rate = 0.5") == 0);
    assert(PyRun_SimpleString("t.rate = '1'") == -1);
    assert(PyRun_SimpleString("t.foo = 1") == 0);
    assert(PyRun_SimpleString("t.foo = 1.1") == 0);
    assert(PyRun_SimpleString("t.foo = 'foois1'") == 0);
    assert(PyRun_SimpleString("print t.foo") == 0);
    assert(PyRun_SimpleString("print t.obsolete()") == 0);
    assert(PyRun_SimpleString("print t.count()") == 0);
    assert(PyRun_SimpleString("print t.new_tick()") == 0);
    assert(PyRun_SimpleString("print t.next_tick(1)") == 0);
    assert(PyRun_SimpleString("print t.next_tick(1.1)") == 0);
    assert(PyRun_SimpleString("print t.next_tick('1')") == -1);
    assert(PyRun_SimpleString("t.irrelevant()") == 0);
    assert(PyRun_SimpleString("print t.obsolete()") == 0);

#ifndef NDEBUG
    assert(PyRun_SimpleString("import sabotage") == 0);
    // Hit the assert checks.
    assert(PyRun_SimpleString("irrelevant_methd=t.irrelevant") == 0);
    assert(PyRun_SimpleString("obsolete_methd=t.obsolete") == 0);
    assert(PyRun_SimpleString("count_methd=t.count") == 0);
    assert(PyRun_SimpleString("new_tick_methd=t.new_tick") == 0);
    assert(PyRun_SimpleString("next_tick_methd=t.next_tick") == 0);

    assert(PyRun_SimpleString("sabotage.null(t)") == 0);

    assert(PyRun_SimpleString("irrelevant_methd()") == -1);
    assert(PyRun_SimpleString("obsolete_methd()") == -1);
    assert(PyRun_SimpleString("count_methd()") == -1);
    assert(PyRun_SimpleString("new_tick_methd()") == -1);
    assert(PyRun_SimpleString("next_tick_methd(1.1)") == -1);

    assert(PyRun_SimpleString("t.progress") == -1);
    assert(PyRun_SimpleString("t.progress = 0") == -1);
    assert(PyRun_SimpleString("t==Task(c)") == -1);
    assert(PyRun_SimpleString("Task(t)") == -1);

    assert(PyRun_SimpleString("c2=Character('2')") == 0);
    assert(PyRun_SimpleString("sabotage.null(c2)") == 0);
    assert(PyRun_SimpleString("t=Task(c2)") == -1);
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
