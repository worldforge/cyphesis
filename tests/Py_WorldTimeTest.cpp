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
#include "rulesets/Py_WorldTime.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyWorldTime * o)
{
    if (!PyWorldTime_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return Py_True;
    }
#ifdef CYPHESIS_DEBUG
    o->time = nullptr;
#endif // NDEBUG
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef sabotage_methods[] = {
    {"null", (PyCFunction)null_wrapper,                 METH_O},
    {nullptr,          nullptr}                       /* Sentinel */
};

static PyObject* init_sabotage() {
    static struct PyModuleDef def = {
            PyModuleDef_HEAD_INIT,
            "sabotage",
            nullptr,
            0,
            sabotage_methods,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    };

    return PyModule_Create(&def);
}

int main()
{
    PyImport_AppendInittab("sabotage", &init_sabotage);
    init_python_api("cc4b05b9-4ff9-4127-85b0-300298d16c3c");

    PyWorldTime * world_time = newPyWorldTime();
    assert(world_time != 0);

    run_python_string("from server import WorldTime");
    expect_python_error("WorldTime()", PyExc_TypeError);
    run_python_string("WorldTime(23)");
    // FIXME This started failing with Python 2.7
    // run_python_string("WorldTime(23.1)");

    run_python_string("w=WorldTime(23)");
    run_python_string("w.season");
    expect_python_error("w.foo", PyExc_AttributeError);
    run_python_string("w.is_now('morning')");
    expect_python_error("w.is_now(1)", PyExc_TypeError);
    run_python_string("w.seconds()");

#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");
    run_python_string("sabotage.null(w)");
    // Hit the assert checks.
    expect_python_error("w.is_now('morning')", PyExc_AssertionError);
    expect_python_error("w.seconds()", PyExc_AssertionError);
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
