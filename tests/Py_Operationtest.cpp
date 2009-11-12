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
#include "rulesets/Py_Operation.h"
#include "rulesets/Py_Oplist.h"
#include "rulesets/Py_Message.h"
#include "rulesets/Py_RootEntity.h"

#include <cassert>

static PyObject * null_wrapper(PyObject * self, PyOperation * o)
{
    if (PyOperation_Check(o)) {
#ifndef NDEBUG
        o->operation = Atlas::Objects::Operation::RootOperation(0);
#endif // NDEBUG
    } else if (PyOplist_Check(o)) {
#ifndef NDEBUG
        ((PyOplist*)o)->ops = 0;
#endif // NDEBUG
    } else if (PyRootEntity_Check(o)) {
#ifndef NDEBUG
        ((PyRootEntity*)o)->entity = Atlas::Objects::Entity::RootEntity(0);
#endif // NDEBUG
    } else if (PyMessage_Check(o)) {
#ifndef NDEBUG
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

    assert(PyRun_SimpleString("from atlas import *") == 0);
    assert(PyRun_SimpleString("o=Operation('get')") == 0);
    // This should fail, but the error throwing has been disabled to
    // allow cooler stuff to be done from the client.
    // FIXME Once the client is constrained to real operations, go back
    // to disallowing arbitrary operation names.
    // assert(PyRun_SimpleString("o=Operation('not_valid')") == -1);
    assert(PyRun_SimpleString("o=Operation('not_valid')") == 0);
    assert(PyRun_SimpleString("o=Operation('get', to='1', from_='1')") == 0);
    assert(PyRun_SimpleString("o=Operation('get', from_=Message({'nonid': '1'}))") == -1);
    assert(PyRun_SimpleString("o=Operation('get', from_=Message({'id': 1}))") == -1);
    assert(PyRun_SimpleString("o=Operation('get', to=Message({'nonid': '1'}))") == -1);
    assert(PyRun_SimpleString("o=Operation('get', to=Message({'id': 1}))") == -1);
    assert(PyRun_SimpleString("e=Entity('1')") == 0);
    assert(PyRun_SimpleString("o=Operation('get', to=e, from_=e)") == 0);
    assert(PyRun_SimpleString("o=Operation('get', Entity(), to='1', from_='1')") == 0);
    assert(PyRun_SimpleString("o=Operation('get', Operation('set'), to='1', from_='1')") == 0);
    assert(PyRun_SimpleString("o=Operation('get', Location(), to='1', from_='1')") == -1);
    assert(PyRun_SimpleString("o=Operation('get', Entity(), Entity(), Entity(), to='1', from_='1')") == 0);
    assert(PyRun_SimpleString("Operation('get', Message('1'))") == -1);
    assert(PyRun_SimpleString("Operation('get', Message({'objtype': 'obj', 'parents': ['thing']}))") == 0);
    assert(PyRun_SimpleString("o=Operation()") == -1);
    assert(PyRun_SimpleString("o=Operation(1)") == -1);
    assert(PyRun_SimpleString("o=Operation('get')") == 0);
    assert(PyRun_SimpleString("o.setSerialno(1)") == 0);
    assert(PyRun_SimpleString("o.setSerialno('1')") == -1);
    assert(PyRun_SimpleString("o.setRefno(1)") == 0);
    assert(PyRun_SimpleString("o.setRefno('1')") == -1);
    assert(PyRun_SimpleString("o.setTo('1')") == 0);
    assert(PyRun_SimpleString("o.setTo(1)") == -1);
    assert(PyRun_SimpleString("o.setFrom('2')") == 0);
    assert(PyRun_SimpleString("o.setFrom(2)") == -1);
    assert(PyRun_SimpleString("o.setSeconds(2)") == 0);
    assert(PyRun_SimpleString("o.setSeconds(2.0)") == 0);
    assert(PyRun_SimpleString("o.setSeconds('2.0')") == -1);
    assert(PyRun_SimpleString("o.setFutureSeconds(2)") == 0);
    assert(PyRun_SimpleString("o.setFutureSeconds(2.0)") == 0);
    assert(PyRun_SimpleString("o.setFutureSeconds('2.0')") == -1);
    assert(PyRun_SimpleString("o.setArgs()") == -1);
    assert(PyRun_SimpleString("o.setArgs([])") == 0);
    assert(PyRun_SimpleString("o.setArgs(1)") == -1);
    assert(PyRun_SimpleString("o.setArgs([1])") == -1);
    assert(PyRun_SimpleString("o.setArgs([Operation('get')])") == 0);
    assert(PyRun_SimpleString("o.setArgs([Entity(parents=[\"oak\"])])") == 0);
    assert(PyRun_SimpleString("o.setArgs([Message({'parents': ['root']})])") == 0);
    assert(PyRun_SimpleString("o.setArgs([Message('1')])") == -1);
    assert(PyRun_SimpleString("o.getSerialno()") == 0);
    assert(PyRun_SimpleString("o.getRefno()") == 0);
    assert(PyRun_SimpleString("o.getTo()") == 0);
    assert(PyRun_SimpleString("o.getFrom()") == 0);
    assert(PyRun_SimpleString("o.getSeconds()") == 0);
    assert(PyRun_SimpleString("o.getFutureSeconds()") == 0);
    assert(PyRun_SimpleString("o.getArgs()") == 0);
    assert(PyRun_SimpleString("o.get_name()") == 0);
    assert(PyRun_SimpleString("len(o)") == 0);
    assert(PyRun_SimpleString("o.setArgs([Operation('get'), Entity(parents=[\"oak\"]), Message({'parents': ['root'], 'objtype': 'obj'})])") == 0);
    assert(PyRun_SimpleString("o[0]") == 0);
    assert(PyRun_SimpleString("o[1]") == 0);
    assert(PyRun_SimpleString("o[2]") == 0);
    assert(PyRun_SimpleString("o[3]") == -1);
    assert(PyRun_SimpleString("o + None") == 0);
    assert(PyRun_SimpleString("o + 1") == -1);
    assert(PyRun_SimpleString("o + Oplist()") == 0);
    assert(PyRun_SimpleString("o + Operation('get')") == 0);
    assert(PyRun_SimpleString("print o.from_") == 0);
    assert(PyRun_SimpleString("print o.to") == 0);
    assert(PyRun_SimpleString("print o.id") == 0);
    assert(PyRun_SimpleString("o.from_='1'") == -1);
    assert(PyRun_SimpleString("o.from_=1") == -1);
    assert(PyRun_SimpleString("o.from_=Message({'id': 1})") == -1);
    assert(PyRun_SimpleString("o.from_=Message({'id': '1'})") == 0);
    assert(PyRun_SimpleString("o.to='1'") == -1);
    assert(PyRun_SimpleString("o.to=1") == -1);
    assert(PyRun_SimpleString("o.to=Message({'id': 1})") == -1);
    assert(PyRun_SimpleString("o.to=Message({'id': '1'})") == 0);
    assert(PyRun_SimpleString("o.other=1") == -1);
    
#ifndef NDEBUG
    assert(PyRun_SimpleString("import sabotage") == 0);

    // Hit the assert checks.
    assert(PyRun_SimpleString("arg1=Message({'objtype': 'obj', 'parents': ['thing']})") == 0);
    assert(PyRun_SimpleString("sabotage.null(arg1)") == 0);
    assert(PyRun_SimpleString("Operation('get', arg1)") == -1);

    assert(PyRun_SimpleString("arg1=Entity()") == 0);
    assert(PyRun_SimpleString("sabotage.null(arg1)") == 0);
    assert(PyRun_SimpleString("Operation('get', arg1)") == -1);

    assert(PyRun_SimpleString("arg1=Operation('get')") == 0);
    assert(PyRun_SimpleString("sabotage.null(arg1)") == 0);
    assert(PyRun_SimpleString("Operation('get', arg1)") == -1);

    assert(PyRun_SimpleString("Operation('get', Entity(), arg1)") == -1);
    assert(PyRun_SimpleString("Operation('get', Entity(), Entity(), arg1)") == -1);

    assert(PyRun_SimpleString("o2=Operation('get')") == 0);
    assert(PyRun_SimpleString("sabotage.null(o2)") == 0);
    assert(PyRun_SimpleString("o + o2") == -1);

    assert(PyRun_SimpleString("sabotage.clear_parents(o)") == 0);
    assert(PyRun_SimpleString("print o.id") == -1);

    assert(PyRun_SimpleString("ol = Oplist()") == 0);
    assert(PyRun_SimpleString("sabotage.null(ol)") == 0);
    assert(PyRun_SimpleString("o + ol") == -1);

    assert(PyRun_SimpleString("method_setSerialno=o.setSerialno") == 0);
    assert(PyRun_SimpleString("method_setRefno=o.setRefno") == 0);
    assert(PyRun_SimpleString("method_setTo=o.setTo") == 0);
    assert(PyRun_SimpleString("method_setFrom=o.setFrom") == 0);
    assert(PyRun_SimpleString("method_setSeconds=o.setSeconds") == 0);
    assert(PyRun_SimpleString("method_setFutureSeconds=o.setFutureSeconds") == 0);
    assert(PyRun_SimpleString("method_setArgs=o.setArgs") == 0);
    assert(PyRun_SimpleString("method_getSerialno=o.getSerialno") == 0);
    assert(PyRun_SimpleString("method_getRefno=o.getRefno") == 0);
    assert(PyRun_SimpleString("method_getTo=o.getTo") == 0);
    assert(PyRun_SimpleString("method_getFrom=o.getFrom") == 0);
    assert(PyRun_SimpleString("method_getSeconds=o.getSeconds") == 0);
    assert(PyRun_SimpleString("method_getFutureSeconds=o.getFutureSeconds") == 0);
    assert(PyRun_SimpleString("method_getArgs=o.getArgs") == 0);
    assert(PyRun_SimpleString("method_get_name=o.get_name") == 0);

    // assert(PyRun_SimpleString("copy_methd=l.copy") == 0);
    assert(PyRun_SimpleString("sabotage.null(o)") == 0);
    assert(PyRun_SimpleString("print o.to") == -1);
    assert(PyRun_SimpleString("len(o)") == -1);
    assert(PyRun_SimpleString("o[0]") == -1);
    assert(PyRun_SimpleString("o + None") == -1);
    assert(PyRun_SimpleString("o.to='1'") == -1);

    assert(PyRun_SimpleString("method_setSerialno(1)") == -1);
    assert(PyRun_SimpleString("method_setRefno(1)") == -1);
    assert(PyRun_SimpleString("method_setTo('1')") == -1);
    assert(PyRun_SimpleString("method_setFrom('2')") == -1);
    assert(PyRun_SimpleString("method_setSeconds(2.0)") == -1);
    assert(PyRun_SimpleString("method_setFutureSeconds(2.0)") == -1);
    assert(PyRun_SimpleString("method_setArgs([])") == -1);
    assert(PyRun_SimpleString("method_getSerialno()") == -1);
    assert(PyRun_SimpleString("method_getRefno()") == -1);
    assert(PyRun_SimpleString("method_getTo()") == -1);
    assert(PyRun_SimpleString("method_getFrom()") == -1);
    assert(PyRun_SimpleString("method_getSeconds()") == -1);
    assert(PyRun_SimpleString("method_getFutureSeconds()") == -1);
    assert(PyRun_SimpleString("method_getArgs()") == -1);
    assert(PyRun_SimpleString("method_get_name()") == -1);

#endif // NDEBUG


    shutdown_python_api();
    return 0;
}
