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


#ifdef NDEBUG
#undef NDEBUG
#else
#define CYPHESIS_DEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "../python_testers.h"

#include "pythonbase/Python_API.h"
#include "rules/Py_Shape.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyShape * o)
{
    if (PyShape_Check(o)) {
#ifdef CYPHESIS_DEBUG
        o->shape.s = nullptr;
#endif // NDEBUG
    } else {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return nullptr;
    }
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
    setupPythonMalloc();
    {
        PyImport_AppendInittab("sabotage", &init_sabotage);

        init_python_api("3f71a0b3-8b4c-4efc-9835-e32928c049c3");

        run_python_string("import atlas");
        run_python_string("import server");
        run_python_string("import physics");
        run_python_string("physics.Shape()");
        expect_python_error("physics.Shape(object(), object())", PyExc_TypeError);
        run_python_string("s = physics.Shape({'type': 'polygon',"
                          "'points': [[ 0.0, 0.0 ],"
                          "[ 1.0, 0.0 ],"
                          "[ 1.0, 1.0 ]] })");
        expect_python_error("physics.Shape({})", PyExc_TypeError);
        expect_python_error("physics.Shape(object())", PyExc_TypeError);
        expect_python_error("physics.Shape({'type': 'polygon',"
                            "'data': object(),"
                            "'points': [[ 0.0, 0.0 ],"
                            "[ 1.0, 0.0 ],"
                            "[ 1.0, 1.0 ]] })",
                            PyExc_TypeError);
        run_python_string("physics.Shape(atlas.Message({'type': 'polygon',"
                          "'points': [[ 0.0, 0.0 ],"
                          "[ 1.0, 0.0 ],"
                          "[ 1.0, 1.0 ]] }))");
        expect_python_error("physics.Shape(atlas.Message('foo'))", PyExc_TypeError);
        expect_python_error("physics.Shape(atlas.Message({'type': 'polygon',"
                            "'points': [[ 0.0, 0.0 ],"
                            "[ 1.0 ],"
                            "[ 1.0, 1.0 ]] }))",
                            PyExc_TypeError);
        run_python_string("s.area()");
        run_python_string("s.footprint()");
        run_python_string("s.as_data()");
        run_python_string("repr(s)");
        run_python_string("assert(len(s) == 3)");
        run_python_string("s *= 5.0");
        expect_python_error("s *= 5", PyExc_TypeError);
        // Can't send attributes yet
        expect_python_error("s.points =  [[ 0.0, 0.0 ],"
                            "[ 1.0, 0.0 ],"
                            "[ 1.0, 1.0 ]]", PyExc_AttributeError);

        //////////////////////////////// Box /////////////////////////////////
        run_python_string("b = physics.Box()");
        expect_python_error("physics.Box([[ 0.0, 0.0 ],"
                            "[ 1.0, 0.0 ],"
                            "[ 1.0, 1.0 ]])", PyExc_TypeError);
        run_python_string("b.area()");
        run_python_string("b.centre()");
        run_python_string("b.footprint()");
        run_python_string("b.low_corner()");
        run_python_string("b.high_corner()");
        run_python_string("b.extrude(0, 1)");
        expect_python_error("b.extrude()", PyExc_TypeError);
        run_python_string("d = b.as_data()");
        run_python_string("repr(b)");
        run_python_string("assert(len(b) == 4)");
        run_python_string("b[0]");
        run_python_string("b[1]");
        run_python_string("b[2]");
        run_python_string("b[3]");
        expect_python_error("b[4]", PyExc_IndexError);
        run_python_string("b *= 5.0");

        /////////////////////////////// Course ///////////////////////////////
        run_python_string("c = physics.Course()");
        run_python_string("c.area()");
        run_python_string("c.centre()");
        run_python_string("c.footprint()");
        run_python_string("c.low_corner()");
        run_python_string("c.high_corner()");
        run_python_string("c.as_data()");
        run_python_string("repr(c)");
        run_python_string("assert(len(c) == 0)");
        expect_python_error("c[0]", PyExc_IndexError);
        run_python_string("c *= 5.0");

        //////////////////////////////// Line ////////////////////////////////
        expect_python_error("physics.Line()", PyExc_TypeError);
        run_python_string("l = physics.Line([[ 0.0, 0.0 ],"
                          "[ 1.0, 0.0 ],"
                          "[ 1.0, 1.0 ]])");
        run_python_string("l.area()");
        run_python_string("l.centre()");
        run_python_string("l.footprint()");
        run_python_string("l.low_corner()");
        run_python_string("l.high_corner()");
        run_python_string("d = l.as_data()");
        run_python_string("repr(l)");
        run_python_string("len(l)");
        run_python_string("l *= 5.0");

        ////////////////////////////// Polygon ///////////////////////////////
        expect_python_error("physics.Polygon()", PyExc_TypeError);
        expect_python_error("physics.Polygon(object())", PyExc_TypeError);
        expect_python_error("physics.Polygon([[ object(), object() ],"
                            "[ 1.0, 0.0 ],"
                            "[ 1.0, 1.0 ]])", PyExc_TypeError);
        // One vector too short
        expect_python_error("physics.Polygon([[ 0.0, 0.0 ],"
                            "[ 1.0 ],"
                            "[ 1.0, 1.0 ]])", PyExc_TypeError);
        // Sequence too short for complete polygon
        expect_python_error("physics.Polygon([[ 0.0, 0.0 ],"
                            "[ 1.0, 1.0 ]])", PyExc_TypeError);
        run_python_string("physics.Polygon(atlas.Message([[ 0.0, 0.0 ],"
                          "[ 1.0, 0.0 ],"
                          "[ 1.0, 1.0 ]]))");
        expect_python_error("physics.Polygon(atlas.Message('foo'))", PyExc_TypeError);
        run_python_string("p = physics.Polygon([[ 0.0, 0.0 ],"
                          "[ 1.0, 0.0 ],"
                          "[ 1.0, 1.0 ]])");
        run_python_string("p.area()");
        run_python_string("p.centre()");
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
    }
    shutdown_python_api();
    return 0;
}
