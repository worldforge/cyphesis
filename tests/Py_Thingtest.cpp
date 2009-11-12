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

#include "TestWorld.h"
#include "TestPropertyManager.h"

#include "rulesets/Python_API.h"
#include "rulesets/Py_Thing.h"
#include "rulesets/Entity.h"
#include "rulesets/Character.h"

#include <cassert>

void check_union()
{
    PyEntity ent;

    Entity * e;
    LocatedEntity * le;
    Character * c;

    e = new Entity("1", 1);
    ent.m_entity.e = e;

    le = ent.m_entity.e;
    assert(le == ent.m_entity.l);
    assert(le == ent.m_entity.e);

    e = new Entity("1", 1);
    ent.m_entity.l = e;

    le = ent.m_entity.e;
    assert(le == ent.m_entity.l);
    assert(le == ent.m_entity.e);

    c = new Character("1", 1);
    ent.m_entity.c = c;

    le = ent.m_entity.e;
    assert(le == ent.m_entity.l);
    assert(le == ent.m_entity.e);
    assert(c == ent.m_entity.e);
    assert(c == ent.m_entity.l);

    ent.m_entity.e = new Entity("1", 1);
}

static PyObject * null_wrapper(PyObject * self, PyEntity * o)
{
    if (PyLocatedEntity_Check(o) || PyEntity_Check(o) || PyCharacter_Check(o)) {
#ifndef NDEBUG
        o->m_entity.l = NULL;
#endif // NDEBUG
    } else {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return NULL;
    }
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

class TestLocatedEntity : public LocatedEntity {
  public:
    TestLocatedEntity(const std::string & id, long intId) :
                      LocatedEntity(id, intId) { }

    virtual void operation(const Operation &, OpVector &) { }
};

int main()
{
    check_union();

    new TestPropertyManager;

    init_python_api();

    setup_test_functions();

    PyEntity * ent = newPyLocatedEntity();
    assert(ent != 0);
    Py_DECREF(ent);
    ent = newPyEntity();
    assert(ent != 0);
    Py_DECREF(ent);
    ent = newPyCharacter();
    assert(ent != 0);
    Py_DECREF(ent);

    Entity * e = new Entity("1", 1);
    Entity * wrld = new Entity("0", 0);
    e->m_location.m_loc = wrld;
    e->m_location.m_loc->makeContainer();
    assert(e->m_location.m_loc->m_contains != 0);
    e->m_location.m_loc->m_contains->insert(e);
    TestWorld test_world(*wrld);

    PyObject * wrap_e = wrapEntity(e);
    assert(wrap_e != 0);
    PyObject * wrap_e_again = wrapEntity(e);
    assert(wrap_e_again != 0);
    assert(wrap_e == wrap_e_again);

    Character * c = new Character("2", 2);
    assert(c != 0);
    PyObject * wrap_c = wrapEntity(c);
    assert(wrap_c != 0);

    LocatedEntity * le = new TestLocatedEntity("3", 3);
    assert(le != 0);
    PyObject * wrap_le = wrapEntity(le);
    assert(wrap_le != 0);
    

    assert(PyRun_SimpleString("from server import *") == 0);
    assert(PyRun_SimpleString("from atlas import Operation") == 0);
    assert(PyRun_SimpleString("from atlas import Oplist") == 0);

    assert(PyRun_SimpleString("LocatedEntity()") == -1);
    assert(PyRun_SimpleString("LocatedEntity('s')") == -1);
    assert(PyRun_SimpleString("le=LocatedEntity('1')") == 0);
    assert(PyRun_SimpleString("le.as_entity()") == 0);
    assert(PyRun_SimpleString("le.send_world(Operation('get'))") == -1);
    assert(PyRun_SimpleString("le==LocatedEntity('2')") == 0);
    assert(PyRun_SimpleString("print le.get_task()") == -1);
    assert(PyRun_SimpleString("print le.type") == -1);
    assert(PyRun_SimpleString("print le.foo_operation") == -1);
    assert(PyRun_SimpleString("print le.location") == 0);
    assert(PyRun_SimpleString("print le.contains") == 0);
    assert(PyRun_SimpleString("le.type") == -1);
    assert(PyRun_SimpleString("le.type='non_exist'") == -1);
    assert(PyRun_SimpleString("le.type=1") == -1);
    assert(PyRun_SimpleString("le.type='game_entity'") == 0);
    assert(PyRun_SimpleString("le.type='game_entity'") == -1);
    assert(PyRun_SimpleString("le.type") == 0);
    assert(PyRun_SimpleString("le.map=1") == -1);
    assert(PyRun_SimpleString("le.map_attr={'1': 2}") == 0);
    assert(PyRun_SimpleString("le.map_attr") == 0);
    assert(PyRun_SimpleString("le.list_attr=[1,2]") == 0);
    assert(PyRun_SimpleString("le.list_attr") == 0);
    assert(PyRun_SimpleString("le.non_atlas=set([1,2])") == 0);
    assert(PyRun_SimpleString("le.non_atlas") == 0);
    // assert(PyRun_SimpleString("le.foo=1") == 0);
    // assert(PyRun_SimpleString("le.foo='1'") == 0);
    // assert(PyRun_SimpleString("le.foo=[1]") == 0);
    // assert(PyRun_SimpleString("le.foo=['1']") == 0);
    // assert(PyRun_SimpleString("le.foo={'foo': 1, 'bar': '1'}") == 0);

    assert(PyRun_SimpleString("Thing()") == -1);
    assert(PyRun_SimpleString("Thing('s')") == -1);
    assert(PyRun_SimpleString("Thing(1)") == -1);
    assert(PyRun_SimpleString("t=Thing('1')") == 0);
    assert(PyRun_SimpleString("Thing(t)") == 0);
    assert(PyRun_SimpleString("t.as_entity()") == 0);
    assert(PyRun_SimpleString("t.send_world(Operation('get'))") == 0);
    assert(PyRun_SimpleString("t.send_world('get')") == -1);
    assert(PyRun_SimpleString("print t.get_task()") == -1);
    assert(PyRun_SimpleString("print t.type") == -1);
    assert(PyRun_SimpleString("print t.foo_operation") == -1);
    assert(PyRun_SimpleString("print t.location") == 0);
    assert(PyRun_SimpleString("print t.contains") == 0);

    assert(PyRun_SimpleString("Character()") == -1);
    assert(PyRun_SimpleString("Character('s')") == -1);
    assert(PyRun_SimpleString("Character(1)") == -1);
    assert(PyRun_SimpleString("c=Character('1')") == 0);
    assert(PyRun_SimpleString("Character(c)") == 0);
    assert(PyRun_SimpleString("c.as_entity()") == 0);
    assert(PyRun_SimpleString("c.send_world(Operation('get'))") == 0);
    assert(PyRun_SimpleString("c.get_task()") == 0);
    assert(PyRun_SimpleString("c.start_task()") == -1);
    assert(PyRun_SimpleString("c.start_task(Task(c))") == -1);
    assert(PyRun_SimpleString("c.start_task(1,Operation('cut'),Oplist())") == -1);
    assert(PyRun_SimpleString("c.start_task(Task(c),1,Oplist())") == -1);
    assert(PyRun_SimpleString("c.start_task(Task(c),Operation('cut'),1)") == -1);
    assert(PyRun_SimpleString("c.start_task(Task(c),Operation('cut'),Oplist())") == 0);
    assert(PyRun_SimpleString("c.get_task()") == 0);
    assert(PyRun_SimpleString("c.clear_task()") == 0);
    assert(PyRun_SimpleString("c.mind2body(1)") == -1);
    assert(PyRun_SimpleString("c.mind2body(Operation('update'))") == 0);
    assert(PyRun_SimpleString("c.mind2body(Operation('get'))") == 0);
    assert(PyRun_SimpleString("print c.type") == -1);
    assert(PyRun_SimpleString("print c.foo_operation") == -1);
    assert(PyRun_SimpleString("print c.location") == 0);
    assert(PyRun_SimpleString("print c.contains") == 0);

#ifndef NDEBUG
    assert(PyRun_SimpleString("import sabotage") == 0);
    // Hit the assert checks.

    assert(PyRun_SimpleString("t4=Thing('4')") == 0);
    assert(PyRun_SimpleString("sabotage.null(t4)") == 0);
    assert(PyRun_SimpleString("Thing(t4)") == -1);
    
    assert(PyRun_SimpleString("c5=Character('5')") == 0);
    assert(PyRun_SimpleString("sabotage.null(c5)") == 0);
    assert(PyRun_SimpleString("Character(c5)") == -1);

    assert(PyRun_SimpleString("sabotage.null(le)") == 0);
    assert(PyRun_SimpleString("le.location") == -1);
    assert(PyRun_SimpleString("le.foo=1") == -1);
    assert(PyRun_SimpleString("le == t") == -1);

    assert(PyRun_SimpleString("as_entity_method=t.as_entity") == 0);
    assert(PyRun_SimpleString("send_world_method=t.send_world") == 0);
    assert(PyRun_SimpleString("sabotage.null(t)") == 0);
    assert(PyRun_SimpleString("as_entity_method()") == -1);
    assert(PyRun_SimpleString("send_world_method(Operation('get'))") == -1);

    assert(PyRun_SimpleString("get_task_method=c.get_task") == 0);
    assert(PyRun_SimpleString("start_task_method=c.start_task") == 0);
    assert(PyRun_SimpleString("clear_task_method=c.clear_task") == 0);
    assert(PyRun_SimpleString("mind2body_method=c.mind2body") == 0);
    assert(PyRun_SimpleString("sabotage.null(c)") == 0);
    assert(PyRun_SimpleString("get_task_method()") == -1);
    assert(PyRun_SimpleString("start_task_method(Task(Character('3')),Operation('cut'),Oplist())") == -1);
    assert(PyRun_SimpleString("clear_task_method()") == -1);
    assert(PyRun_SimpleString("mind2body_method(Operation('update'))") == -1);

#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
