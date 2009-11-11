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
#include "rulesets/Py_Map.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyMap * o)
{
    if (!PyMap_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return Py_True;
    }
#ifndef NDEBUG
    o->m_map = NULL;
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

    PyMap * map = newPyMap();
    assert(map != 0);

    assert(PyRun_SimpleString("from server import Map") == 0);
    assert(PyRun_SimpleString("from atlas import Location") == 0);
    assert(PyRun_SimpleString("from atlas import Entity") == 0);
    assert(PyRun_SimpleString("from atlas import Message") == 0);
    assert(PyRun_SimpleString("m=Map()") == 0);
    assert(PyRun_SimpleString("m.find_by_location()") == -1);
    assert(PyRun_SimpleString("l=Location()") == 0);
    assert(PyRun_SimpleString("m.find_by_location(l)") == -1);
    assert(PyRun_SimpleString("m.find_by_location(l, 5.0, 'foo')") == -1);
    assert(PyRun_SimpleString("m.find_by_location(5, 5.0, 'foo')") == -1);
    assert(PyRun_SimpleString("m.find_by_type()") == -1);
    assert(PyRun_SimpleString("m.find_by_type(1)") == -1);
    assert(PyRun_SimpleString("m.find_by_type('foo')") == 0);
    assert(PyRun_SimpleString("m.add()") == -1);
    assert(PyRun_SimpleString("m.add('2')") == -1);
    assert(PyRun_SimpleString("m.add('2', 1.2)") == -1);
    assert(PyRun_SimpleString("m.add(Message())") == -1);
    assert(PyRun_SimpleString("m.add(Message(), 1.2)") == -1);
    assert(PyRun_SimpleString("m.add(Message({'objtype': 'op', 'parents': ['get']}), 1.2)") == -1);
    assert(PyRun_SimpleString("m.add(Message({}), 1.2)") == -1);
    assert(PyRun_SimpleString("m.add(Message({'parents': 'get'}), 1.2)") == -1);
    assert(PyRun_SimpleString("m.add(Message({'id': '2'}), 1.2)") == 0);
    assert(PyRun_SimpleString("m.add(Message({'id': '2'}), 1.2)") == 0);
    assert(PyRun_SimpleString("m.add(Entity())") == -1);
    assert(PyRun_SimpleString("m.add(Entity('1', type='oak'))") == -1);
    assert(PyRun_SimpleString("m.add(Entity('1', type='thing'), 1.1)") == 0);
    assert(PyRun_SimpleString("m.find_by_type('thing')") == 0);
    assert(PyRun_SimpleString("m.get()") == -1);
    assert(PyRun_SimpleString("m.get(1)") == -1);
    assert(PyRun_SimpleString("m.get('1')") == 0);
    assert(PyRun_SimpleString("m.get('23')") == 0);
    assert(PyRun_SimpleString("m.get_add()") == -1);
    assert(PyRun_SimpleString("m.get_add(3)") == -1);
    assert(PyRun_SimpleString("m.get_add('3')") == 0);
    assert(PyRun_SimpleString("m.update()") == -1);
    assert(PyRun_SimpleString("m.delete()") == -1);
    assert(PyRun_SimpleString("m.delete(1)") == -1);
    assert(PyRun_SimpleString("m.delete('1')") == 0);
    assert(PyRun_SimpleString("m.add_hooks_append()") == -1);
    assert(PyRun_SimpleString("m.add_hooks_append(1)") == -1);
    assert(PyRun_SimpleString("m.add_hooks_append('add_map')") == 0);
    assert(PyRun_SimpleString("m.update_hooks_append()") == -1);
    assert(PyRun_SimpleString("m.update_hooks_append(1)") == -1);
    assert(PyRun_SimpleString("m.update_hooks_append('update_map')") == 0);
    assert(PyRun_SimpleString("m.delete_hooks_append()") == -1);
    assert(PyRun_SimpleString("m.delete_hooks_append(1)") == -1);
    assert(PyRun_SimpleString("m.delete_hooks_append('delete_map')") == 0);

#ifndef NDEBUG
    assert(PyRun_SimpleString("import sabotage") == 0);
    assert(PyRun_SimpleString("sabotage.null(m)") == 0);
    // Hit the assert checks.
    assert(PyRun_SimpleString("m.find_by_location(l, 5.0, 'foo')") == -1);
    assert(PyRun_SimpleString("m.find_by_type('foo')") == -1);
    assert(PyRun_SimpleString("m.add(Entity('1', type='thing'), 1.1)") == -1);
    assert(PyRun_SimpleString("m.delete('1')") == -1);
    assert(PyRun_SimpleString("m.get('1')") == -1);
    assert(PyRun_SimpleString("m.get_add('3')") == -1);
    assert(PyRun_SimpleString("m.add_hooks_append('add_map')") == -1);
    assert(PyRun_SimpleString("m.update_hooks_append('update_map')") == -1);
    assert(PyRun_SimpleString("m.delete_hooks_append('delete_map')") == -1);
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
