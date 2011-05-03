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
#else
#define CYPHESIS_DEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "python_testers.h"

#include "rulesets/Python_API.h"
#include "rulesets/Py_WorldTime.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyWorldTime * o)
{
    if (!PyWorldTime_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return Py_True;
    }
#ifdef CYPHESIS_DEBUG
    o->time = NULL;
#endif // NDEBUG
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

    PyWorldTime * world_time = newPyWorldTime();
    assert(world_time != 0);

    run_python_string("from server import WorldTime");
    fail_python_string("WorldTime()");
    run_python_string("WorldTime(23)");
    // FIXME This started failing with Python 2.7
    // run_python_string("WorldTime(23.1)");

    run_python_string("w=WorldTime(23)");
    run_python_string("w.season");
    fail_python_string("w.foo");
    run_python_string("w.is_now('morning')");
    fail_python_string("w.is_now(1)");
    run_python_string("w.seconds()");

#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");
    run_python_string("sabotage.null(w)");
    // Hit the assert checks.
    fail_python_string("w.is_now('morning')");
    fail_python_string("w.seconds()");
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
