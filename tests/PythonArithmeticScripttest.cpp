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
#include "rulesets/Python_Script_Utils.h"
#include "rulesets/PythonArithmeticScript.h"

#include <cassert>

static PyMethodDef no_methods[] = {
    {NULL,          NULL}                       /* Sentinel */
};

#define run_python_string(_s) { int pyret = PyRun_SimpleString(_s); \
                                assert(pyret == 0); }

int main()
{
    init_python_api();

    PyObject * testmod = Py_InitModule("testmod", no_methods);

    assert(testmod != 0);

    run_python_string("import testmod");
    run_python_string("class TestArithmeticScript(object):\n"
                      " def __init__(self):\n"
                      "  self.foo=1\n"
                      "  self.bar=1.1\n"
                      "  self.baz=None\n"
                      "  self.qux='1'\n"
                     );
    run_python_string("testmod.TestArithmeticScript=TestArithmeticScript");

    PyObject * clss = Get_PyClass(testmod, "testmod", "TestArithmeticScript");

    assert(clss != 0);

    PyObject * instance = PyEval_CallFunction(clss,"()");

    assert(instance != 0);

    PythonArithmeticScript pas(instance);

    float val;
    pas.attribute("foo", val);
    pas.attribute("bar", val);
    pas.attribute("baz", val);
    pas.attribute("qux", val);
    pas.attribute("nonexistent", val);

    pas.set("foo", 1.0f);
    pas.set("mim", 1.0f);

    shutdown_python_api();
    return 0;
}
