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


#include "../python_testers.h"

#include "rules/Script.h"
#include "rules/simulation/Entity.h"
#include "rules/simulation/BaseWorld.h"
#include "rules/simulation/python/CyPy_Server.h"
#include "pythonbase/Python_API.h"
#include "rules/python/PythonScriptFactory.h"
#include "rules/python/PythonWrapper.h"
#include "pycxx/CXX/Extensions.hxx"
#include "common/operations/Tick.h"

#include "../TestWorld.h"

#include <Atlas/Objects/Operation.h>
#include <cassert>
#include <rules/python/CyPy_Rules.h>
#include <rules/python/CyPy_Atlas.h>
#include <rules/python/CyPy_Physics.h>
#include <rules/python/CyPy_Common.h>
#include <common/Inheritance.h>
#include "pythonbase/PythonMalloc.h"

Atlas::Objects::Factories factories;
Inheritance inheritance(factories);

struct TestMod : public Py::ExtensionModule<TestMod>
{
    TestMod() : ExtensionModule("testmod")
    {
        initialize("testmod");
    }
};

int main()
{
    setupPythonMalloc();
    {
        PyImport_AppendInittab("testmod", []() {
            auto module = new TestMod();
            return module->module().ptr();
        });

        init_python_api({&CyPy_Server::init,
                         &CyPy_Rules::init,
                         &CyPy_Atlas::init,
                         &CyPy_Physics::init,
                         &CyPy_Common::init});

        run_python_string("import server");
        run_python_string("import testmod");
        run_python_string("from atlas import Operation");
        run_python_string("class TestEntity(server.Thing):\n"
                          " def __init__(self, cppthing):\n"
                          "  self.foo = 'bar'\n"
                          "  assert self.foo == 'bar'\n"
                          " def look_operation(self, op): pass\n"
                          " def delete_operation(self, op):\n"
                          "  raise AssertionError('deliberate')\n"
                          " def tick_operation(self, op):\n"
                          "  raise AssertionError('deliberate')\n"
                          " def talk_operation(self, op):\n"
                          "  return 'invalid result'\n"
                          " def set_operation(self, op):\n"
                          "  return Operation('sight')\n"
                          " def move_operation(self, op):\n"
                          "  return Operation('sight') + Operation('move')\n"
                          " def test_hook(self, ent): pass\n");
        run_python_string("testmod.TestEntity=TestEntity");

        // PyObject * package_name = PyUnicode_FromString("testmod");
        // PyObject * testmod = PyImport_Import(package_name);
        // Py_DECREF(package_name);
        // assert(testmod);

        PythonScriptFactory<LocatedEntity> psf("testmod", "TestEntity");
        int ret = psf.setup();
        assert(ret == 0);
        Ref<Entity> e = new Entity(1);
        new TestWorld(e);
        ret = psf.addScript(*e);
        assert(ret == 0);

        assert(e->m_scripts.size() == 1);
        assert(dynamic_cast<PythonWrapper*>(e->m_scripts.front().get())->wrapper().getAttr("foo").as_string() == "bar");

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

        auto& script = e->m_scripts.front();
        assert(script);

        script->hook("nohookfunction", e.get(), res);
        script->hook("test_hook", e.get(), res);
        e = nullptr;
    }
    shutdown_python_api();
    return 0;
}
