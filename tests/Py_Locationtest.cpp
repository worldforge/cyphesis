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
#else
#define CYPHESIS_DEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "python_testers.h"

#include "TestWorld.h"

#include "rulesets/Python_API.h"
#include "rulesets/Py_Location.h"
#include "rulesets/Py_Thing.h"
#include "rulesets/Py_Mind.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyLocation * o)
{
    if (PyLocation_Check(o)) {
#ifdef CYPHESIS_DEBUG
        o->location = NULL;
#endif // NDEBUG
    } else if (PyMind_Check(o)) {
#ifdef CYPHESIS_DEBUG
        ((PyMind*)o)->m_mind = NULL;
#endif // NDEBUG
    } else if (PyLocatedEntity_Check(o) || PyEntity_Check(o) || PyCharacter_Check(o)) {
#ifdef CYPHESIS_DEBUG
        ((PyEntity*)o)->m_entity.l = NULL;
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

    Entity wrld("0", 0);
    TestWorld tw(wrld);

    run_python_string("import atlas");
    run_python_string("import server");
    run_python_string("from physics import Point3D");
    fail_python_string("atlas.Location(set([1,1]))");
    fail_python_string("atlas.Location(1,1,1)");
    run_python_string("atlas.Location(server.LocatedEntity('1'))");
    run_python_string("atlas.Location(server.Thing('1'))");
    run_python_string("atlas.Location(server.Character('1'))");
    run_python_string("atlas.Location(server.World())");
    run_python_string("atlas.Location(server.Mind('1'))");
    fail_python_string("atlas.Location(server.Thing('1'), 1)");
    run_python_string("atlas.Location(server.Thing('1'), Point3D(0,0,0))");
    run_python_string("l=atlas.Location()");
    run_python_string("l1=l.copy()");
    run_python_string("l.parent");
    run_python_string("l.coordinates");
    run_python_string("l.velocity");
    run_python_string("l.orientation");
    run_python_string("l.bbox");
    run_python_string("from physics import Vector3D");
    run_python_string("from physics import Quaternion");
    run_python_string("from physics import BBox");
    run_python_string("l.coordinates=Point3D()");
    run_python_string("l.coordinates=Point3D(0,0,0)");
    run_python_string("l.coordinates=Vector3D()");
    run_python_string("l.coordinates=Vector3D(0,0,0)");
    fail_python_string("l.coordinates=()");
    run_python_string("l.coordinates=(0,0,0)");
    run_python_string("l.coordinates=(0.0,0,0)");
    fail_python_string("l.coordinates=('0',0,0)");
    fail_python_string("l.coordinates=[]");
    run_python_string("l.coordinates=[0,0,0]");
    run_python_string("l.coordinates=[0.0,0,0]");
    fail_python_string("l.coordinates=['0',0,0]");
    run_python_string("l.velocity=Vector3D()");
    run_python_string("l.velocity=Vector3D(0,0,0)");
    run_python_string("l.orientation=Quaternion()");
    run_python_string("l.orientation=Quaternion(0,0,0,1)");
    run_python_string("l.bbox=BBox()");
    run_python_string("l.bbox=Vector3D(0,0,0)");
    fail_python_string("l.parent='1'");
    run_python_string("l.parent=server.Thing('1')");
    fail_python_string("l.other=Vector3D(0,0,0)");
    run_python_string("repr(l)");
    run_python_string("l2=atlas.Location(server.Thing('1'), Point3D(0,0,0))");
    run_python_string("l.parent");

#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");
    // Hit the assert checks.
    run_python_string("t=server.Thing('1')");
    run_python_string("sabotage.null(t)");
    fail_python_string("l.parent=t");
    fail_python_string("atlas.Location(t)");

    run_python_string("m=server.Mind('1')");
    run_python_string("sabotage.null(m)");
    fail_python_string("atlas.Location(m)");

    run_python_string("copy_methd=l.copy");
    run_python_string("sabotage.null(l)");
    fail_python_string("copy_methd()");
    fail_python_string("l.parent");
    fail_python_string("l.velocity=Vector3D()");
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
