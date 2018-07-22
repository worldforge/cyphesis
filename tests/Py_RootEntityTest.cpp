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
#include "rulesets/Py_RootEntity.h"
#include "rulesets/python/CyPy_RootEntity.h"

#include <cassert>


int main()
{
    init_python_api("c67ce8e7-d195-4806-b8e4-d905e7c9d928");



    run_python_string("from atlas import Entity");
    run_python_string("from atlas import Location");

    auto ent = (Py::PythonClassObject<CyPy_RootEntity>)(CyPy_RootEntity::wrap(Atlas::Objects::Entity::RootEntity()));
    assert(ent.getCxxObject() != 0);
    assert(ent.getCxxObject()->m_value.isValid());

    run_python_string("Entity('1')");
    expect_python_error("Entity(1)", PyExc_TypeError);
    expect_python_error("Entity('1', location='loc')", PyExc_TypeError);
    run_python_string("l=Location()");
    run_python_string("Entity('1', location=l)");
    run_python_string("Entity('1', pos=())");
    run_python_string("Entity('1', pos=[])");
    expect_python_error("Entity('1', pos=(1,1.0,'1'))", PyExc_TypeError);
    expect_python_error("Entity('1', pos=[1,1.0,'1'])", PyExc_TypeError);
    run_python_string("Entity('1', tasks=[{'name': 'twist', 'param': 'value'}])");
    expect_python_error("Entity('1', pos=1)", PyExc_TypeError);
    expect_python_error("Entity('1', parent=1)", PyExc_TypeError);
    run_python_string("Entity('1', parent='0')");
    expect_python_error("Entity('1', type=1)", PyExc_TypeError);
    run_python_string("Entity('1', type='pig')");
    run_python_string("Entity('1', other=1)");
    expect_python_error("Entity('1', other=set([1,1]))", PyExc_TypeError);
    run_python_string("e=Entity()");
    run_python_string("e.get_name()");
    run_python_string("e.name");
    run_python_string("e.id");
    expect_python_error("e.foo", PyExc_AttributeError);
    run_python_string("e.name='Bob'");
    expect_python_error("e.name=1", PyExc_TypeError);
    run_python_string("e.foo='Bob'");
    run_python_string("e.bar=1");
    run_python_string("e.baz=[1,2.0,'three']");
    run_python_string("e.qux={'mim': 23}");
    expect_python_error("e.ptr=set([1,2])", PyExc_TypeError);
    run_python_string("e.foo");
    expect_python_error("e.ptr", PyExc_AttributeError);


    shutdown_python_api();
    return 0;
}
