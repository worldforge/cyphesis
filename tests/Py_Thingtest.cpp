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
    if (PyLocatedEntity_Check(o)) {
#ifdef CYPHESIS_DEBUG
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

    virtual void externalOperation(const Operation &, Link &) { }
    virtual void operation(const Operation &, OpVector &) { }
};

int main()
{
    check_union();

    new TestPropertyManager;

    init_python_api("993bbe09-5751-41fe-8a18-17995c713c2e");

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

    expect_python_error("LocatedEntity()", PyExc_TypeError);
    expect_python_error("LocatedEntity('s')", PyExc_TypeError);
    run_python_string("le=LocatedEntity('1')");
    run_python_string("le_ent = le.as_entity()");
    run_python_string("assert(le_ent.id == '1')");
    expect_python_error("le.send_world(Operation('get'))",
                        PyExc_AttributeError);
    run_python_string("le==LocatedEntity('2')");
    expect_python_error("print le.type", PyExc_AttributeError);
    expect_python_error("print le.foo_operation", PyExc_AttributeError);
    run_python_string("print le.location");
    run_python_string("print le.contains");
    expect_python_error("le.type", PyExc_AttributeError);
    expect_python_error("le.type='non_exist'", PyExc_ValueError);
    expect_python_error("le.type=1", PyExc_TypeError);
    run_python_string("le.type='game_entity'");
    expect_python_error("le.type='game_entity'", PyExc_RuntimeError);
    run_python_string("le.type");
    expect_python_error("le.map=1", PyExc_AttributeError);
    run_python_string("le.map_attr={'1': 2}");
    run_python_string("le.map_attr");
    run_python_string("le.list_attr=[1,2]");
    run_python_string("le.list_attr");
    expect_python_error("le.non_atlas=set([1,2])", PyExc_AttributeError);
    expect_python_error("le.non_atlas", PyExc_AttributeError);

    run_python_string("class LocatedEntitySub(LocatedEntity): pass");

    run_python_string("le2=LocatedEntitySub('1')");
    run_python_string("le2.non_atlas=set([1,2])");
    run_python_string("le2.non_atlas");

    // run_python_string("le.foo=1");
    // run_python_string("le.foo='1'");
    // run_python_string("le.foo=[1]");
    // run_python_string("le.foo=['1']");
    // run_python_string("le.foo={'foo': 1, 'bar': '1'}");

    expect_python_error("Thing()", PyExc_TypeError);
    expect_python_error("Thing('s')", PyExc_TypeError);
    expect_python_error("Thing(1)", PyExc_TypeError);
    expect_python_error("Thing(LocatedEntity('1'))", PyExc_TypeError);
    run_python_string("t=Thing('1')");
    run_python_string("Thing(t)");
    run_python_string("Thing(Character('1'))");
    run_python_string("t.as_entity()");
    run_python_string("t.send_world(Operation('get'))");
    expect_python_error("t.send_world('get')", PyExc_TypeError);
    expect_python_error("print t.type", PyExc_AttributeError);
    expect_python_error("print t.foo_operation", PyExc_AttributeError);
    run_python_string("print t.location");
    run_python_string("print t.contains");

    expect_python_error("Character()", PyExc_TypeError);
    expect_python_error("Character('s')", PyExc_TypeError);
    expect_python_error("Character(1)", PyExc_TypeError);
    expect_python_error("Character(Thing('1'))", PyExc_TypeError);
    run_python_string("c=Character('1')");
    run_python_string("Character(c)");
    run_python_string("c.as_entity()");
    run_python_string("c.send_world(Operation('get'))");
    expect_python_error("c.start_task()", PyExc_TypeError);
    expect_python_error("c.start_task(Task(c))", PyExc_TypeError);
    expect_python_error("c.start_task(1,Operation('cut'),Oplist())",
                        PyExc_TypeError);
    expect_python_error("c.start_task(Task(c),1,Oplist())", PyExc_TypeError);
    expect_python_error("c.start_task(Task(c),Operation('cut'),1)",
                        PyExc_TypeError);
    run_python_string("c.start_task(Task(c),Operation('cut'),Oplist())");
    expect_python_error("c.mind2body(1)", PyExc_TypeError);
    run_python_string("c.mind2body(Operation('update'))");
    run_python_string("c.mind2body(Operation('get'))");
    expect_python_error("print c.type", PyExc_AttributeError);
    expect_python_error("print c.foo_operation", PyExc_AttributeError);
    run_python_string("print c.location");
    run_python_string("print c.contains");

    expect_python_error("Mind()", PyExc_TypeError);
    expect_python_error("Mind('s')", PyExc_TypeError);
    expect_python_error("Mind(1)", PyExc_TypeError);
    run_python_string("m=Mind('1')");
    run_python_string("m=Mind('1')");
    run_python_string("m_ent = m.as_entity()");
    run_python_string("assert(m_ent.id == '1')");
    expect_python_error("m.send_world(Operation('get'))", PyExc_AttributeError);
    run_python_string("m==LocatedEntity('2')");
    expect_python_error("print m.foo_operation", PyExc_AttributeError);
    run_python_string("print m.location");
    run_python_string("print m.contains");
    expect_python_error("m.type", PyExc_AttributeError);
    expect_python_error("m.map=1", PyExc_AttributeError);
    run_python_string("m.string_attr='foo'");
    run_python_string("assert(m.string_attr == 'foo')");
    run_python_string("m.int_attr=23");
    run_python_string("assert(m.int_attr == 23)");
    run_python_string("m.float_attr=17.23");
    expect_python_error("m.map_attr={'1': 2}", PyExc_AttributeError);
    expect_python_error("m.map_attr", PyExc_AttributeError);
    expect_python_error("m.list_attr=[1,2]", PyExc_AttributeError);
    expect_python_error("m.list_attr", PyExc_AttributeError);
    expect_python_error("m.non_atlas=set([1,2])", PyExc_AttributeError);
    expect_python_error("m.non_atlas", PyExc_AttributeError);

#ifdef CYPHESIS_DEBUG
    run_python_string("import sabotage");
    // Hit the assert checks.

    run_python_string("t4=Thing('4')");
    run_python_string("sabotage.null(t4)");
    expect_python_error("Thing(t4)", PyExc_AssertionError);
    
    run_python_string("c5=Character('5')");
    run_python_string("sabotage.null(c5)");
    expect_python_error("Character(c5)", PyExc_AssertionError);

    run_python_string("sabotage.null(le)");
    expect_python_error("le.location", PyExc_AssertionError);
    expect_python_error("le.foo=1", PyExc_AssertionError);
    expect_python_error("le == t", PyExc_AssertionError);

    run_python_string("as_entity_method=t.as_entity");
    run_python_string("send_world_method=t.send_world");
    run_python_string("sabotage.null(t)");
    expect_python_error("as_entity_method()", PyExc_AssertionError);
    expect_python_error("send_world_method(Operation('get'))",
                        PyExc_AssertionError);

    run_python_string("start_task_method=c.start_task");
    run_python_string("mind2body_method=c.mind2body");
    run_python_string("sabotage.null(c)");
    expect_python_error("start_task_method(Task(Character('3')),Operation('cut'),Oplist())",
                        PyExc_AssertionError);
    expect_python_error("mind2body_method(Operation('update'))",
                        PyExc_AssertionError);

#endif // NDEBUG

    shutdown_python_api();
    return 0;
}

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}
