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

#include <Python.h>

#include "rulesets/Python_API.h"
#include "rulesets/Py_RootEntity.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyRootEntity * o)
{
    if (!PyRootEntity_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return NULL;
    }
#ifndef NDEBUG
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

    assert(PyRun_SimpleString("from atlas import Entity") == 0);
    assert(PyRun_SimpleString("from atlas import Location") == 0);
    assert(PyRun_SimpleString("Entity('1')") == 0);
    assert(PyRun_SimpleString("Entity(1)") == -1);
    assert(PyRun_SimpleString("Entity('1', location='loc')") == -1);
    assert(PyRun_SimpleString("l=Location()") == 0);
    assert(PyRun_SimpleString("Entity('1', location=l)") == 0);
    assert(PyRun_SimpleString("Entity('1', pos=())") == 0);
    assert(PyRun_SimpleString("Entity('1', pos=[])") == 0);
    assert(PyRun_SimpleString("Entity('1', pos=(1,1.0,'1'))") == -1);
    assert(PyRun_SimpleString("Entity('1', pos=[1,1.0,'1'])") == -1);
    assert(PyRun_SimpleString("Entity('1', tasks=[{'name': 'twist', 'param': 'value'}])") == 0);
    assert(PyRun_SimpleString("Entity('1', pos=1)") == -1);
    assert(PyRun_SimpleString("Entity('1', parent=1)") == -1);
    assert(PyRun_SimpleString("Entity('1', parent='0')") == 0);
    assert(PyRun_SimpleString("Entity('1', type=1)") == -1);
    assert(PyRun_SimpleString("Entity('1', type='pig')") == 0);
    assert(PyRun_SimpleString("Entity('1', other=1)") == 0);
    assert(PyRun_SimpleString("Entity('1', other=set([1,1]))") == -1);
    assert(PyRun_SimpleString("e=Entity()") == 0);
    assert(PyRun_SimpleString("e.get_name()") == 0);
    assert(PyRun_SimpleString("e.name") == 0);
    assert(PyRun_SimpleString("e.id") == 0);
    assert(PyRun_SimpleString("e.foo") == -1);
    assert(PyRun_SimpleString("e.name='Bob'") == 0);
    assert(PyRun_SimpleString("e.name=1") == -1);
    assert(PyRun_SimpleString("e.foo='Bob'") == 0);
    assert(PyRun_SimpleString("e.bar=1") == 0);
    assert(PyRun_SimpleString("e.baz=[1,2.0,'three']") == 0);
    assert(PyRun_SimpleString("e.qux={'mim': 23}") == 0);
    assert(PyRun_SimpleString("e.ptr=set([1,2])") == 0);
    assert(PyRun_SimpleString("e.foo") == 0);
    assert(PyRun_SimpleString("e.ptr") == 0);

#ifndef NDEBUG
    assert(PyRun_SimpleString("import sabotage") == 0);
    // Hit the assert checks.
    assert(PyRun_SimpleString("get_name_methd=e.get_name") == 0);
    assert(PyRun_SimpleString("sabotage.null(e)") == 0);
    assert(PyRun_SimpleString("get_name_methd()") == -1);
    assert(PyRun_SimpleString("e.name") == -1);
    assert(PyRun_SimpleString("e.name='Bob'") == -1);
#endif // NDEBUG


    shutdown_python_api();
    return 0;
}
