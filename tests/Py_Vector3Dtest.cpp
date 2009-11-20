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

#include "rulesets/Python_API.h"

#include <cassert>

int main()
{
    init_python_api();

    run_python_string("from physics import Vector3D");
    run_python_string("from atlas import Message");
    run_python_string("v=Vector3D(1,0,0)");
    run_python_string("v1=Vector3D(0,1,0)");
    run_python_string("v2=Vector3D(0,1,0)");
    run_python_string("Vector3D()");
    fail_python_string("Vector3D([1])");
    run_python_string("Vector3D([1,0,0])");
    run_python_string("Vector3D([1.1,0.0,0.0])");
    run_python_string("Vector3D([Message(1.0),0,0])");
    run_python_string("Vector3D([Message(1),0,0])");
    fail_python_string("Vector3D([Message('1'),0,0])");
    fail_python_string("Vector3D(['1','1','1'])");
    fail_python_string("Vector3D(1.1)");
    run_python_string("Vector3D(1.1,0.0,0.0)");
    fail_python_string("Vector3D(1.1,0.0,'0.0')");
    fail_python_string("Vector3D(1.1,0.0,0.0,1.1)");
    run_python_string("repr(v)");
    fail_python_string("v.dot(1.0)");
    run_python_string("v.dot(v1)");
    fail_python_string("v.cross(1.0)");
    run_python_string("v.cross(v1)");
    run_python_string("v.rotatex(1.0)");
    fail_python_string("v.rotatex(1)");
    run_python_string("v.rotatey(1.0)");
    fail_python_string("v.rotatey(1)");
    run_python_string("v.rotatez(1.0)");
    fail_python_string("v.rotatez(1)");
    run_python_string("from physics import Quaternion");
    run_python_string("q=Quaternion(1,0,0,0)");
    run_python_string("v.rotate(q)");
    fail_python_string("v.rotate(Vector3D(0,1,0))");
    fail_python_string("v.angle(1.0)");
    run_python_string("v.angle(v1)");
    run_python_string("v.square_mag()");
    run_python_string("v.mag()");
    run_python_string("v.is_valid()");
    run_python_string("v.unit_vector()");
    fail_python_string("Vector3D(0,0,0).unit_vector()");
    run_python_string("v.unit_vector_to(v1)");
    fail_python_string("v.unit_vector_to(v)");
    fail_python_string("v.unit_vector_to(q)");
    run_python_string("print v");
    run_python_string("v.x=1");
    run_python_string("v.y=1");
    run_python_string("v.z=1");
    run_python_string("v.z=1.9");
    fail_python_string("v.z='1'");
    fail_python_string("v.w=1");
    run_python_string("v == v1");
    run_python_string("v1 == v2");
    run_python_string("v1 + v2");
    fail_python_string("v1 + 2");
    run_python_string("v1 - v2");
    fail_python_string("v1 - 2");
    run_python_string("v * 1");
    run_python_string("v * 1.2");
    fail_python_string("v * '1.2'");
    run_python_string("v / 1");
    run_python_string("v / 1.2");
    fail_python_string("v / '1.2'");

    shutdown_python_api();
    return 0;
}
