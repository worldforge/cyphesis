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
#include "rulesets/Python_Script_Utils.h"
#include "rulesets/Py_Location.h"

#include "stubs/navigation/stubAwareness.h"
#include "stubs/navigation/stubSteering.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyLocation * o)
{
    if (!PyLocation_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return nullptr;
    }
#ifdef CYPHESIS_DEBUG
    o->location = nullptr;
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

    init_python_api("1a6c913e-79b2-4fc8-9467-ee6c39d0f674");

    PyObject * mod = Get_PyModule("notamodule");
    assert(mod == 0);

    mod = Get_PyModule("http.server");
    assert(mod != 0);
    PyObject * cls = Get_PyClass(mod, "BaseHTTPServer", "noclassbythisname");
    assert(cls == 0);
    cls = Get_PyClass(mod, "BaseHTTPServer", "HTTPServer");
    assert(cls != 0);
    cls = Get_PyClass(mod, "BaseHTTPServer", "__all__");
    assert(cls == 0);
    // We don't actually get a class back, because apparantly classes in
    // the library don't inherit from object yet.

    run_python_string("print('hello')");
    run_python_string("import sys");
    run_python_string("sys.stdout.write('hello')");
    expect_python_error("sys.stdout.write(1)", PyExc_TypeError);
    run_python_string("sys.stderr.write('hello')");
    expect_python_error("sys.stderr.write(1)", PyExc_TypeError);

    run_python_string("from common import log");
    run_python_string("log.debug('foo')");
    run_python_string("log.thinking('foo')");

    run_python_string("import atlas");

    run_python_string("l=atlas.Location()");
    run_python_string("atlas.isLocation(l)");
    run_python_string("atlas.isLocation(1)");
    run_python_string("l1=atlas.Location()");
    run_python_string("l2=atlas.Location()");

    run_python_string("import physics");
    expect_python_error("physics.distance_to()", PyExc_TypeError);
    expect_python_error("physics.square_distance()", PyExc_TypeError);
    expect_python_error("physics.square_horizontal_distance()",
                        PyExc_TypeError);
    run_python_string("physics.distance_to(l1, l2)");
    run_python_string("physics.square_distance(l1, l2)");
    run_python_string("physics.square_horizontal_distance(l1, l2)");
    expect_python_error("physics.distance_to('1', l2)", PyExc_TypeError);
    expect_python_error("physics.square_distance('1', l2)", PyExc_TypeError);
    expect_python_error("physics.square_horizontal_distance('1', l2)",
                        PyExc_TypeError);

#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");
    run_python_string("sabotage.null(l1)");
    expect_python_error("physics.distance_to(l1, l2)", PyExc_AssertionError);
    expect_python_error("physics.distance_to(l2, l1)", PyExc_AssertionError);
    expect_python_error("physics.square_distance(l1, l2)",
                        PyExc_AssertionError);
    expect_python_error("physics.square_distance(l2, l1)",
                        PyExc_AssertionError);
    expect_python_error("physics.square_horizontal_distance(l1, l2)",
                        PyExc_AssertionError);
    expect_python_error("physics.square_horizontal_distance(l2, l1)",
                        PyExc_AssertionError);
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
