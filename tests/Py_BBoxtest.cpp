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

    assert(PyRun_SimpleString("from physics import BBox") == 0);
    assert(PyRun_SimpleString("b=BBox()") == 0);
    assert(PyRun_SimpleString("b=BBox([1])") == -1);
    assert(PyRun_SimpleString("b=BBox([1,1,1])") == 0);
    assert(PyRun_SimpleString("b=BBox([1.0,1.0,1.0])") == 0);
    assert(PyRun_SimpleString("b=BBox(['1','1','1'])") == -1);
    assert(PyRun_SimpleString("b=BBox(1)") == -1);
    assert(PyRun_SimpleString("b=BBox(1,1)") == -1);
    assert(PyRun_SimpleString("b=BBox(1,1,1)") == 0);
    assert(PyRun_SimpleString("b=BBox(1.0,1.0,1.0)") == 0);
    assert(PyRun_SimpleString("b=BBox(1.0,1.0,1.0,1.0,1.0,1.0)") == 0);
    assert(PyRun_SimpleString("b=BBox('1','1','1')") == -1);
    assert(PyRun_SimpleString("print b.near_point") == 0);
    assert(PyRun_SimpleString("print b.far_point") == 0);
    assert(PyRun_SimpleString("print b.square_bounding_radius()") == 0);
    assert(PyRun_SimpleString("from physics import Vector3D") == 0);
    assert(PyRun_SimpleString("b.near_point=1") == -1);
    assert(PyRun_SimpleString("b.near_point=Vector3D()") == -1);
    assert(PyRun_SimpleString("b.far_point=Vector3D()") == -1);
    assert(PyRun_SimpleString("b.near_point=Vector3D(0,0,0)") == 0);
    assert(PyRun_SimpleString("b.far_point=Vector3D(1,1,1)") == 0);
    assert(PyRun_SimpleString("b.other=Vector3D(0,0,0)") == -1);
    assert(PyRun_SimpleString("print b.square_bounding_radius()") == 0);
    assert(PyRun_SimpleString("b2=BBox()") == 0);
    assert(PyRun_SimpleString("print b == b2") == 0);
    assert(PyRun_SimpleString("b2.near_point=Vector3D(0,0,0)") == 0);
    assert(PyRun_SimpleString("b2.far_point=Vector3D(1,1,1)") == 0);
    assert(PyRun_SimpleString("print b == b2") == 0);
    assert(PyRun_SimpleString("print b == '0'") == 0);
    

    shutdown_python_api();
    return 0;
}
