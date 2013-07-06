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
#include "rulesets/Py_Message.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyMessage * o)
{
    if (!PyMessage_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return NULL;
    }
#ifdef CYPHESIS_DEBUG
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
    init_python_api("16799987-a321-43a2-aa66-f7bc8ed8e9b2");

    setup_test_functions();

    run_python_string("from atlas import Message");
    run_python_string("from atlas import Operation");
    run_python_string("from atlas import Oplist");
    run_python_string("from atlas import Location");
    run_python_string("from physics import Vector3D");
    run_python_string("Message()");
    run_python_string("Message(1)");
    run_python_string("Message(1.1)");
    run_python_string("Message('1')");
    run_python_string("Message([1, 1])");
    run_python_string("Message((1, 1))");
    run_python_string("Message({'foo': 1})");
    run_python_string("Message(Message(1))");
    run_python_string("Message(Operation('get'))");
    run_python_string("Message(Oplist(Operation('get')))");
    run_python_string("Message(Location())");
    expect_python_error("Message(Vector3D())", PyExc_TypeError);
    run_python_string("Message([Message(1)])");
    expect_python_error("Message([Vector3D()])", PyExc_TypeError);
    run_python_string("Message({'foo': Message(1)})");
    expect_python_error("Message({'foo': Vector3D()})", PyExc_TypeError);
    expect_python_error("Message(1, 1)", PyExc_TypeError);

    run_python_string("m=Message(1)");
    run_python_string("print m.get_name()");
    expect_python_error("print m.foo", PyExc_AttributeError);
    expect_python_error("m.foo = 1", PyExc_AttributeError);
    run_python_string("m=Message({})");
    expect_python_error("print m.foo", PyExc_AttributeError);
    expect_python_error("m.foo = Vector3D()", PyExc_TypeError);
    run_python_string("m.foo = 1");
    run_python_string("print m.foo");
    run_python_string("m.foo = 1.1");
    run_python_string("print m.foo");
    run_python_string("m.foo = '1'");
    run_python_string("print m.foo");
    run_python_string("m.foo = ['1']");
    run_python_string("print m.foo");
    run_python_string("m.foo = {'foo': 1}");
    run_python_string("print m.foo");
    run_python_string("m=Message(1)");
    run_python_string("assert m == 1");
    run_python_string("assert not m == 1.0");
    run_python_string("assert not m == '1'");
    run_python_string("assert not m != 1");
    run_python_string("assert m != 1.0");
    run_python_string("assert m != '1'");
    run_python_string("assert m != 2");
    run_python_string("m=Message(1.0)");
    run_python_string("assert not m == 1");
    run_python_string("assert m == 1.0");
    run_python_string("assert not m == '1'");
    run_python_string("assert m != 1");
    run_python_string("assert not m != 1.0");
    run_python_string("assert m != '1'");
    run_python_string("assert m != 2.0");
    run_python_string("m=Message('1')");
    run_python_string("assert not m == 1");
    run_python_string("assert not m == 1.0");
    run_python_string("assert m == '1'");
    run_python_string("assert m != 1");
    run_python_string("assert m != 1.0");
    run_python_string("assert not m != '1'");
    run_python_string("assert m != '2'");
    run_python_string("m=Message([])");
    run_python_string("assert not m == 1");
    run_python_string("assert not m == 1.0");
    run_python_string("assert not m == '1'");
    run_python_string("assert m != 1");
    run_python_string("assert m != 1.0");
    run_python_string("assert m != '1'");
    run_python_string("m=Message({})");
    run_python_string("assert not m == 1");
    run_python_string("assert not m == 1.0");
    run_python_string("assert not m == '1'");
    run_python_string("assert m != 1");
    run_python_string("assert m != 1.0");
    run_python_string("assert m != '1'");

#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");
    run_python_string("get_name_method=m.get_name");
    run_python_string("sabotage.null(m)");
    // Hit the assert checks.
    expect_python_error("print get_name_method()", PyExc_AssertionError);
    expect_python_error("print m.foo", PyExc_AssertionError);
    expect_python_error("m.foo = 1", PyExc_AssertionError);
#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
