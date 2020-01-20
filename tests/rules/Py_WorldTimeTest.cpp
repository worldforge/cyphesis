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

#include "rules/python/Python_API.h"

#include <cassert>
#include <rules/simulation/python/CyPy_Server.h>
#include <rules/python/CyPy_Rules.h>
#include <rules/python/CyPy_Atlas.h>
#include <rules/python/CyPy_Physics.h>
#include <rules/python/CyPy_Common.h>

int main()
{
    init_python_api({&CyPy_Server::init,
                     &CyPy_Rules::init,
                     &CyPy_Atlas::init,
                     &CyPy_Physics::init,
                     &CyPy_Common::init});

    run_python_string("from rules import WorldTime");
    expect_python_error("WorldTime()", PyExc_IndexError);
    run_python_string("WorldTime(23)");
    run_python_string("WorldTime(23.1)");

    run_python_string("w=WorldTime(23)");
    run_python_string("w.season");
    expect_python_error("w.foo", PyExc_AttributeError);
    run_python_string("w.is_now('morning')");
    expect_python_error("w.is_now(1)", PyExc_TypeError);
    run_python_string("w.seconds()");


    shutdown_python_api();
    return 0;
}
