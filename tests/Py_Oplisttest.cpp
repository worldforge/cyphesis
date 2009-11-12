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
#include "rulesets/Py_Oplist.h"
#include "rulesets/Py_Operation.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyOplist * o)
{
    if (PyOplist_Check(o)) {
#ifndef NDEBUG
        o->ops = NULL;
#endif // NDEBUG
    } else if (PyOperation_Check(o)) {
#ifndef NDEBUG
        ((PyOperation*)o)->operation = Atlas::Objects::Operation::RootOperation(0);
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

    assert(PyRun_SimpleString("from atlas import Oplist") == 0);
    assert(PyRun_SimpleString("from atlas import Operation") == 0);
    assert(PyRun_SimpleString("m=Oplist()") == 0);
    assert(PyRun_SimpleString("m.append(None)") == 0);
    assert(PyRun_SimpleString("m.append(1)") == -1);
    assert(PyRun_SimpleString("m.append(Operation('get'))") == 0);
    assert(PyRun_SimpleString("m.append(Oplist())") == 0);
    assert(PyRun_SimpleString("m.append(Oplist(Operation('get')))") == 0);
    assert(PyRun_SimpleString("m += None") == 0);
    assert(PyRun_SimpleString("m += 1") == -1);
    assert(PyRun_SimpleString("m += Operation('get')") == 0);
    assert(PyRun_SimpleString("m += Oplist()") == 0);
    assert(PyRun_SimpleString("m += Oplist(Operation('get'))") == 0);
    assert(PyRun_SimpleString("len(m)") == 0);
    
    assert(PyRun_SimpleString("Oplist(Operation('get'), Operation('get'), Operation('get'), Operation('get'), Operation('get'))") == -1);
    assert(PyRun_SimpleString("Oplist(1)") == 0);

#ifndef NDEBUG
    assert(PyRun_SimpleString("import sabotage") == 0);

    // Hit the assert checks.
    assert(PyRun_SimpleString("arg1=Operation('get')") == 0);
    assert(PyRun_SimpleString("sabotage.null(arg1)") == 0);
    assert(PyRun_SimpleString("m += arg1") == 0);
    
    assert(PyRun_SimpleString("sabotage.null(m)") == 0);

    assert(PyRun_SimpleString("m.append(None)") == -1);
    assert(PyRun_SimpleString("m += None") == -1);
    assert(PyRun_SimpleString("len(m)") == -1);

#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
