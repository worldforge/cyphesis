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

#include "python_testers.h"

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
    

    run_python_string("from server import *");
    run_python_string("from atlas import Operation");
    run_python_string("from atlas import Oplist");

    fail_python_string("LocatedEntity()");
    fail_python_string("LocatedEntity('s')");
    run_python_string("le=LocatedEntity('1')");
    run_python_string("le.as_entity()");
    fail_python_string("le.send_world(Operation('get'))");
    run_python_string("le==LocatedEntity('2')");
    fail_python_string("print le.get_task()");
    fail_python_string("print le.type");
    fail_python_string("print le.foo_operation");
    run_python_string("print le.location");
    run_python_string("print le.contains");
    fail_python_string("le.type");
    fail_python_string("le.type='non_exist'");
    fail_python_string("le.type=1");
    run_python_string("le.type='game_entity'");
    fail_python_string("le.type='game_entity'");
    run_python_string("le.type");
    fail_python_string("le.map=1");
    run_python_string("le.map_attr={'1': 2}");
    run_python_string("le.map_attr");
    run_python_string("le.list_attr=[1,2]");
    run_python_string("le.list_attr");
    run_python_string("le.non_atlas=set([1,2])");
    run_python_string("le.non_atlas");
    // run_python_string("le.foo=1");
    // run_python_string("le.foo='1'");
    // run_python_string("le.foo=[1]");
    // run_python_string("le.foo=['1']");
    // run_python_string("le.foo={'foo': 1, 'bar': '1'}");

    fail_python_string("Thing()");
    fail_python_string("Thing('s')");
    fail_python_string("Thing(1)");
    run_python_string("t=Thing('1')");
    run_python_string("Thing(t)");
    run_python_string("t.as_entity()");
    run_python_string("t.send_world(Operation('get'))");
    fail_python_string("t.send_world('get')");
    fail_python_string("print t.get_task()");
    fail_python_string("print t.type");
    fail_python_string("print t.foo_operation");
    run_python_string("print t.location");
    run_python_string("print t.contains");

    fail_python_string("Character()");
    fail_python_string("Character('s')");
    fail_python_string("Character(1)");
    run_python_string("c=Character('1')");
    run_python_string("Character(c)");
    run_python_string("c.as_entity()");
    run_python_string("c.send_world(Operation('get'))");
    run_python_string("c.get_task()");
    fail_python_string("c.start_task()");
    fail_python_string("c.start_task(Task(c))");
    fail_python_string("c.start_task(1,Operation('cut'),Oplist())");
    fail_python_string("c.start_task(Task(c),1,Oplist())");
    fail_python_string("c.start_task(Task(c),Operation('cut'),1)");
    run_python_string("c.start_task(Task(c),Operation('cut'),Oplist())");
    run_python_string("c.get_task()");
    run_python_string("c.clear_task()");
    fail_python_string("c.mind2body(1)");
    run_python_string("c.mind2body(Operation('update'))");
    run_python_string("c.mind2body(Operation('get'))");
    fail_python_string("print c.type");
    fail_python_string("print c.foo_operation");
    run_python_string("print c.location");
    run_python_string("print c.contains");

#ifndef NDEBUG
    run_python_string("import sabotage");
    // Hit the assert checks.

    run_python_string("t4=Thing('4')");
    run_python_string("sabotage.null(t4)");
    fail_python_string("Thing(t4)");
    
    run_python_string("c5=Character('5')");
    run_python_string("sabotage.null(c5)");
    fail_python_string("Character(c5)");

    run_python_string("sabotage.null(le)");
    fail_python_string("le.location");
    fail_python_string("le.foo=1");
    fail_python_string("le == t");

    run_python_string("as_entity_method=t.as_entity");
    run_python_string("send_world_method=t.send_world");
    run_python_string("sabotage.null(t)");
    fail_python_string("as_entity_method()");
    fail_python_string("send_world_method(Operation('get'))");

    run_python_string("get_task_method=c.get_task");
    run_python_string("start_task_method=c.start_task");
    run_python_string("clear_task_method=c.clear_task");
    run_python_string("mind2body_method=c.mind2body");
    run_python_string("sabotage.null(c)");
    fail_python_string("get_task_method()");
    fail_python_string("start_task_method(Task(Character('3')),Operation('cut'),Oplist())");
    fail_python_string("clear_task_method()");
    fail_python_string("mind2body_method(Operation('update'))");

#endif // NDEBUG

    shutdown_python_api();
    return 0;
}
