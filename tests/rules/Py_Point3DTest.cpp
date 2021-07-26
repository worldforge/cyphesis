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
        init_python_api({&CyPy_Atlas::init, &CyPy_Physics::init});

        run_python_string("from physics import Point3D");
        run_python_string("Point3D([1, 0, 0])");
        expect_python_error("Point3D(['1', 0, 0])",
                            PyExc_TypeError);
        run_python_string("p=Point3D(1,0,0)");
        run_python_string("p1=Point3D(0,1,0)");
        run_python_string("p2=Point3D(0,1,0)");
        run_python_string("print(Point3D())");
        expect_python_error("print(Point3D('1'))", PyExc_ValueError);
        expect_python_error("print(Point3D([1]))", PyExc_ValueError);
        run_python_string("print(Point3D([1,0,0]))");
        run_python_string("print(Point3D([1.1,0.0,0.0]))");
        expect_python_error("print(Point3D(['1','1','1']))", PyExc_TypeError);
        expect_python_error("print(Point3D(1.1))", PyExc_TypeError);
        run_python_string("print(Point3D(1.1,0.0,0.0))");
        expect_python_error("print(Point3D('1','1','1'))", PyExc_TypeError);
        expect_python_error("print(Point3D(1.1,0.0,0.0,1.1))", PyExc_TypeError);
        run_python_string("print(repr(p))");
        run_python_string("print(p.mag())");
        run_python_string("print(p.unit_vector_to(p1))");
        expect_python_error("print(p.unit_vector_to(1.0))", PyExc_TypeError);
        run_python_string("print(p.distance(p1))");
        expect_python_error("print(p.distance(1.0))", PyExc_TypeError);
        run_python_string("print(p.is_valid())");
        run_python_string("print(p)");
        run_python_string("print(p.x)");
        run_python_string("print(p.y)");
        run_python_string("print(p.z)");
        run_python_string("print(p[0])");
        run_python_string("print(p[1])");
        run_python_string("print(p[2])");
        run_python_string("print(p[-1])");
        run_python_string("for c in p: print(c)");
        expect_python_error("print(p[3])", PyExc_IndexError);
        run_python_string("p[0]=1.0");
        run_python_string("p[1]=1.0");
        run_python_string("p[2]=1.0");
        expect_python_error("p[3]=1", PyExc_IndexError);
        run_python_string("print(p == p1)");
        run_python_string("print(p1 == p2)");
        run_python_string("from physics import Vector3D");
        run_python_string("v=Vector3D(1,0,0)");
        run_python_string("print(p + v)");
        run_python_string("print(p + p1)");
        run_python_string("print(p - v)");
        run_python_string("print(p - p1)");
        expect_python_error("print(p - 1.0)", PyExc_TypeError);
        run_python_string("print(p == v)");
    }
    shutdown_python_api();
    return 0;
}
