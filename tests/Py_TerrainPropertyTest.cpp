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
#include "rulesets/Py_Thing.h"
#include "rulesets/Py_Property.h"
#include "rulesets/Entity.h"
#include "rulesets/TerrainProperty.h"

#include <cassert>

static PyObject * add_properties(PyObject * self, PyEntity * o)
{
    if (!PyEntity_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return nullptr;
    }

    Entity * ent = o->m_entity.e;
    
    PropertyBase * p = ent->setProperty("terrain", new TerrainProperty);
    p->install(ent, "terrain");
    p->apply(ent);
    ent->propertyApplied("terrain", *p);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * null_wrapper(PyObject * self, PyProperty * o)
{
    if (PyTerrainProperty_Check(o)) {
#ifdef CYPHESIS_DEBUG
        o->m_p.base = nullptr;
#endif // NDEBUG
    } else {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return nullptr;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef testprop_methods[] = {
    {"add_properties", (PyCFunction)add_properties,                 METH_O},
    {nullptr,          nullptr}                       /* Sentinel */
};

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

static PyObject* init_testprop() {
    static struct PyModuleDef def = {
            PyModuleDef_HEAD_INIT,
            "testprop",
            nullptr,
            0,
            testprop_methods,
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
    PyImport_AppendInittab("testprop", &init_testprop);
    init_python_api("bac81904-0516-4dd0-b9d8-32e879339b96");

    run_python_string("from server import *");
    run_python_string("import testprop");
    run_python_string("t=Thing('1')");
    expect_python_error("t.terrain", PyExc_AttributeError);
    run_python_string("testprop.add_properties(t)");
    run_python_string("terrain = t.terrain");
    expect_python_error("terrain.foo = 1", PyExc_AttributeError);
    expect_python_error("terrain.get_height()", PyExc_TypeError);
    run_python_string("terrain.get_height(0,0)");
    expect_python_error("terrain.get_surface()", PyExc_TypeError);
    expect_python_error("terrain.get_surface('1')", PyExc_TypeError);
    run_python_string("from physics import *");
    expect_python_error("terrain.get_surface(Point3D(0,0,0))", PyExc_TypeError);
    expect_python_error("terrain.get_normal()", PyExc_TypeError);
    run_python_string("terrain.get_normal(0,0)");
    run_python_string("terrain.find_mods(Point3D(0,0,0))");

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

    //No surfaces until "surfaces" is defined.
    expect_python_error("terrain.get_surface(Point3D(0,0,0))", PyExc_TypeError);

    run_python_string("surface = {'name': 'rock', 'pattern': 'fill'}");
    run_python_string("surfaces = [surface]");
    run_python_string("t.terrain = {'points': points, 'surfaces': surfaces}");

    run_python_string("terrain.get_surface(Point3D(0,0,0))");

#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");
    // Hit the assert checks.
    run_python_string("method_get_height = terrain.get_height");
    run_python_string("method_get_surface = terrain.get_surface");
    run_python_string("method_get_normal = terrain.get_normal");
    run_python_string("sabotage.null(terrain)");
    expect_python_error("method_get_height(0,0)", PyExc_AssertionError);
    expect_python_error("method_get_surface(Point3D(0,0,0))",
                        PyExc_AssertionError);
    expect_python_error("method_get_normal(0,0)", PyExc_AssertionError);
#endif // NDEBUG
   

    shutdown_python_api();
    return 0;
}
