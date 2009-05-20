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

    assert(PyRun_SimpleString("from physics import Vector3D") == 0);
    assert(PyRun_SimpleString("v=Vector3D(1,0,0)") == 0);
    assert(PyRun_SimpleString("v1=Vector3D(0,1,0)") == 0);
    assert(PyRun_SimpleString("v2=Vector3D(0,1,0)") == 0);
    assert(PyRun_SimpleString("print Vector3D()") == 0);
    assert(PyRun_SimpleString("print Vector3D([1])") == -1);
    assert(PyRun_SimpleString("print Vector3D([1,0,0])") == 0);
    assert(PyRun_SimpleString("print Vector3D([1.1,0.0,0.0])") == 0);
    assert(PyRun_SimpleString("print Vector3D(['1','1','1'])") == -1);
    assert(PyRun_SimpleString("print Vector3D(1.1)") == -1);
    assert(PyRun_SimpleString("print Vector3D(1.1,0.0,0.0)") == 0);
    assert(PyRun_SimpleString("print Vector3D(1.1,0.0,0.0,1.1)") == -1);
    assert(PyRun_SimpleString("print repr(v)") == 0);
    assert(PyRun_SimpleString("print v.dot(1.0)") == -1);
    assert(PyRun_SimpleString("print v.dot(v1)") == 0);
    assert(PyRun_SimpleString("print v.cross(1.0)") == -1);
    assert(PyRun_SimpleString("print v.cross(v1)") == 0);
    assert(PyRun_SimpleString("v.rotatex(1.0)") == 0);
    assert(PyRun_SimpleString("v.rotatey(1.0)") == 0);
    assert(PyRun_SimpleString("v.rotatez(1.0)") == 0);
    assert(PyRun_SimpleString("from physics import Quaternion") == 0);
    assert(PyRun_SimpleString("q=Quaternion(1,0,0,0)") == 0);
    assert(PyRun_SimpleString("v.rotate(q)") == 0);
    assert(PyRun_SimpleString("print v.angle(1.0)") == -1);
    assert(PyRun_SimpleString("print v.angle(v1)") == 0);
    assert(PyRun_SimpleString("print v.square_mag()") == 0);
    assert(PyRun_SimpleString("print v.mag()") == 0);
    assert(PyRun_SimpleString("print v.is_valid()") == 0);
    assert(PyRun_SimpleString("print v.unit_vector()") == 0);
    assert(PyRun_SimpleString("print v.unit_vector_to(v1)") == 0);
    assert(PyRun_SimpleString("print v") == 0);
    assert(PyRun_SimpleString("v.x=1") == 0);
    assert(PyRun_SimpleString("v.y=1") == 0);
    assert(PyRun_SimpleString("v.z=1") == 0);
    assert(PyRun_SimpleString("v.z=1.9") == 0);
    assert(PyRun_SimpleString("v.z='1'") == -1);
    assert(PyRun_SimpleString("v.w=1") == -1);
    assert(PyRun_SimpleString("print v == v1") == 0);
    assert(PyRun_SimpleString("print v1 == v2") == 0);
    assert(PyRun_SimpleString("print v1 + v2") == 0);
    assert(PyRun_SimpleString("print v1 - v2") == 0);
    assert(PyRun_SimpleString("print v * 1") == 0);
    assert(PyRun_SimpleString("print v * 1.2") == 0);
    assert(PyRun_SimpleString("print v * '1.2'") == -1);
    assert(PyRun_SimpleString("print v / 1") == 0);
    assert(PyRun_SimpleString("print v / 1.2") == 0);
    assert(PyRun_SimpleString("print v / '1.2'") == -1);

    shutdown_python_api();
    return 0;
}
