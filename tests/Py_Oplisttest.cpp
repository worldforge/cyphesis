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

    run_python_string("from atlas import Oplist");
    run_python_string("from atlas import Operation");
    run_python_string("m=Oplist()");
    run_python_string("m.append(None)");
    fail_python_string("m.append(1)");
    run_python_string("m.append(Operation('get'))");
    run_python_string("m.append(Oplist())");
    run_python_string("m.append(Oplist(Operation('get')))");
    run_python_string("m += None");
    fail_python_string("m += 1");
    run_python_string("m += Operation('get')");
    run_python_string("m += Oplist()");
    run_python_string("m += Oplist(Operation('get'))");
    run_python_string("len(m)");
    
    fail_python_string("Oplist(1)");
    fail_python_string("Oplist(Operation('get'), 1)");
    fail_python_string("Oplist(Operation('get'), Operation('get'), 1)");
    fail_python_string("Oplist(Operation('get'), Operation('get'), Operation('get'), 1)");
    fail_python_string("Oplist(Operation('get'), Operation('get'), Operation('get'), Operation('get'), Operation('get'))");

#ifndef NDEBUG
    run_python_string("import sabotage");

    // Hit the assert checks.
    run_python_string("arg1=Operation('get')");
    run_python_string("sabotage.null(arg1)");
    run_python_string("m += arg1");
    
    run_python_string("sabotage.null(m)");

    fail_python_string("m.append(None)");
    fail_python_string("m += None");
    fail_python_string("len(m)");

#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
