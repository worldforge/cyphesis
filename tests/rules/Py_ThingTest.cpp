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

#include "../TestWorld.h"
#include "../TestPropertyManager.h"

#include "pythonbase/Python_API.h"
#include "rules/simulation/Entity.h"

#include <cassert>
#include <common/Inheritance.h>
#include "pythonbase/Python_Script_Utils.h"
#include <rules/python/CyPy_LocatedEntity.h>
#include <rules/simulation/python/CyPy_UsageInstance.h>
#include <rules/simulation/python/CyPy_Server.h>
#include <rules/simulation/python/CyPy_Entity.h>
#include <Atlas/Objects/Operation.h>
#include <rules/python/CyPy_Atlas.h>
#include <rules/python/CyPy_Physics.h>
#include <rules/python/CyPy_Common.h>
#include <rules/python/CyPy_Rules.h>
#include "pythonbase/PythonMalloc.h"

#include "../stubs/common/stubMonitors.h"

Atlas::Objects::Factories factories;

int main()
{

    setupPythonMalloc();
    {
        Inheritance inheritance(factories);
        TestPropertyManager testPropertyManager;
        init_python_api({&CyPy_Server::init,
                         &CyPy_Rules::init,
                         &CyPy_Atlas::init,
                         &CyPy_Physics::init,
                         &CyPy_Common::init});

        run_python_string("from server import *");
        run_python_string("from atlas import Operation");
        run_python_string("from atlas import Oplist");

        UsageInstance usageInstance;
        usageInstance.actor = new Entity(100);
        usageInstance.op = Atlas::Objects::Operation::Action();

        Py::Module serverModule("server");
        serverModule.setAttr("usage_instance", CyPy_UsageInstance::wrap(usageInstance));

        //Check that reference handling is correct.
        {
            Ref<Entity> entity = new Entity(2);
            assert(entity->checkRef() == 1);
            {
                auto wrap1 = CyPy_LocatedEntity::wrap(entity);
                assert(CyPy_Entity::check(wrap1));
                assert(entity->checkRef() == 2);
                {
                    //Since the Python object is cached another call to "wrap" should _not_ increase the count,
                    // since no new Python object is creatd.
                    auto wrap2 = CyPy_LocatedEntity::wrap(entity);
                    assert(entity->checkRef() == 2);
                    assert(CyPy_Entity::check(wrap2));
                }
                assert(entity->checkRef() == 2);
            }
            assert(entity->checkRef() == 1);
        }

        Ref<Entity> e = new Entity(1);

        {
            auto prop = std::make_unique<SoftProperty>();
            prop->set("bar");
            e->setProperty("foo", std::move(prop));
        }
        Ref<Entity> wrld = new Entity(0);
        e->m_parent = wrld.get();
        e->m_parent->makeContainer();
        assert(e->m_parent->m_contains != nullptr);
        e->m_parent->m_contains->insert(e);
        TestWorld test_world(wrld);

        auto wrap_e = CyPy_LocatedEntity::wrap(e);
        assert(CyPy_LocatedEntity::check(wrap_e));
        auto wrap_e_again = CyPy_LocatedEntity::wrap(e);
        assert(CyPy_LocatedEntity::check(wrap_e_again));
        assert(wrap_e == wrap_e_again);

        Ref<Entity> c = new Entity(2);
        auto wrap_c = CyPy_LocatedEntity::wrap(c);
        assert(CyPy_LocatedEntity::check(wrap_c));

        Py::Module module("__main__");
        module.setAttr("testentity", wrap_e);

        run_python_string("import server")
        run_python_string("assert testentity is not None");
        run_python_string("assert testentity.props.foo == 'bar'");
        run_python_string("assert testentity.is_destroyed == False");
        run_python_string("assert testentity.parent.id == '0'"); //Parent should be world
        run_python_string("assert testentity.parent.parent is None"); //The world should have no parent

        expect_python_error("Thing()", PyExc_IndexError);
        expect_python_error("Thing('s')", PyExc_TypeError);
        run_python_string("le=Thing('1')");
        run_python_string("le_ent = le.as_entity()");
        run_python_string("assert(le_ent.id == '1')");
        run_python_string("le.send_world(Operation('get'))");
        run_python_string("le==Thing('2')");
        run_python_string("le.type==''");
        expect_python_error("print(le.foo_operation)", PyExc_AttributeError);
        run_python_string("print(le.contains)");
        run_python_string("le.type==''");
        expect_python_error("le.type='non_exist'", PyExc_ValueError);
        expect_python_error("le.type=1", PyExc_TypeError);
        run_python_string("le.type='game_entity'");
        expect_python_error("le.type='game_entity'", PyExc_RuntimeError);
        run_python_string("le.type");
        expect_python_error("le.map=1", PyExc_AttributeError);
        run_python_string("le.props.map_attr={'1': 2}");
        run_python_string("le.props.map_attr");
        run_python_string("le.props.list_attr=[1,2]");
        run_python_string("le.props.list_attr");
        run_python_string("le.props.string_attr='foo'");
        run_python_string("le.props.int_attr=1");
        run_python_string("le.props.float_attr=2.0");
        run_python_string("assert(le.get_prop_string('string_attr')=='foo')");
        run_python_string("assert(le.get_prop_string('string_attr', 'bar')=='foo')");
        run_python_string("assert(le.get_prop_string('none_attr', 'bar')=='bar')");
        run_python_string("assert(le.get_prop_float('float_attr')==2.0)");
        run_python_string("assert(le.get_prop_float('float_attr', 10.0)==2.0)");
        run_python_string("assert(le.get_prop_float('none_attr', 10.0)==10.0)");
        run_python_string("assert(le.get_prop_int('int_attr')==1)");
        run_python_string("assert(le.get_prop_int('int_attr', 10)==1)");
        run_python_string("assert(le.get_prop_int('none_attr', 10)==10)");
        expect_python_error("le.props.non_atlas=set([1,2])", PyExc_TypeError);
        run_python_string("le.props.non_atlas == None");


        run_python_string("Thing(Thing('1'))");
        run_python_string("t=Thing('1')");
        run_python_string("Thing(t)");
        //run_python_string("Thing(Character('1'))");
        run_python_string("t.as_entity()");
        run_python_string("t.send_world(Operation('get'))");
        expect_python_error("t.send_world('get')", PyExc_TypeError);
        run_python_string("t.type==''");
        expect_python_error("print(t.foo_operation)", PyExc_AttributeError);
        run_python_string("print(t.contains)");

        module.getDict().clear();

        e->destroy();
    }
    shutdown_python_api();
    return 0;
}

