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

#include "rulesets/Python_API.h"
#include "rulesets/Py_Mind.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyMind * o)
{
    if (!PyMind_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return NULL;
    }
#ifndef NDEBUG
    o->m_mind = NULL;
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

    PyMind * mind = newPyMind();
    assert(mind != 0);

    run_python_string("from server import Mind");
    fail_python_string("Mind()");
    fail_python_string("Mind(1)");
    fail_python_string("Mind('s')");
    run_python_string("m=Mind('1')");
    run_python_string("print m.as_entity()");
    fail_python_string("print m.foo_operation()");
    run_python_string("print m.id");
    fail_python_string("print m.type");
    run_python_string("print m.map");
    run_python_string("print m.location");
    run_python_string("print m.time");
    run_python_string("print m.contains");
    fail_python_string("print m.foo");
    fail_python_string("m.map=1");
    run_python_string("m.foo=1");
    run_python_string("m.foo=1.1");
    run_python_string("m.foo='1'");
    run_python_string("print m.foo");
    run_python_string("m.bar=[1]");
    run_python_string("m.baz={'foo': 1}");
    fail_python_string("print m.qux");
    run_python_string("print m.baz");
    run_python_string("m==m");
    run_python_string("m2=Mind('2')");
    fail_python_string("m2.mind=1");
    run_python_string("m2.mind=m");
    

#ifndef NDEBUG
    run_python_string("import sabotage");
    // Hit the assert checks.
    run_python_string("as_entity_methd=m.as_entity");
    run_python_string("sabotage.null(m)");
    fail_python_string("as_entity_methd()");
    fail_python_string("m==m");
    fail_python_string("print m.id");
    fail_python_string("m.foo=1");
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
