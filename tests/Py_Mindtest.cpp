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

    assert(PyRun_SimpleString("from server import Mind") == 0);
    assert(PyRun_SimpleString("Mind()") == -1);
    assert(PyRun_SimpleString("Mind(1)") == -1);
    assert(PyRun_SimpleString("Mind('s')") == -1);
    assert(PyRun_SimpleString("m=Mind('1')") == 0);
    assert(PyRun_SimpleString("print m.as_entity()") == 0);
    assert(PyRun_SimpleString("print m.foo_operation()") == -1);
    assert(PyRun_SimpleString("print m.id") == 0);
    assert(PyRun_SimpleString("print m.type") == -1);
    assert(PyRun_SimpleString("print m.map") == 0);
    assert(PyRun_SimpleString("print m.location") == 0);
    assert(PyRun_SimpleString("print m.time") == 0);
    assert(PyRun_SimpleString("print m.contains") == 0);
    assert(PyRun_SimpleString("print m.foo") == -1);
    assert(PyRun_SimpleString("m.map=1") == -1);
    assert(PyRun_SimpleString("m.foo=1") == 0);
    assert(PyRun_SimpleString("m.foo=1.1") == 0);
    assert(PyRun_SimpleString("m.foo='1'") == 0);
    assert(PyRun_SimpleString("print m.foo") == 0);
    assert(PyRun_SimpleString("m.bar=[1]") == 0);
    assert(PyRun_SimpleString("m.baz={'foo': 1}") == 0);
    assert(PyRun_SimpleString("print m.qux") == -1);
    assert(PyRun_SimpleString("print m.baz") == 0);
    assert(PyRun_SimpleString("m==m") == 0);
    assert(PyRun_SimpleString("m2=Mind('2')") == 0);
    assert(PyRun_SimpleString("m2.mind=1") == -1);
    assert(PyRun_SimpleString("m2.mind=m") == 0);
    

#ifndef NDEBUG
    assert(PyRun_SimpleString("import sabotage") == 0);
    // Hit the assert checks.
    assert(PyRun_SimpleString("as_entity_methd=m.as_entity") == 0);
    assert(PyRun_SimpleString("sabotage.null(m)") == 0);
    assert(PyRun_SimpleString("as_entity_methd()") == -1);
    assert(PyRun_SimpleString("m==m") == -1);
    assert(PyRun_SimpleString("print m.id") == -1);
    assert(PyRun_SimpleString("m.foo=1") == -1);
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
