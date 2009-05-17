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

    assert(PyRun_SimpleString("import Point3D") == 0);
    assert(PyRun_SimpleString("p=Point3D.Point3D(1,0,0)") == 0);
    assert(PyRun_SimpleString("p1=Point3D.Point3D(0,1,0)") == 0);
    assert(PyRun_SimpleString("p2=Point3D.Point3D(0,1,0)") == 0);
    assert(PyRun_SimpleString("print Point3D.Point3D()") == 0);
    assert(PyRun_SimpleString("print Point3D.Point3D([1])") == -1);
    assert(PyRun_SimpleString("print Point3D.Point3D([1,0,0])") == 0);
    assert(PyRun_SimpleString("print Point3D.Point3D([1.1,0.0,0.0])") == 0);
    assert(PyRun_SimpleString("print Point3D.Point3D(['1','1','1'])") == -1);
    assert(PyRun_SimpleString("print Point3D.Point3D(1.1)") == -1);
    assert(PyRun_SimpleString("print Point3D.Point3D(1.1,0.0,0.0)") == 0);
    assert(PyRun_SimpleString("print Point3D.Point3D('1','1','1')") == -1);
    assert(PyRun_SimpleString("print Point3D.Point3D(1.1,0.0,0.0,1.1)") == -1);
    assert(PyRun_SimpleString("print repr(p)") == 0);
    assert(PyRun_SimpleString("print p.mag()") == 0);
    assert(PyRun_SimpleString("print p.unit_vector_to(p1)") == 0);
    assert(PyRun_SimpleString("print p.unit_vector_to(1.0)") == -1);
    assert(PyRun_SimpleString("print p.distance(p1)") == 0);
    assert(PyRun_SimpleString("print p.distance(1.0)") == -1);
    assert(PyRun_SimpleString("print p.is_valid()") == 0);
    assert(PyRun_SimpleString("print p") == 0);
    assert(PyRun_SimpleString("print p == p1") == 0);
    assert(PyRun_SimpleString("print p1 == p2") == 0);
    assert(PyRun_SimpleString("import Vector3D") == 0);
    assert(PyRun_SimpleString("v=Vector3D.Vector3D(1,0,0)") == 0);
    assert(PyRun_SimpleString("print p + v") == 0);
    assert(PyRun_SimpleString("print p + p1") == -1);
    assert(PyRun_SimpleString("print p - v") == 0);
    assert(PyRun_SimpleString("print p - p1") == 0);
    assert(PyRun_SimpleString("print p - 1.0") == -1);
    assert(PyRun_SimpleString("print p == v") == 0);

    shutdown_python_api();
    return 0;
}
