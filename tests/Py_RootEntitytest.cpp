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

#include "rulesets/Python_API.h"
#include "rulesets/Py_RootEntity.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyRootEntity * o)
{
    if (!PyRootEntity_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return NULL;
    }
#ifdef CYPHESIS_DEBUG
    o->entity = Atlas::Objects::Entity::RootEntity(0);
#endif // NDEBUG
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

    PyRootEntity * ent = newPyRootEntity();
    assert(ent != 0);

    run_python_string("from atlas import Entity");
    run_python_string("from atlas import Location");
    run_python_string("Entity('1')");
    expect_python_error("Entity(1)", PyExc_TypeError);
    expect_python_error("Entity('1', location='loc')", PyExc_TypeError);
    run_python_string("l=Location()");
    run_python_string("Entity('1', location=l)");
    run_python_string("Entity('1', pos=())");
    run_python_string("Entity('1', pos=[])");
    expect_python_error("Entity('1', pos=(1,1.0,'1'))", PyExc_TypeError);
    expect_python_error("Entity('1', pos=[1,1.0,'1'])", PyExc_TypeError);
    run_python_string("Entity('1', tasks=[{'name': 'twist', 'param': 'value'}])");
    expect_python_error("Entity('1', pos=1)", PyExc_TypeError);
    expect_python_error("Entity('1', parent=1)", PyExc_TypeError);
    run_python_string("Entity('1', parent='0')");
    expect_python_error("Entity('1', type=1)", PyExc_TypeError);
    run_python_string("Entity('1', type='pig')");
    run_python_string("Entity('1', other=1)");
    expect_python_error("Entity('1', other=set([1,1]))", PyExc_TypeError);
    run_python_string("e=Entity()");
    run_python_string("e.get_name()");
    run_python_string("e.name");
    run_python_string("e.id");
    expect_python_error("e.foo", PyExc_AttributeError);
    run_python_string("e.name='Bob'");
    expect_python_error("e.name=1", PyExc_TypeError);
    run_python_string("e.foo='Bob'");
    run_python_string("e.bar=1");
    run_python_string("e.baz=[1,2.0,'three']");
    run_python_string("e.qux={'mim': 23}");
    run_python_string("e.ptr=set([1,2])");
    run_python_string("e.foo");
    run_python_string("e.ptr");

#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");
    // Hit the assert checks.
    run_python_string("get_name_methd=e.get_name");
    run_python_string("sabotage.null(e)");
    expect_python_error("get_name_methd()", PyExc_AssertionError);
    expect_python_error("e.name", PyExc_AssertionError);
    expect_python_error("e.name='Bob'", PyExc_AssertionError);
#endif // NDEBUG


    shutdown_python_api();
    return 0;
}
