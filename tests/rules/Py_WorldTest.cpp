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

#include "../TestWorld.h"

#include "pythonbase/Python_API.h"

#include "rules/simulation/Entity.h"

#include <cassert>
#include <rules/simulation/python/CyPy_Server.h>
#include "pythonbase/PythonMalloc.h"
#include "rules/simulation/World.h"
#include "rules/simulation/python/CyPy_World.h"

int main()
{
    setupPythonMalloc();
    init_python_api({&CyPy_Server::init});
    {
        Ref<World> wrld(new World());
        TestWorld tw(wrld);

        Py::Module server("server");
        server.setAttr("test_world", CyPy_World::wrap(&tw));

        run_python_string("import server");
        run_python_string("w=server.test_world");
        run_python_string("w.get_time()");
        run_python_string("w.get_entity('0')");
        run_python_string("w.get_entity('1')");
        expect_python_error("w.get_entity(1)", PyExc_TypeError);
    }
    shutdown_python_api();
    return 0;
}

