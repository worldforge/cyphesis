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

#include <cassert>

int main()
{
    init_python_api();

    run_python_string("from physics import Point3D");
    run_python_string("from atlas import Message");
    run_python_string("Point3D([Message(1), Message(0), Message(0)])");
    fail_python_string("Point3D([Message('1'), Message(0), Message(0)])");
    run_python_string("p=Point3D(1,0,0)");
    run_python_string("p1=Point3D(0,1,0)");
    run_python_string("p2=Point3D(0,1,0)");
    run_python_string("print Point3D()");
    fail_python_string("print Point3D([1])");
    run_python_string("print Point3D([1,0,0])");
    run_python_string("print Point3D([1.1,0.0,0.0])");
    fail_python_string("print Point3D(['1','1','1'])");
    fail_python_string("print Point3D(1.1)");
    run_python_string("print Point3D(1.1,0.0,0.0)");
    fail_python_string("print Point3D('1','1','1')");
    fail_python_string("print Point3D(1.1,0.0,0.0,1.1)");
    run_python_string("print repr(p)");
    run_python_string("print p.mag()");
    run_python_string("print p.unit_vector_to(p1)");
    fail_python_string("print p.unit_vector_to(1.0)");
    run_python_string("print p.distance(p1)");
    fail_python_string("print p.distance(1.0)");
    run_python_string("print p.is_valid()");
    run_python_string("print p");
    run_python_string("print p == p1");
    run_python_string("print p1 == p2");
    run_python_string("from physics import Vector3D");
    run_python_string("v=Vector3D(1,0,0)");
    run_python_string("print p + v");
    fail_python_string("print p + p1");
    run_python_string("print p - v");
    run_python_string("print p - p1");
    fail_python_string("print p - 1.0");
    run_python_string("print p == v");

    shutdown_python_api();
    return 0;
}
