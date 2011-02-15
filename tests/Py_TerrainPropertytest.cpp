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
#include "rulesets/Py_Thing.h"
#include "rulesets/Py_Property.h"
#include "rulesets/Entity.h"
#include "rulesets/TerrainProperty.h"

#include <cassert>

#define PyTerrainProperty_Check(_o) ((_o)->ob_type == &PyTerrainProperty_Type)

static PyObject * add_properties(PyObject * self, PyEntity * o)
{
    if (!PyEntity_Check(o) && !PyCharacter_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return NULL;
    }

    Entity * ent = o->m_entity.e;
    
    PropertyBase * p = ent->setProperty("terrain", new TerrainProperty);
    p->install(ent);
    p->apply(ent);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * null_wrapper(PyObject * self, PyTerrainProperty * o)
{
    if (PyTerrainProperty_Check(o)) {
#ifndef NDEBUG
        o->m_property = NULL;
#endif // NDEBUG
    } else {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef testprop_methods[] = {
    {"add_properties", (PyCFunction)add_properties,                 METH_O},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef sabotage_methods[] = {
    {"null", (PyCFunction)null_wrapper,                 METH_O},
    {NULL,          NULL}                       /* Sentinel */
};

static void setup_test_functions()
{
    PyObject * testprop = Py_InitModule("testprop", testprop_methods);
    assert(testprop != 0);
    PyObject * sabotage = Py_InitModule("sabotage", sabotage_methods);
    assert(sabotage != 0);
}

int main()
{
    init_python_api();

    setup_test_functions();

    run_python_string("from server import *");
    run_python_string("import testprop");
    run_python_string("t=Thing('1')");
    fail_python_string("t.terrain");
    run_python_string("testprop.add_properties(t)");
    run_python_string("terrain = t.terrain");
    fail_python_string("terrain.foo = 1");
    fail_python_string("terrain.get_height()");
    run_python_string("terrain.get_height(0,0)");
    fail_python_string("terrain.get_surface()");
    fail_python_string("terrain.get_surface('1')");
    run_python_string("from physics import *");
    fail_python_string("terrain.get_surface(Point3D(0,0,0))");
    fail_python_string("terrain.get_normal()");
    run_python_string("terrain.get_normal(0,0)");

    run_python_string("points = { }");
    run_python_string("points['-1x-1'] = [-1, -1, -16.8]");
    run_python_string("points['0x-1'] = [0, -1, -3.8]");
    run_python_string("points['-1x0'] = [-1, 0, -2.8]");
    run_python_string("points['-1x1'] = [-1, 1, -1.8]");
    run_python_string("points['1x-1'] = [1, -1, 15.8]");
    run_python_string("points['0x0'] = [0, 0, 12.8]");
    run_python_string("points['1x0'] = [1, 0, 23.1]");
    run_python_string("points['0x1'] = [0, 1, 14.2]");
    run_python_string("points['1x1'] = [1, 1, 19.7]");
    run_python_string("t.terrain = {'points': points}");

    run_python_string("terrain.get_surface(Point3D(0,0,0))");

#ifndef NDEBUG
    run_python_string("import sabotage");
    // Hit the assert checks.
    run_python_string("method_get_height = terrain.get_height");
    run_python_string("method_get_surface = terrain.get_surface");
    run_python_string("method_get_normal = terrain.get_normal");
    run_python_string("sabotage.null(terrain)");
    fail_python_string("terrain.foo");
    fail_python_string("terrain.foo = 1");
    fail_python_string("method_get_height(0,0)");
    fail_python_string("method_get_surface(Point3D(0,0,0))");
    fail_python_string("method_get_normal(0,0)");
#endif // NDEBUG
   

    shutdown_python_api();
    return 0;
}
