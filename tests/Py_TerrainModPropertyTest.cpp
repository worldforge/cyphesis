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
#include "rulesets/TerrainModProperty.h"

#include "physics/Shape.h"

#include <wfmath/polygon.h>

#include <cassert>

static PyObject * add_properties(PyObject * self, PyEntity * o)
{
    if (!PyEntity_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return nullptr;
    }

    Entity * ent = o->m_entity.e;
    
    PropertyBase * p = ent->setProperty("terrainmod", new TerrainModProperty);
    p->install(ent, "terrainmod");
    p->apply(ent);
    ent->propertyApplied("terrainmod", *p);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * add_terrainmod_shape(PyObject * self, PyProperty * o)
{
    if (!PyTerrainModProperty_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return nullptr;
    }

    TerrainModProperty * p = o->m_p.terrainmod;

    WFMath::Polygon<2> raw_polygon;
    raw_polygon.addCorner(0, WFMath::Point<2>(1,1));
    raw_polygon.addCorner(0, WFMath::Point<2>(1,0));
    raw_polygon.addCorner(0, WFMath::Point<2>(0,0));
    MathShape<WFMath::Polygon, 2> polygon(raw_polygon);
    Atlas::Message::MapType shape_data;
    polygon.toAtlas(shape_data);

    p->setAttr("shape", shape_data);
    p->setAttr("nonshape", "testval");

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * null_wrapper(PyObject * self, PyProperty * o)
{
    if (PyTerrainModProperty_Check(o)) {
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
    {"add_terrainmod_shape", (PyCFunction)add_terrainmod_shape,     METH_O},
    {nullptr,          nullptr}                       /* Sentinel */
};

static PyMethodDef sabotage_methods[] = {
    {"null", (PyCFunction)null_wrapper,                 METH_O},
    {nullptr,          nullptr}                       /* Sentinel */
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
    init_python_api("db35f202-3ebb-4df6-bf9e-4e840f6d7eb3");

    setup_test_functions();

    run_python_string("from server import *");
    run_python_string("import physics");
    run_python_string("import testprop");
    run_python_string("t=Thing('1')");
    expect_python_error("t.terrainmod", PyExc_AttributeError);
    run_python_string("testprop.add_properties(t)");
    run_python_string("terrainmod = t.terrainmod");
    expect_python_error("terrainmod.foo = 1", PyExc_AttributeError);
    expect_python_error("terrainmod.foo", PyExc_AttributeError);
    expect_python_error("terrainmod.shape", PyExc_AttributeError);
    expect_python_error("terrainmod.nonshape", PyExc_AttributeError);
    run_python_string("testprop.add_terrainmod_shape(terrainmod)");
    run_python_string("assert type(terrainmod.shape) == physics.Area");
    run_python_string("assert terrainmod.nonshape == 'testval'");
    run_python_string("print 'test1'");
    run_python_string("terrainmod.shape = physics.Polygon([[ -0.7, -0.7],"
                                                          "[ -1.0,  0.0],"
                                                          "[ -0.7,  0.7]])");

#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");
    // Hit the assert checks.
    run_python_string("sabotage.null(terrainmod)");
    expect_python_error("terrainmod.foo", PyExc_AssertionError);
    expect_python_error("terrainmod.foo = 1", PyExc_AssertionError);
#endif // NDEBUG
   

    shutdown_python_api();
    return 0;
}
