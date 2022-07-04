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
#else
#define CYPHESIS_DEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "../python_testers.h"

#include "../TestWorld.h"

#include "rules/simulation/Entity.h"
#include "pythonbase/Python_API.h"

#include <cassert>
#include <rules/python/CyPy_Atlas.h>
#include <rules/simulation/python/CyPy_Server.h>
#include <rules/python/CyPy_Physics.h>
#include <rules/python/CyPy_Common.h>
#include <rules/python/CyPy_Rules.h>
#include <rules/ai/python/CyPy_Ai.h>
#include "pythonbase/PythonMalloc.h"

int main()
{
    setupPythonMalloc();
    {
        init_python_api({&CyPy_Server::init,
                         &CyPy_Rules::init,
                         &CyPy_Atlas::init,
                         &CyPy_Physics::init,
                         &CyPy_Ai::init,
                         &CyPy_Common::init});

        Ref<Entity> wrld(new Entity(0));
        TestWorld tw(wrld);

        run_python_string("import atlas");
        run_python_string("import server");
        run_python_string("import rules");
        run_python_string("import ai");
        run_python_string("from physics import Point3D");
        expect_python_error("rules.Location(set([1,1]))", PyExc_TypeError);
        expect_python_error("rules.Location(1,1,1)", PyExc_TypeError);
        run_python_string("rules.Location(server.Thing('1'))");
        //run_python_string("rules.Location(server.World())");
        run_python_string("rules.Location(ai.MemEntity('1'))");
        expect_python_error("rules.Location(server.Thing('1'), 1)",
                            PyExc_TypeError);
        run_python_string("rules.Location(server.Thing('1'), Point3D(0,0,0))");
        run_python_string("rules.Location(rules.EntityLocation(server.Thing('1'), Point3D(0,0,0)))");
        run_python_string("l=rules.Location()");
        run_python_string("l1=l.copy()");
        run_python_string("l.parent");
        run_python_string("l.pos");
        run_python_string("l.velocity");
        run_python_string("l.orientation");
        run_python_string("l.bbox");
        run_python_string("from physics import Vector3D");
        run_python_string("from physics import Quaternion");
        run_python_string("from physics import BBox");
        run_python_string("l.pos=Point3D()");
        run_python_string("l.pos=Point3D(0,0,0)");
        run_python_string("l.pos=Vector3D()");
        run_python_string("l.pos=Vector3D(0,0,0)");
        expect_python_error("l.pos=()", PyExc_ValueError);
        run_python_string("l.pos=(0,0,0)");
        run_python_string("l.pos=(0.0,0,0)");
        expect_python_error("l.pos=('0',0,0)", PyExc_TypeError);
        expect_python_error("l.pos=[]", PyExc_ValueError);
        run_python_string("l.pos=[0,0,0]");
        run_python_string("l.pos=[0.0,0,0]");
        expect_python_error("l.pos=['0',0,0]", PyExc_TypeError);
        run_python_string("l.velocity=Vector3D()");
        run_python_string("l.velocity=Vector3D(0,0,0)");
        run_python_string("l.orientation=Quaternion()");
        run_python_string("l.orientation=Quaternion(0,0,0,1)");
        run_python_string("l.bbox=BBox()");
        run_python_string("l.bbox=Vector3D(0,0,0)");
        expect_python_error("l.parent='1'", PyExc_TypeError);
        run_python_string("l.parent=server.Thing('1')");
        expect_python_error("l.other=Vector3D(0,0,0)", PyExc_AttributeError);
        run_python_string("print(repr(l))");
        run_python_string("l2=rules.Location(server.Thing('1'), Point3D(0,0,0))");
        run_python_string("l.parent");
        run_python_string("common_parent = server.Thing('1')");
        run_python_string("rules.Location(common_parent, Point3D(0,0,0)) - rules.Location(common_parent, Point3D(1,0,0))");
        expect_python_error("rules.Location(common_parent, Point3D(0,0,0)) - Point3D(1,0,0)", PyExc_TypeError);
    }
    shutdown_python_api();
    return 0;
}


