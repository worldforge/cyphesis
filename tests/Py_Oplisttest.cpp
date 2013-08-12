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
#include "rulesets/Py_Oplist.h"
#include "rulesets/Py_Operation.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyOplist * o)
{
    if (PyOplist_Check(o)) {
#ifdef CYPHESIS_DEBUG
        o->ops = NULL;
#endif // NDEBUG
    } else if (PyOperation_Check(o)) {
#ifdef CYPHESIS_DEBUG
        ((PyOperation*)o)->operation = Atlas::Objects::Operation::RootOperation(nullptr);
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
    init_python_api("3803b66a-022e-420e-8e63-30e0c32aaf87");

    setup_test_functions();

    run_python_string("from atlas import Oplist");
    run_python_string("from atlas import Operation");
    run_python_string("m=Oplist()");
    run_python_string("m.append(None)");
    expect_python_error("m.append(1)", PyExc_TypeError);
    run_python_string("m.append(Operation('get'))");
    run_python_string("m.append(Oplist())");
    run_python_string("m.append(Oplist(Operation('get')))");
    run_python_string("len(m)");
    run_python_string("lenm = len(m)");
    run_python_string("m += None");
    run_python_string("len(m) == lenm");
    expect_python_error("m += 1", PyExc_TypeError);
    run_python_string("len(m) == lenm");
    run_python_string("m += Operation('get')");
    run_python_string("len(m) == lenm + 1");
    run_python_string("m += Oplist()");
    run_python_string("len(m) == lenm + 1");
    run_python_string("m += Oplist(Operation('get'))");
    run_python_string("len(m) == lenm + 2");

    run_python_string("n = m + None");
    expect_python_error("m + 1", PyExc_TypeError);
    run_python_string("n = m + Operation('get')");
    run_python_string("assert len(n) == len(m) + 1");
    run_python_string("n = m + Oplist()");
    run_python_string("assert len(n) == len(m)");
    run_python_string("n = m + Oplist(Operation('get'))");
    run_python_string("assert len(n) == len(m) + 1");
    
    expect_python_error("Oplist(1)", PyExc_TypeError);
    expect_python_error("Oplist(Operation('get'), 1)", PyExc_TypeError);
    expect_python_error("Oplist(Operation('get'), Operation('get'), 1)",
                        PyExc_TypeError);
    expect_python_error("Oplist(Operation('get'), Operation('get'), Operation('get'), 1)",
                        PyExc_TypeError);
    expect_python_error("Oplist(Operation('get'), Operation('get'), Operation('get'), Operation('get'), Operation('get'))",
                        PyExc_TypeError);

#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");

    // Hit the assert checks.
    run_python_string("arg1=Operation('get')");
    run_python_string("sabotage.null(arg1)");
    expect_python_error("m += arg1", PyExc_ValueError);
    expect_python_error("n = m + arg1", PyExc_ValueError);
    
    run_python_string("sabotage.null(m)");

    expect_python_error("m.append(None)", PyExc_AssertionError);
    expect_python_error("m += None", PyExc_AssertionError);
    expect_python_error("n = m + None", PyExc_AssertionError);
    expect_python_error("len(m)", PyExc_AssertionError);

#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
