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

#include <cassert>
#include <rules/python/CyPy_Atlas.h>
#include <Atlas/Objects/Factories.h>
#include <common/Inheritance.h>
#include "pythonbase/PythonMalloc.h"

Atlas::Objects::Factories factories;
Inheritance inheritance(factories);

int main()
{
    setupPythonMalloc();
    {
        init_python_api({&CyPy_Atlas::init});

        run_python_string("from atlas import Oplist");
        run_python_string("from atlas import Operation");
        run_python_string("m=Oplist()");
        run_python_string("m.append(None)");
        expect_python_error("m.append(1)", PyExc_TypeError);
        run_python_string("m.append(Operation('get'))");
        run_python_string("m.append(Oplist())");
        run_python_string("m.append(Oplist(Operation('get')))");
        run_python_string("len(m)");
        run_python_string("lenm = len(m)");
        run_python_string("m += None");
        run_python_string("len(m) == lenm");
        expect_python_error("m += 1", PyExc_TypeError);
        run_python_string("len(m) == lenm");
        run_python_string("m += Operation('get')");
        run_python_string("len(m) == lenm + 1");
        run_python_string("m += Oplist()");
        run_python_string("len(m) == lenm + 1");
        run_python_string("m += Oplist(Operation('get'))");
        run_python_string("len(m) == lenm + 2");

        run_python_string("n = m + None");
        expect_python_error("m + 1", PyExc_TypeError);
        run_python_string("n = m + Operation('get')");
        run_python_string("assert len(n) == len(m) + 1");
        run_python_string("n = m + Oplist()");
        run_python_string("assert len(n) == len(m)");
        run_python_string("n = m + Oplist(Operation('get'))");
        run_python_string("assert len(n) == len(m) + 1");

        expect_python_error("Oplist(1)", PyExc_TypeError);
        expect_python_error("Oplist(Operation('get'), 1)", PyExc_TypeError);
        expect_python_error("Oplist(Operation('get'), Operation('get'), 1)",
                            PyExc_TypeError);
        expect_python_error("Oplist(Operation('get'), Operation('get'), Operation('get'), 1)",
                            PyExc_TypeError);
    }
    shutdown_python_api();
    return 0;
}
