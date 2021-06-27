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
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "../python_testers.h"

#include "pythonbase/Python_API.h"

#include <cassert>
#include <rules/python/CyPy_Physics.h>
#include <rules/python/CyPy_Atlas.h>
#include "pythonbase/PythonMalloc.h"

int main()
{
    setupPythonMalloc();
    {
        init_python_api({&CyPy_Physics::init, &CyPy_Atlas::init});

        run_python_string("from physics import Vector3D");
        run_python_string("from atlas import ElementList");
        run_python_string("from atlas import ElementMap");
        run_python_string("v=Vector3D(1,0,0)");
        run_python_string("v1=Vector3D(0,1,0)");
        run_python_string("v2=Vector3D(0,1,0)");
        run_python_string("Vector3D()");
        expect_python_error("Vector3D('1')", PyExc_ValueError);
        expect_python_error("Vector3D([1])", PyExc_ValueError);
        run_python_string("Vector3D([1,0,0])");
        run_python_string("Vector3D([1.1,0.0,0.0])");
        expect_python_error("Vector3D(['1','1','1'])", PyExc_TypeError);
        expect_python_error("Vector3D(1.1)", PyExc_TypeError);
        run_python_string("Vector3D(1.1,0.0,0.0)");
        expect_python_error("Vector3D(1.1,0.0,'0.0')", PyExc_TypeError);
        expect_python_error("Vector3D(1.1,0.0,0.0,1.1)", PyExc_TypeError);
        run_python_string("repr(v)");
        expect_python_error("v.dot(1.0)", PyExc_TypeError);
        run_python_string("v.dot(v1)");
        expect_python_error("v.cross(1.0)", PyExc_TypeError);
        run_python_string("v.cross(v1)");
        run_python_string("v.rotatex(1.0)");
        expect_python_error("v.rotatex(1)", PyExc_TypeError);
        run_python_string("v.rotatey(1.0)");
        expect_python_error("v.rotatey(1)", PyExc_TypeError);
        run_python_string("v.rotatez(1.0)");
        expect_python_error("v.rotatez(1)", PyExc_TypeError);
        run_python_string("from physics import Quaternion");
        run_python_string("q=Quaternion(1,0,0,0)");
        run_python_string("v.rotate(q)");
        expect_python_error("v.rotate(Vector3D(0,1,0))", PyExc_TypeError);
        expect_python_error("v.angle(1.0)", PyExc_TypeError);
        run_python_string("v.angle(v1)");
        run_python_string("v.sqr_mag()");
        run_python_string("v.mag()");
        run_python_string("v.is_valid()");
        run_python_string("v.unit_vector()");
        expect_python_error("Vector3D(0,0,0).unit_vector()",
                            PyExc_ZeroDivisionError);
        run_python_string("v.unit_vector_to(v1)");
        expect_python_error("v.unit_vector_to(v)", PyExc_ZeroDivisionError);
        expect_python_error("v.unit_vector_to(q)", PyExc_TypeError);
        run_python_string("print(v)");
        run_python_string("print(v.x)");
        run_python_string("print(v.y)");
        run_python_string("print(v.z)");
        run_python_string("print(v[0])");
        run_python_string("print(v[1])");
        run_python_string("print(v[2])");
        run_python_string("print(v[-1])");
        expect_python_error("print(v[3])", PyExc_IndexError);
        run_python_string("v.x=1");
        run_python_string("v.y=1");
        run_python_string("v.z=1");
        run_python_string("v[0]=1.0");
        run_python_string("v[1]=1.0");
        run_python_string("v[2]=1.0");
        expect_python_error("v[3]=1", PyExc_IndexError);
        run_python_string("v.z=1.9");
        expect_python_error("v.z='1'", PyExc_TypeError);
        expect_python_error("v.w=1", PyExc_AttributeError);
        run_python_string("v == v1");
        run_python_string("v1 == v2");
        run_python_string("v1 + v2");
        expect_python_error("v1 + 2", PyExc_TypeError);
        run_python_string("v1 - v2");
        expect_python_error("v1 - 2", PyExc_TypeError);
        run_python_string("v * 1");
        run_python_string("v * 1.2");
        expect_python_error("v * '1.2'", PyExc_TypeError);
        run_python_string("v / 1");
        run_python_string("v / 1.2");
        expect_python_error("v / '1.2'", PyExc_TypeError);
    }
    shutdown_python_api();
    return 0;
}
