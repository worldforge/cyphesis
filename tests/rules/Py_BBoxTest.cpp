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
#include <rules/python/CyPy_Common.h>
#include "pythonbase/PythonMalloc.h"

int main()
{
    setupPythonMalloc();
    {
        init_python_api({&CyPy_Atlas::init,
                         &CyPy_Physics::init,
                         &CyPy_Common::init});

        run_python_string("from physics import BBox");
        run_python_string("b=BBox()");
        expect_python_error("b=BBox([1])", PyExc_ValueError);
        run_python_string("b=BBox([1,1,1])");
        run_python_string("b=BBox([1.0,1.0,1.0])");
        run_python_string("b=BBox([1,1,1])");
        expect_python_error("b=BBox(['1','1','1'])", PyExc_TypeError);
        run_python_string("from atlas import ElementList");
        run_python_string("b=BBox(ElementList(1,1,1))");
        expect_python_error("b=BBox(ElementList('1', '1','1'))",
                            PyExc_TypeError);
        expect_python_error("b=BBox(1)", PyExc_TypeError);
        expect_python_error("b=BBox(1,1)", PyExc_TypeError);
        run_python_string("b=BBox(1,1,1)");
        run_python_string("b=BBox(1.0,1.0,1.0)");
        expect_python_error("b=BBox('1','1','1')", PyExc_TypeError);
        run_python_string("b=BBox(0.0,0.0,0.0,1.0,1.0,1.0)");
        run_python_string("from physics import Point3D");
        run_python_string("assert b.low_corner == Point3D(0.0,0.0,0.0)");
        run_python_string("assert b.high_corner == Point3D(1.0,1.0,1.0)");
        run_python_string("assert b.as_sequence() == [0.0,0.0,0.0,1.0,1.0,1.0]");
        run_python_string("assert b.square_bounding_radius() > 1");
        expect_python_error("b.low_corner=1", PyExc_TypeError);
        expect_python_error("b.low_corner=Point3D()", PyExc_ValueError);
        expect_python_error("b.high_corner=Point3D()", PyExc_ValueError);
        run_python_string("b.low_corner=Point3D(0,0,0)");
        run_python_string("b.high_corner=Point3D(2,2,2)");
        expect_python_error("b.other=Point3D(0,0,0)", PyExc_AttributeError);
        run_python_string("assert b.high_corner == Point3D(2,2,2)");
        run_python_string("assert b.square_bounding_radius() > 2");
        run_python_string("b2=BBox()");
        run_python_string("assert not b == b2");
        run_python_string("assert b != b2");
        run_python_string("b2.low_corner=Point3D(0,0,0)");
        run_python_string("b2.high_corner=Point3D(2,2,2)");
        run_python_string("assert b == b2");
        run_python_string("assert not b != b2");
        run_python_string("assert not b == '0'");

    }
    shutdown_python_api();
    return 0;
}
