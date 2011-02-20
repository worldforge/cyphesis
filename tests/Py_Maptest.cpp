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
#include "rulesets/Py_Map.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyMap * o)
{
    if (!PyMap_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return NULL;
    }
#ifdef CYPHESIS_DEBUG
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

    run_python_string("from server import Map");
    run_python_string("from atlas import Location");
    run_python_string("from atlas import Entity");
    run_python_string("from atlas import Message");
    run_python_string("m=Map()");
    fail_python_string("m.find_by_location()");
    run_python_string("l=Location()");
    fail_python_string("m.find_by_location(l)");
    fail_python_string("m.find_by_location(l, 5.0, 'foo')");
    fail_python_string("m.find_by_location(5, 5.0, 'foo')");
    fail_python_string("m.find_by_type()");
    fail_python_string("m.find_by_type(1)");
    run_python_string("m.find_by_type('foo')");
    fail_python_string("m.add()");
    fail_python_string("m.add('2')");
    fail_python_string("m.add('2', 1.2)");
    fail_python_string("m.add(Message())");
    fail_python_string("m.add(Message(), 1.2)");
    fail_python_string("m.add(Message({'objtype': 'op', 'parents': ['get']}), 1.2)");
    fail_python_string("m.add(Message({}), 1.2)");
    fail_python_string("m.add(Message({'parents': 'get'}), 1.2)");
    run_python_string("m.add(Message({'id': '2'}), 1.2)");
    run_python_string("m.add(Message({'id': '2'}), 1.2)");
    fail_python_string("m.add(Entity())");
    fail_python_string("m.add(Entity('1', type='oak'))");
    run_python_string("m.add(Entity('1', type='thing'), 1.1)");
    run_python_string("m.find_by_type('thing')");
    fail_python_string("m.get()");
    fail_python_string("m.get(1)");
    run_python_string("m.get('1')");
    run_python_string("m.get('23')");
    fail_python_string("m.get_add()");
    fail_python_string("m.get_add(3)");
    run_python_string("m.get_add('3')");
    fail_python_string("m.update()");
    fail_python_string("m.delete()");
    fail_python_string("m.delete(1)");
    run_python_string("m.delete('1')");
    fail_python_string("m.add_hooks_append()");
    fail_python_string("m.add_hooks_append(1)");
    run_python_string("m.add_hooks_append('add_map')");
    fail_python_string("m.update_hooks_append()");
    fail_python_string("m.update_hooks_append(1)");
    run_python_string("m.update_hooks_append('update_map')");
    fail_python_string("m.delete_hooks_append()");
    fail_python_string("m.delete_hooks_append(1)");
    run_python_string("m.delete_hooks_append('delete_map')");

#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");
    run_python_string("sabotage.null(m)");
    // Hit the assert checks.
    fail_python_string("m.find_by_location(l, 5.0, 'foo')");
    fail_python_string("m.find_by_type('foo')");
    fail_python_string("m.add(Entity('1', type='thing'), 1.1)");
    fail_python_string("m.delete('1')");
    fail_python_string("m.get('1')");
    fail_python_string("m.get_add('3')");
    fail_python_string("m.add_hooks_append('add_map')");
    fail_python_string("m.update_hooks_append('update_map')");
    fail_python_string("m.delete_hooks_append('delete_map')");
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
