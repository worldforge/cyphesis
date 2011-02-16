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
    fail_python_string("Entity(1)");
    fail_python_string("Entity('1', location='loc')");
    run_python_string("l=Location()");
    run_python_string("Entity('1', location=l)");
    run_python_string("Entity('1', pos=())");
    run_python_string("Entity('1', pos=[])");
    fail_python_string("Entity('1', pos=(1,1.0,'1'))");
    fail_python_string("Entity('1', pos=[1,1.0,'1'])");
    run_python_string("Entity('1', tasks=[{'name': 'twist', 'param': 'value'}])");
    fail_python_string("Entity('1', pos=1)");
    fail_python_string("Entity('1', parent=1)");
    run_python_string("Entity('1', parent='0')");
    fail_python_string("Entity('1', type=1)");
    run_python_string("Entity('1', type='pig')");
    run_python_string("Entity('1', other=1)");
    fail_python_string("Entity('1', other=set([1,1]))");
    run_python_string("e=Entity()");
    run_python_string("e.get_name()");
    run_python_string("e.name");
    run_python_string("e.id");
    fail_python_string("e.foo");
    run_python_string("e.name='Bob'");
    fail_python_string("e.name=1");
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
    fail_python_string("get_name_methd()");
    fail_python_string("e.name");
    fail_python_string("e.name='Bob'");
#endif // NDEBUG


    shutdown_python_api();
    return 0;
}
