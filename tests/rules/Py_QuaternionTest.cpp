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
#include "pythonbase/PythonMalloc.h"

int main()
{
    setupPythonMalloc();
    {
        init_python_api({&CyPy_Physics::init});

        run_python_string("from physics import Quaternion");
        run_python_string("q=Quaternion()");
        expect_python_error("q=Quaternion([1])", PyExc_ValueError);
        run_python_string("q=Quaternion([0,0,0,1])");
        run_python_string("q=Quaternion([0.0,0.0,0.0,1.0])");
        expect_python_error("q=Quaternion(['0.0',0.0,0.0,1.0])",
                            PyExc_TypeError);
        expect_python_error("q=Quaternion(1,0)", PyExc_TypeError);
        run_python_string("from physics import Vector3D");
        expect_python_error("q=Quaternion(Vector3D(1,0,0),0)",
                            PyExc_TypeError);
        run_python_string("q=Quaternion(Vector3D(1,0,0),0.0)");
        run_python_string("q=Quaternion(Vector3D(1,0,0),Vector3D(0,1,0), Vector3D(0,1,0))");
        expect_python_error("q=Quaternion(1,0,0)", PyExc_TypeError);
        run_python_string("q=Quaternion(0,0,0,1)");
        run_python_string("q=Quaternion(0.0,0.0,0.0,1.0)");
        expect_python_error("q=Quaternion('0.0',0.0,0.0,1.0)",
                            PyExc_TypeError);
        run_python_string("q=Quaternion()");
        run_python_string("q2=Quaternion()");
        run_python_string("q3=Quaternion(1,0,0,0)");
        run_python_string("print(q == q2)");
        run_python_string("print(q == q3)");
        run_python_string("print(q * q2)");
        expect_python_error("print(q * 2)", PyExc_TypeError);
        run_python_string("print(q.is_valid())");
        run_python_string("print(q.as_list())");
        expect_python_error("print(q.rotation())", PyExc_IndexError);
        expect_python_error("print(q.rotation('foo',1.0))", PyExc_TypeError);
        run_python_string("from physics import Vector3D");
        run_python_string("v=Vector3D(0,0,0)");
        run_python_string("print(q == v)");
//    run_python_string("print(v == q)");
//    run_python_string("print(q.rotation(v,1.0))");
        run_python_string("print(repr(q))");
    }
    shutdown_python_api();
    return 0;
}
