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

#include "python_testers.h"

#include "rulesets/Entity.h"
#include "rulesets/Py_Thing.h"
#include "rulesets/Python_API.h"
#include "rulesets/PythonScriptFactory.h"
#include "rulesets/Script.h"

#include "common/Tick.h"
#include "common/BaseWorld.h"

#include <Atlas/Objects/Operation.h>

#include <cassert>

class TestWorld : public BaseWorld {
  public:
    explicit TestWorld(LocatedEntity& e) : BaseWorld(e) {
        m_realTime = 100000;
    }

    virtual bool idle(const SystemTime &) { return false; }
    virtual LocatedEntity * addEntity(LocatedEntity * ent) {
        return 0;
    }
    virtual LocatedEntity * addNewEntity(const std::string &,
                                  const Atlas::Objects::Entity::RootEntity &) {
        return 0;
    }
    void delEntity(LocatedEntity * obj) {}
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         LocatedEntity *) { return 0; }
    int getSpawnList(Atlas::Message::ListType & data) { return 0; }
    LocatedEntity * spawnNewEntity(const std::string & name,
                                   const std::string & type,
                                   const Atlas::Objects::Entity::RootEntity & desc) {
        return addNewEntity(type, desc);
    }
    virtual Task * newTask(const std::string &, LocatedEntity &) { return 0; }
    virtual Task * activateTask(const std::string &, const std::string &,
                                LocatedEntity *, LocatedEntity &) { return 0; }
    virtual ArithmeticScript * newArithmetic(const std::string &,
                                             LocatedEntity *) {
        return 0;
    }
    virtual void message(const Operation & op, LocatedEntity & ent) {
    }
    virtual LocatedEntity * findByName(const std::string & name) { return 0; }
    virtual LocatedEntity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(LocatedEntity *) { }
};


static PyMethodDef no_methods[] = {
    {NULL,          NULL}                       /* Sentinel */
};

int main()
{
    init_python_api("9fb5e26d-5631-479c-bdfc-cdb3c14b5428");

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

    PythonScriptFactory<LocatedEntity> psf("testmod", "TestEntity");
    int ret = psf.setup();
    assert(ret == 0);
    Entity * e = new Entity("1", 1);
    new TestWorld(*e);
    ret = psf.addScript(e);
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

    script->hook("nohookfunction", e);
    script->hook("test_hook", e);

    delete e;

    shutdown_python_api();
    return 0;
}
