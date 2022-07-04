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

#include "pythonbase/Python_API.h"
#include "rules/simulation/Task.h"
#include "rules/simulation/Entity.h"

#include <cassert>
#include <rules/simulation/python/CyPy_Task.h>
#include <rules/simulation/python/CyPy_UsageInstance.h>
#include <rules/python/CyPy_LocatedEntity.h>
#include <rules/simulation/python/CyPy_Server.h>
#include "pythonbase/PythonMalloc.h"


int main()
{
    setupPythonMalloc();
    init_python_api({&CyPy_Server::init});
    {
        Ref<Entity> entity(new Entity(1));
        UsageInstance usageInstance;
        usageInstance.actor = entity;
        Py::Module module("server");
        module.setAttr("usageInstance", CyPy_UsageInstance::wrap(usageInstance));
        module.setAttr("entity", CyPy_LocatedEntity::wrap(entity));

        run_python_string("import server")
        run_python_string("from server import Task");
        expect_python_error("Task()", PyExc_IndexError);
        expect_python_error("Task(1)", PyExc_TypeError);
        expect_python_error("Task('1')", PyExc_TypeError);
        run_python_string("c=server.entity");
        run_python_string("t=Task(server.usageInstance)");
        run_python_string("Task(t)");
        run_python_string("Task(t, name='t', tick_interval=11.0, duration=12.0, progress=0.5)");
        run_python_string("assert t.name == 't'");
        run_python_string("assert t.tick_interval == 11.0");
        run_python_string("assert t.duration == 12.0");
        run_python_string("assert t.progress == 0.5");
        run_python_string("assert t.actor == c");
        run_python_string("print(t.progress)");
        expect_python_error("print(t.foo)", PyExc_AttributeError);
        run_python_string("t.progress = 0");
        run_python_string("t.progress = 0.5");
        expect_python_error("t.progress = '1'", PyExc_TypeError);
        run_python_string("t.rate = 0");
        run_python_string("t.rate = 0.5");
        run_python_string("t.foo = 1");
        run_python_string("t.foo = 1.1");
        run_python_string("t.foo = 'foois1'");
        run_python_string("assert t.foo == 'foois1'");

        run_python_string("class TaskSubclass(Task): pass");
        run_python_string("t2=TaskSubclass(server.usageInstance)");
        // The subclass should have a dict offset
        run_python_string("t2.foo = 1");
        run_python_string("t2.foo = 1.1");
        run_python_string("t2.foo = 'foois1'");
        run_python_string("assert t2.foo == 'foois1'");
        run_python_string("t_temp = Task(server.usageInstance)");
        run_python_string("assert t!=t_temp");
        run_python_string("t_temp.irrelevant()");
        run_python_string("t2.irrelevant()");


        // Tasks do not permit wrappers of core server objects
        // to be stored directly.
        expect_python_error("t.foo = server.Thing('2')", PyExc_TypeError);

        // Except when using the "temporaries" map.
        run_python_string("t.temporaries['foo'] = server.Thing('2')");

        run_python_string("assert not t.obsolete()");
        run_python_string("t.irrelevant()");
        run_python_string("assert t.obsolete()");

        entity->destroy();
    }
    shutdown_python_api();

    return 0;
}
