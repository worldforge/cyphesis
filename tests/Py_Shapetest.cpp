// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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
#include "rulesets/Py_Shape.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyShape * o)
{
    if (PyShape_Check(o)) {
#ifdef CYPHESIS_DEBUG
        o->shape = NULL;
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
    init_python_api("3f71a0b3-8b4c-4efc-9835-e32928c049c3");

    setup_test_functions();

    run_python_string("import atlas");
    run_python_string("import server");
    run_python_string("import physics");
    run_python_string("physics.Shape()");
    run_python_string("s = physics.Shape({'type': 'polygon',"
                                         "'points': [[ 0.0, 0.0 ],"
                                                    "[ 1.0, 0.0 ],"
                                                    "[ 1.0, 1.0 ]] })");
    expect_python_error("physics.Shape({})", PyExc_TypeError);
    expect_python_error("s = physics.Shape({'type': 'polygon',"
                                           "'data': object(),"
                                           "'points': [[ 0.0, 0.0 ],"
                                                      "[ 1.0, 0.0 ],"
                                                      "[ 1.0, 1.0 ]] })",
                        PyExc_TypeError);
    run_python_string("s.area()");
    run_python_string("s.footprint()");
    run_python_string("s.low_corner()");
    run_python_string("s.high_corner()");
    run_python_string("s.as_data()");
    run_python_string("repr(s)");
    run_python_string("len(s)");
    run_python_string("s *= 5.0");
    expect_python_error("s *= 5", PyExc_TypeError);
    // Can't send attributes yet
    expect_python_error("s.points =  [[ 0.0, 0.0 ],"
                                     "[ 1.0, 0.0 ],"
                                     "[ 1.0, 1.0 ]]", PyExc_AttributeError);


    expect_python_error("physics.Polygon()", PyExc_TypeError);
    run_python_string("p = physics.Polygon([[ 0.0, 0.0 ],"
                                           "[ 1.0, 0.0 ],"
                                           "[ 1.0, 1.0 ]])");
    run_python_string("p.area()");
    run_python_string("p.footprint()");
    run_python_string("p.low_corner()");
    run_python_string("p.high_corner()");
    run_python_string("d = p.as_data()");
    run_python_string("repr(p)");
    run_python_string("len(p)");
    run_python_string("p *= 5.0");
#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");
    // Hit the assert checks.
    run_python_string("s=physics.Shape()");
    run_python_string("sabotage.null(s)");
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
