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

    assert(PyRun_SimpleString("import Quaternion") == 0);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion()") == 0);
    assert(PyRun_SimpleString("q2=Quaternion.Quaternion()") == 0);
    assert(PyRun_SimpleString("q3=Quaternion.Quaternion(1,0,0,0)") == 0);
    assert(PyRun_SimpleString("print q == q2") == 0);
    assert(PyRun_SimpleString("print q == q3") == 0);
    assert(PyRun_SimpleString("print q * q2") == 0);
    assert(PyRun_SimpleString("print q * 2") == -1);
    assert(PyRun_SimpleString("print q.is_valid()") == 0);
    assert(PyRun_SimpleString("print q.as_list()") == 0);
    assert(PyRun_SimpleString("print q.rotation()") == -1);
    assert(PyRun_SimpleString("print q.rotation('foo',1.0)") == -1);
    assert(PyRun_SimpleString("from physics import Vector3D") == 0);
    assert(PyRun_SimpleString("v=Vector3D(0,0,0)") == 0);
    assert(PyRun_SimpleString("print q == v") == 0);
    assert(PyRun_SimpleString("print v == q") == 0);
    assert(PyRun_SimpleString("print q.rotation(v,1.0)") == 0);
    assert(PyRun_SimpleString("print repr(q)") == 0);

    shutdown_python_api();
    return 0;
}
