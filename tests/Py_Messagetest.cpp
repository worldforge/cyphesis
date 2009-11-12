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
#include "rulesets/Py_Message.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyMessage * o)
{
    if (!PyMessage_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return NULL;
    }
#ifndef NDEBUG
    o->m_obj = NULL;
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

    assert(PyRun_SimpleString("from atlas import Message") == 0);
    assert(PyRun_SimpleString("from atlas import Operation") == 0);
    assert(PyRun_SimpleString("from atlas import Oplist") == 0);
    assert(PyRun_SimpleString("from atlas import Location") == 0);
    assert(PyRun_SimpleString("from physics import Vector3D") == 0);
    assert(PyRun_SimpleString("Message()") == 0);
    assert(PyRun_SimpleString("Message(1)") == 0);
    assert(PyRun_SimpleString("Message(1.1)") == 0);
    assert(PyRun_SimpleString("Message('1')") == 0);
    assert(PyRun_SimpleString("Message([1, 1])") == 0);
    assert(PyRun_SimpleString("Message((1, 1))") == 0);
    assert(PyRun_SimpleString("Message({'foo': 1})") == 0);
    assert(PyRun_SimpleString("Message(Message(1))") == 0);
    assert(PyRun_SimpleString("Message(Operation('get'))") == 0);
    assert(PyRun_SimpleString("Message(Oplist(Operation('get')))") == 0);
    assert(PyRun_SimpleString("Message(Location())") == 0);
    assert(PyRun_SimpleString("Message(Vector3D())") == -1);
    assert(PyRun_SimpleString("Message([Message(1)])") == 0);
    assert(PyRun_SimpleString("Message([Vector3D()])") == -1);
    assert(PyRun_SimpleString("Message({'foo': Message(1)})") == 0);
    assert(PyRun_SimpleString("Message({'foo': Vector3D()})") == -1);
    assert(PyRun_SimpleString("Message(1, 1)") == -1);

    assert(PyRun_SimpleString("m=Message(1)") == 0);
    assert(PyRun_SimpleString("print m.get_name()") == 0);
    assert(PyRun_SimpleString("print m.foo") == -1);
    assert(PyRun_SimpleString("m.foo = 1") == -1);
    assert(PyRun_SimpleString("m=Message({})") == 0);
    assert(PyRun_SimpleString("print m.foo") == -1);
    assert(PyRun_SimpleString("m.foo = Vector3D()") == -1);
    assert(PyRun_SimpleString("m.foo = 1") == 0);
    assert(PyRun_SimpleString("print m.foo") == 0);
    assert(PyRun_SimpleString("m.foo = 1.1") == 0);
    assert(PyRun_SimpleString("print m.foo") == 0);
    assert(PyRun_SimpleString("m.foo = '1'") == 0);
    assert(PyRun_SimpleString("print m.foo") == 0);
    assert(PyRun_SimpleString("m.foo = ['1']") == 0);
    assert(PyRun_SimpleString("print m.foo") == 0);
    assert(PyRun_SimpleString("m.foo = {'foo': 1}") == 0);
    assert(PyRun_SimpleString("print m.foo") == 0);

#ifndef NDEBUG
    assert(PyRun_SimpleString("import sabotage") == 0);
    assert(PyRun_SimpleString("get_name_method=m.get_name") == 0);
    assert(PyRun_SimpleString("sabotage.null(m)") == 0);
    // Hit the assert checks.
    assert(PyRun_SimpleString("print get_name_method()") == -1);
    assert(PyRun_SimpleString("print m.foo") == -1);
    assert(PyRun_SimpleString("m.foo = 1") == -1);
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
