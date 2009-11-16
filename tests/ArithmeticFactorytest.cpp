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

#include "rulesets/ArithmeticFactory.h"
#include "rulesets/Python_API.h"
#include "rulesets/Entity.h"

#include <cassert>

static PyMethodDef no_methods[] = {
    {NULL,          NULL}                       /* Sentinel */
};

int main()
{
    init_python_api();

    PyObject * testmod = Py_InitModule("testmod", no_methods);

    assert(testmod != 0);

    assert(PyRun_SimpleString("import testmod") == 0);
    assert(PyRun_SimpleString("class TestArithmeticScript(object):\n"
                              " def __init__(self):\n"
                              "  self.foo=1\n"
                              "  self.bar=1.1\n"
                              "  self.baz=None\n"
                              "  self.qux='1'\n"
                              ) == 0);
    assert(PyRun_SimpleString("class FailArithmeticScript(object):\n"
                              " def __init__(self):\n"
                              "  raise AssertionError, 'deliberate'\n"
                              ) == 0);
    assert(PyRun_SimpleString("testmod.TestArithmeticScript=TestArithmeticScript") == 0);
    assert(PyRun_SimpleString("testmod.FailArithmeticScript=FailArithmeticScript") == 0);

    {
        PythonArithmeticFactory paf("badmod", "TestArithmeticScript");
        assert(paf.m_module == 0);
        assert(paf.m_class == 0);

        ArithmeticScript * as = paf.newScript(0);
        assert(as == 0);
    }

    {
        PythonArithmeticFactory paf("testmod", "BadArithmeticScriptClass");
        assert(paf.m_module != 0);
        assert(paf.m_class == 0);

        ArithmeticScript * as = paf.newScript(0);
        assert(as == 0);
    }

    {
        PythonArithmeticFactory paf("testmod", "FailArithmeticScript");
        assert(paf.m_module != 0);
        assert(paf.m_class != 0);

        ArithmeticScript * as = paf.newScript(0);
        assert(as != 0);
    }


    PythonArithmeticFactory paf("testmod", "TestArithmeticScript");
    assert(paf.m_module != 0);
    assert(paf.m_class != 0);

    ArithmeticScript * as = paf.newScript(0);
    assert(as != 0);
    
    Entity * e = new Entity("1", 1);

    as = paf.newScript(e);
    assert(as != 0);
    
    shutdown_python_api();
    return 0;
}
