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
#include "rulesets/Py_Operation.h"
#include "rulesets/Py_Oplist.h"
#include "rulesets/Py_Message.h"
#include "rulesets/Py_RootEntity.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyOperation * o)
{
    if (PyOperation_Check(o)) {
#ifdef CYPHESIS_DEBUG
        o->operation = Atlas::Objects::Operation::RootOperation(nullptr);
#endif // NDEBUG
    } else if (PyOplist_Check(o)) {
#ifdef CYPHESIS_DEBUG
        ((PyOplist*)o)->ops = 0;
#endif // NDEBUG
    } else if (PyRootEntity_Check(o)) {
#ifdef CYPHESIS_DEBUG
        ((PyRootEntity*)o)->entity = Atlas::Objects::Entity::RootEntity(nullptr);
#endif // NDEBUG
    } else if (PyMessage_Check(o)) {
#ifdef CYPHESIS_DEBUG
        ((PyMessage*)o)->m_obj = 0;
#endif // NDEBUG
    } else {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * clear_parents(PyObject * self, PyOperation * o)
{
    if (PyOperation_Check(o)) {
        o->operation->setParents(std::list<std::string>());
    } else {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return Py_True;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef sabotage_methods[] = {
    {"null",          (PyCFunction)null_wrapper,                 METH_O},
    {"clear_parents", (PyCFunction)clear_parents,                METH_O},
    {NULL,          NULL}                       /* Sentinel */
};

static void setup_test_functions()
{
    PyObject * sabotage = Py_InitModule("sabotage", sabotage_methods);
    assert(sabotage != 0);
}

int main()
{
    init_python_api("3622159a-de3c-42e6-858c-f6bd7cf8e7b1");

    setup_test_functions();

    PyOperation * op = newPyOperation();
    assert(op != 0);
    op = newPyConstOperation();
    assert(op != 0);

    run_python_string("from atlas import *");
    run_python_string("o=Operation('get')");
    // This should fail, but the error throwing has been disabled to
    // allow cooler stuff to be done from the client.
    // FIXME Once the client is constrained to real operations, go back
    // to disallowing arbitrary operation names.
    // expect_python_error("o=Operation('not_valid')");
    run_python_string("o=Operation('not_valid')");
    run_python_string("o=Operation('get', to='1', from_='1')");
    expect_python_error("o=Operation('get', from_=Message({'nonid': '1'}))",
                        PyExc_TypeError);
    expect_python_error("o=Operation('get', from_=Message({'id': 1}))",
                        PyExc_TypeError);
    expect_python_error("o=Operation('get', to=Message({'nonid': '1'}))",
                        PyExc_TypeError);
    expect_python_error("o=Operation('get', to=Message({'id': 1}))",
                        PyExc_TypeError);
    run_python_string("e=Entity('1')");
    run_python_string("o=Operation('get', to=e, from_=e)");
    run_python_string("o=Operation('get', Entity(), to='1', from_='1')");
    run_python_string("o=Operation('get', Operation('set'), to='1', from_='1')");
    expect_python_error("o=Operation('get', Location(), to='1', from_='1')",
                        PyExc_TypeError);
    run_python_string("o=Operation('get', Entity(), Entity(), Entity(), to='1', from_='1')");
    expect_python_error("Operation('get', Message('1'))", PyExc_TypeError);
    run_python_string("Operation('get', Message({'objtype': 'obj', 'parents': ['thing']}))");
    expect_python_error("o=Operation()", PyExc_TypeError);
    expect_python_error("o=Operation(1)", PyExc_TypeError);
    run_python_string("o=Operation('get')");
    run_python_string("o.setSerialno(1)");
    expect_python_error("o.setSerialno('1')", PyExc_TypeError);
    run_python_string("o.setRefno(1)");
    expect_python_error("o.setRefno('1')", PyExc_TypeError);
    run_python_string("o.setTo('1')");
    expect_python_error("o.setTo(1)", PyExc_TypeError);
    run_python_string("o.setFrom('2')");
    expect_python_error("o.setFrom(2)", PyExc_TypeError);
    run_python_string("o.setSeconds(2)");
    run_python_string("o.setSeconds(2.0)");
    expect_python_error("o.setSeconds('2.0')", PyExc_TypeError);
    run_python_string("o.setFutureSeconds(2)");
    run_python_string("o.setFutureSeconds(2.0)");
    expect_python_error("o.setFutureSeconds('2.0')", PyExc_TypeError);
    expect_python_error("o.setArgs()", PyExc_TypeError);
    run_python_string("o.setArgs([])");
    expect_python_error("o.setArgs(1)", PyExc_TypeError);
    expect_python_error("o.setArgs([1])", PyExc_TypeError);
    run_python_string("o.setArgs([Operation('get')])");
    run_python_string("o.setArgs([Entity(parents=[\"oak\"])])");
    run_python_string("o.setArgs([Message({'parents': ['root']})])");
    expect_python_error("o.setArgs([Message('1')])", PyExc_TypeError);
    run_python_string("import types");
    run_python_string("assert type(o.getSerialno()) == types.IntType");
    run_python_string("assert type(o.getRefno()) == types.IntType");
    run_python_string("assert type(o.getTo()) == types.StringType");
    run_python_string("assert type(o.getFrom()) == types.StringType");
    run_python_string("assert type(o.getSeconds()) == types.FloatType");
    run_python_string("assert type(o.getFutureSeconds()) == types.FloatType");
    run_python_string("assert type(o.getArgs()) == types.ListType");
    run_python_string("assert type(o.get_name()) == types.StringType");
    run_python_string("assert len(o) == 1");
    run_python_string("o.setArgs([Operation('get'), Entity(parents=[\"oak\"]), Message({'parents': ['root'], 'objtype': 'obj'})])");
    run_python_string("assert type(o[0]) == Operation");
    run_python_string("assert type(o[1]) == Entity");
    run_python_string("assert type(o[2]) == Message");
    expect_python_error("o[3]", PyExc_IndexError);
    run_python_string("assert o + None == o");
    expect_python_error("o + 1", PyExc_TypeError);
    run_python_string("assert type(o + Oplist()) == Oplist");
    run_python_string("assert type(o + Operation('get')) == Oplist");
    run_python_string("assert type(o.from_) == types.StringType");
    run_python_string("assert type(o.to) == types.StringType");
    run_python_string("assert type(o.id) == types.StringType");
    expect_python_error("o.from_='1'", PyExc_TypeError);
    expect_python_error("o.from_=1", PyExc_TypeError);
    expect_python_error("o.from_=Message({'id': 1})", PyExc_TypeError);
    run_python_string("o.from_=Message({'id': '1'})");
    expect_python_error("o.to='1'", PyExc_TypeError);
    expect_python_error("o.to=1", PyExc_TypeError);
    expect_python_error("o.to=Message({'id': 1})", PyExc_TypeError);
    run_python_string("o.to=Message({'id': '1'})");
    expect_python_error("o.other=1", PyExc_AttributeError);
    
#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");

    // Hit the assert checks.
    run_python_string("arg1=Message({'objtype': 'obj', 'parents': ['thing']})");
    run_python_string("sabotage.null(arg1)");
    expect_python_error("Operation('get', arg1)", PyExc_AssertionError);

    run_python_string("arg1=Entity()");
    run_python_string("sabotage.null(arg1)");
    expect_python_error("Operation('get', arg1)", PyExc_AssertionError);

    run_python_string("arg1=Operation('get')");
    run_python_string("sabotage.null(arg1)");
    expect_python_error("Operation('get', arg1)", PyExc_AssertionError);

    expect_python_error("Operation('get', Entity(), arg1)",
                        PyExc_AssertionError);
    expect_python_error("Operation('get', Entity(), Entity(), arg1)",
                        PyExc_AssertionError);

    run_python_string("o2=Operation('get')");
    run_python_string("sabotage.null(o2)");
    expect_python_error("o + o2", PyExc_AssertionError);

    run_python_string("sabotage.clear_parents(o)");
    expect_python_error("print o.id", PyExc_AttributeError);

    run_python_string("ol = Oplist()");
    run_python_string("sabotage.null(ol)");
    expect_python_error("o + ol", PyExc_AssertionError);

    run_python_string("method_setSerialno=o.setSerialno");
    run_python_string("method_setRefno=o.setRefno");
    run_python_string("method_setTo=o.setTo");
    run_python_string("method_setFrom=o.setFrom");
    run_python_string("method_setSeconds=o.setSeconds");
    run_python_string("method_setFutureSeconds=o.setFutureSeconds");
    run_python_string("method_setArgs=o.setArgs");
    run_python_string("method_getSerialno=o.getSerialno");
    run_python_string("method_getRefno=o.getRefno");
    run_python_string("method_getTo=o.getTo");
    run_python_string("method_getFrom=o.getFrom");
    run_python_string("method_getSeconds=o.getSeconds");
    run_python_string("method_getFutureSeconds=o.getFutureSeconds");
    run_python_string("method_getArgs=o.getArgs");
    run_python_string("method_get_name=o.get_name");

    run_python_string("sabotage.null(o)");
    expect_python_error("print o.to", PyExc_AssertionError);
    expect_python_error("len(o)", PyExc_AssertionError);
    expect_python_error("o[0]", PyExc_AssertionError);
    expect_python_error("o + None", PyExc_AssertionError);
    expect_python_error("o.to='1'", PyExc_AssertionError);

    expect_python_error("method_setSerialno(1)", PyExc_AssertionError);
    expect_python_error("method_setRefno(1)", PyExc_AssertionError);
    expect_python_error("method_setTo('1')", PyExc_AssertionError);
    expect_python_error("method_setFrom('2')", PyExc_AssertionError);
    expect_python_error("method_setSeconds(2.0)", PyExc_AssertionError);
    expect_python_error("method_setFutureSeconds(2.0)", PyExc_AssertionError);
    expect_python_error("method_setArgs([])", PyExc_AssertionError);
    expect_python_error("method_getSerialno()", PyExc_AssertionError);
    expect_python_error("method_getRefno()", PyExc_AssertionError);
    expect_python_error("method_getTo()", PyExc_AssertionError);
    expect_python_error("method_getFrom()", PyExc_AssertionError);
    expect_python_error("method_getSeconds()", PyExc_AssertionError);
    expect_python_error("method_getFutureSeconds()", PyExc_AssertionError);
    expect_python_error("method_getArgs()", PyExc_AssertionError);
    expect_python_error("method_get_name()", PyExc_AssertionError);

#endif // NDEBUG


    shutdown_python_api();
    return 0;
}
