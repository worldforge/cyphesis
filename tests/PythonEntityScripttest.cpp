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

#include "python_testers.h"

#include "rulesets/Python_API.h"
#include "rulesets/Py_Thing.h"
#include "rulesets/Entity.h"
#include "rulesets/Script.h"

#include "server/ScriptFactory.h"
#include "common/Tick.h"

#include <Atlas/Objects/Operation.h>

#include <cassert>

static PyMethodDef no_methods[] = {
    {NULL,          NULL}                       /* Sentinel */
};

int main()
{
    init_python_api();

    Py_InitModule("testmod", no_methods);

    run_python_string("import server");
    run_python_string("import testmod");
    run_python_string("from atlas import Operation");
    run_python_string("class TestEntity(server.Thing):\n"
                      " def look_operation(self, op): pass\n"
                      " def delete_operation(self, op):\n"
                      "  raise AssertionError, 'deliberate'\n"
                      " def tick_operation(self, op):\n"
                      "  raise AssertionError, 'deliberate'\n"
                      " def talk_operation(self, op):\n"
                      "  return 'invalid result'\n"
                      " def set_operation(self, op):\n"
                      "  return Operation('sight')\n"
                      " def move_operation(self, op):\n"
                      "  return Operation('sight') + Operation('move')\n"
                      " def test_hook(self, ent): pass\n");
    run_python_string("testmod.TestEntity=TestEntity");

    // PyObject * package_name = PyString_FromString("testmod");
    // PyObject * testmod = PyImport_Import(package_name);
    // Py_DECREF(package_name);
    // assert(testmod);

    PythonScriptFactory psf("testmod", "TestEntity");
    Entity * e = new Entity("1", 1);
    int ret = psf.addScript(e);
    assert(ret == 0);

    OpVector res;
    Atlas::Objects::Operation::Look op1;
    e->operation(op1, res);

    Atlas::Objects::Operation::Create op2;
    e->operation(op2, res);

    Atlas::Objects::Operation::Delete op3;
    e->operation(op3, res);

    Atlas::Objects::Operation::Talk op4;
    e->operation(op4, res);

    Atlas::Objects::Operation::Set op5;
    e->operation(op5, res);

    Atlas::Objects::Operation::Move op6;
    e->operation(op6, res);

    Atlas::Objects::Operation::Tick op7;
    e->operation(op7, res);

    Script * script = e->script();
    assert(script != 0);
    assert(script != &noScript);

    script->hook("nohookfunction", e);
    script->hook("test_hook", e);

    delete e;

    shutdown_python_api();
    return 0;
}
