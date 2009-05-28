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

#include "rulesets/Python_API.h"

#include <cassert>

int main()
{
    init_python_api();

    assert(PyRun_SimpleString("from server import WorldTime") == 0);
    assert(PyRun_SimpleString("WorldTime()") == -1);
    assert(PyRun_SimpleString("WorldTime(23)") == 0);
    assert(PyRun_SimpleString("WorldTime(23.1)") == 0);

    assert(PyRun_SimpleString("w=WorldTime(23)") == 0);
    assert(PyRun_SimpleString("w.season") == 0);
    assert(PyRun_SimpleString("w.foo") == -1);
    assert(PyRun_SimpleString("w.is_now('morning')") == 0);
    assert(PyRun_SimpleString("w.is_now(1)") == -1);
    assert(PyRun_SimpleString("w.seconds()") == 0);


    shutdown_python_api();
    return 0;
}
