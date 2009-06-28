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

#include <cassert>

int main()
{
    init_python_api();

    assert(PyRun_SimpleString("from atlas import Entity") == 0);
    assert(PyRun_SimpleString("from atlas import Location") == 0);
    assert(PyRun_SimpleString("Entity('1')") == 0);
    assert(PyRun_SimpleString("Entity(1)") == -1);
    assert(PyRun_SimpleString("Entity('1', location='loc')") == -1);
    assert(PyRun_SimpleString("l=Location()") == 0);
    assert(PyRun_SimpleString("Entity('1', location=l)") == 0);
    assert(PyRun_SimpleString("Entity('1', pos=())") == 0);
    assert(PyRun_SimpleString("Entity('1', pos=[])") == 0);
    assert(PyRun_SimpleString("Entity('1', pos=(1,1.0,'1'))") == -1);
    assert(PyRun_SimpleString("Entity('1', pos=[1,1.0,'1'])") == -1);
    assert(PyRun_SimpleString("Entity('1', tasks=[{'name': 'twist', 'param': 'value'}])") == 0);
    assert(PyRun_SimpleString("Entity('1', pos=1)") == -1);
    assert(PyRun_SimpleString("Entity('1', parent=1)") == -1);
    assert(PyRun_SimpleString("Entity('1', parent='0')") == 0);
    assert(PyRun_SimpleString("Entity('1', type=1)") == -1);
    assert(PyRun_SimpleString("Entity('1', type='pig')") == 0);
    assert(PyRun_SimpleString("Entity('1', other=1)") == 0);
    assert(PyRun_SimpleString("e=Entity()") == 0);
    assert(PyRun_SimpleString("e.get_name()") == 0);
    assert(PyRun_SimpleString("e.name") == 0);
    assert(PyRun_SimpleString("e.id") == 0);
    assert(PyRun_SimpleString("e.foo") == -1);
    assert(PyRun_SimpleString("e.name='Bob'") == 0);
    assert(PyRun_SimpleString("e.name=1") == -1);
    assert(PyRun_SimpleString("e.foo='Bob'") == 0);
    assert(PyRun_SimpleString("e.bar=1") == 0);
    assert(PyRun_SimpleString("e.baz=[1,2.0,'three']") == 0);
    assert(PyRun_SimpleString("e.qux={'mim': 23}") == 0);
    assert(PyRun_SimpleString("e.foo") == 0);

    shutdown_python_api();
    return 0;
}
