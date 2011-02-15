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
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "python_testers.h"

#include "rulesets/Python_API.h"
#include "rulesets/Py_BBox.h"

#include <cassert>

int main()
{
    init_python_api();

    PyBBox * bbox = newPyBBox();
    assert(bbox != 0);

    run_python_string("from physics import BBox");
    run_python_string("b=BBox()");
    fail_python_string("b=BBox([1])");
    run_python_string("b=BBox([1,1,1])");
    run_python_string("b=BBox([1.0,1.0,1.0])");
    run_python_string("b=BBox([1,1,1])");
    fail_python_string("b=BBox(['1','1','1'])");
    run_python_string("from atlas import Message");
    run_python_string("b=BBox([Message(1),Message(1),Message(1)])");
    fail_python_string("b=BBox([Message('1'),Message('1'),Message('1')])");
    fail_python_string("b=BBox(1)");
    fail_python_string("b=BBox(1,1)");
    run_python_string("b=BBox(1,1,1)");
    run_python_string("b=BBox(1.0,1.0,1.0)");
    run_python_string("b=BBox(1.0,1.0,1.0,1.0,1.0,1.0)");
    fail_python_string("b=BBox('1','1','1')");
    run_python_string("print b.near_point");
    run_python_string("print b.far_point");
    run_python_string("print b.square_bounding_radius()");
    run_python_string("from physics import Point3D");
    fail_python_string("b.near_point=1");
    fail_python_string("b.near_point=Point3D()");
    fail_python_string("b.far_point=Point3D()");
    run_python_string("b.near_point=Point3D(0,0,0)");
    run_python_string("b.far_point=Point3D(1,1,1)");
    fail_python_string("b.other=Point3D(0,0,0)");
    run_python_string("print b.square_bounding_radius()");
    run_python_string("b2=BBox()");
    run_python_string("print b == b2");
    run_python_string("b2.near_point=Point3D(0,0,0)");
    run_python_string("b2.far_point=Point3D(1,1,1)");
    run_python_string("print b == b2");
    run_python_string("print b == '0'");
    

    shutdown_python_api();
    return 0;
}
