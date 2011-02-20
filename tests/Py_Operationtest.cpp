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
#include "rulesets/Py_Operation.h"
#include "rulesets/Py_Oplist.h"
#include "rulesets/Py_Message.h"
#include "rulesets/Py_RootEntity.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyOperation * o)
{
    if (PyOperation_Check(o)) {
#ifdef CYPHESIS_DEBUG
        o->operation = Atlas::Objects::Operation::RootOperation(0);
#endif // NDEBUG
    } else if (PyOplist_Check(o)) {
#ifdef CYPHESIS_DEBUG
        ((PyOplist*)o)->ops = 0;
#endif // NDEBUG
    } else if (PyRootEntity_Check(o)) {
#ifdef CYPHESIS_DEBUG
        ((PyRootEntity*)o)->entity = Atlas::Objects::Entity::RootEntity(0);
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
    init_python_api();

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
    // fail_python_string("o=Operation('not_valid')");
    run_python_string("o=Operation('not_valid')");
    run_python_string("o=Operation('get', to='1', from_='1')");
    fail_python_string("o=Operation('get', from_=Message({'nonid': '1'}))");
    fail_python_string("o=Operation('get', from_=Message({'id': 1}))");
    fail_python_string("o=Operation('get', to=Message({'nonid': '1'}))");
    fail_python_string("o=Operation('get', to=Message({'id': 1}))");
    run_python_string("e=Entity('1')");
    run_python_string("o=Operation('get', to=e, from_=e)");
    run_python_string("o=Operation('get', Entity(), to='1', from_='1')");
    run_python_string("o=Operation('get', Operation('set'), to='1', from_='1')");
    fail_python_string("o=Operation('get', Location(), to='1', from_='1')");
    run_python_string("o=Operation('get', Entity(), Entity(), Entity(), to='1', from_='1')");
    fail_python_string("Operation('get', Message('1'))");
    run_python_string("Operation('get', Message({'objtype': 'obj', 'parents': ['thing']}))");
    fail_python_string("o=Operation()");
    fail_python_string("o=Operation(1)");
    run_python_string("o=Operation('get')");
    run_python_string("o.setSerialno(1)");
    fail_python_string("o.setSerialno('1')");
    run_python_string("o.setRefno(1)");
    fail_python_string("o.setRefno('1')");
    run_python_string("o.setTo('1')");
    fail_python_string("o.setTo(1)");
    run_python_string("o.setFrom('2')");
    fail_python_string("o.setFrom(2)");
    run_python_string("o.setSeconds(2)");
    run_python_string("o.setSeconds(2.0)");
    fail_python_string("o.setSeconds('2.0')");
    run_python_string("o.setFutureSeconds(2)");
    run_python_string("o.setFutureSeconds(2.0)");
    fail_python_string("o.setFutureSeconds('2.0')");
    fail_python_string("o.setArgs()");
    run_python_string("o.setArgs([])");
    fail_python_string("o.setArgs(1)");
    fail_python_string("o.setArgs([1])");
    run_python_string("o.setArgs([Operation('get')])");
    run_python_string("o.setArgs([Entity(parents=[\"oak\"])])");
    run_python_string("o.setArgs([Message({'parents': ['root']})])");
    fail_python_string("o.setArgs([Message('1')])");
    run_python_string("o.getSerialno()");
    run_python_string("o.getRefno()");
    run_python_string("o.getTo()");
    run_python_string("o.getFrom()");
    run_python_string("o.getSeconds()");
    run_python_string("o.getFutureSeconds()");
    run_python_string("o.getArgs()");
    run_python_string("o.get_name()");
    run_python_string("len(o)");
    run_python_string("o.setArgs([Operation('get'), Entity(parents=[\"oak\"]), Message({'parents': ['root'], 'objtype': 'obj'})])");
    run_python_string("o[0]");
    run_python_string("o[1]");
    run_python_string("o[2]");
    fail_python_string("o[3]");
    run_python_string("o + None");
    fail_python_string("o + 1");
    run_python_string("o + Oplist()");
    run_python_string("o + Operation('get')");
    run_python_string("print o.from_");
    run_python_string("print o.to");
    run_python_string("print o.id");
    fail_python_string("o.from_='1'");
    fail_python_string("o.from_=1");
    fail_python_string("o.from_=Message({'id': 1})");
    run_python_string("o.from_=Message({'id': '1'})");
    fail_python_string("o.to='1'");
    fail_python_string("o.to=1");
    fail_python_string("o.to=Message({'id': 1})");
    run_python_string("o.to=Message({'id': '1'})");
    fail_python_string("o.other=1");
    
#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");

    // Hit the assert checks.
    run_python_string("arg1=Message({'objtype': 'obj', 'parents': ['thing']})");
    run_python_string("sabotage.null(arg1)");
    fail_python_string("Operation('get', arg1)");

    run_python_string("arg1=Entity()");
    run_python_string("sabotage.null(arg1)");
    fail_python_string("Operation('get', arg1)");

    run_python_string("arg1=Operation('get')");
    run_python_string("sabotage.null(arg1)");
    fail_python_string("Operation('get', arg1)");

    fail_python_string("Operation('get', Entity(), arg1)");
    fail_python_string("Operation('get', Entity(), Entity(), arg1)");

    run_python_string("o2=Operation('get')");
    run_python_string("sabotage.null(o2)");
    fail_python_string("o + o2");

    run_python_string("sabotage.clear_parents(o)");
    fail_python_string("print o.id");

    run_python_string("ol = Oplist()");
    run_python_string("sabotage.null(ol)");
    fail_python_string("o + ol");

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
    fail_python_string("print o.to");
    fail_python_string("len(o)");
    fail_python_string("o[0]");
    fail_python_string("o + None");
    fail_python_string("o.to='1'");

    fail_python_string("method_setSerialno(1)");
    fail_python_string("method_setRefno(1)");
    fail_python_string("method_setTo('1')");
    fail_python_string("method_setFrom('2')");
    fail_python_string("method_setSeconds(2.0)");
    fail_python_string("method_setFutureSeconds(2.0)");
    fail_python_string("method_setArgs([])");
    fail_python_string("method_getSerialno()");
    fail_python_string("method_getRefno()");
    fail_python_string("method_getTo()");
    fail_python_string("method_getFrom()");
    fail_python_string("method_getSeconds()");
    fail_python_string("method_getFutureSeconds()");
    fail_python_string("method_getArgs()");
    fail_python_string("method_get_name()");

#endif // NDEBUG


    shutdown_python_api();
    return 0;
}
