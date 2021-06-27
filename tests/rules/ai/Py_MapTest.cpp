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

#include "../../python_testers.h"

#include "pythonbase/Python_API.h"
#include "../../TestPropertyManager.h"

#include <cassert>
#include <rules/ai/python/CyPy_MemMap.h>
#include <rules/SimpleTypeStore.h>
#include <rules/ai/TypeResolver.h>
#include <Atlas/Objects/RootOperation.h>
#include <rules/simulation/python/CyPy_Server.h>
#include <rules/python/CyPy_Atlas.h>
#include <rules/python/CyPy_Rules.h>
#include <rules/python/CyPy_Physics.h>
#include <rules/python/CyPy_Common.h>
#include <rules/ai/python/CyPy_Ai.h>
#include "pythonbase/PythonMalloc.h"

Atlas::Objects::Factories factories;

int main()
{

    {
        setupPythonMalloc();
        TestPropertyManager propertyManager;

        init_python_api({&CyPy_Server::init,
                         &CyPy_Rules::init,
                         &CyPy_Atlas::init,
                         &CyPy_Physics::init,
                         &CyPy_Common::init,
                         &CyPy_Ai::init});

        SimpleTypeStore typeStore{propertyManager};
        TypeResolver typeResolver{typeStore};

        MemMap memMap(typeResolver);
        auto map = CyPy_MemMap::wrap(&memMap);
        assert(!map.isNull());

        Py::Module module("server");
        module.setAttr("testmap", map);

        run_python_string("import server");
        run_python_string("from rules import Location");
        run_python_string("from atlas import Entity");
        run_python_string("from atlas import ElementMap");
        run_python_string("from atlas import ElementList");
        run_python_string("m=server.testmap");
        expect_python_error("m.find_by_location()", PyExc_IndexError);
        run_python_string("l=Location()");
        expect_python_error("m.find_by_location(l)", PyExc_IndexError);
        expect_python_error("m.find_by_location(l, 5.0, 'foo')",
                            PyExc_RuntimeError);
        expect_python_error("m.find_by_location(5, 5.0, 'foo')", PyExc_TypeError);
        expect_python_error("m.find_by_type()", PyExc_IndexError);
        expect_python_error("m.find_by_type(1)", PyExc_TypeError);
        run_python_string("m.find_by_type('foo')");
        expect_python_error("m.add()", PyExc_IndexError);
        expect_python_error("m.add('2')", PyExc_IndexError);
        expect_python_error("m.add('2', 1.2)", PyExc_TypeError);
        expect_python_error("m.add([])", PyExc_IndexError);
        expect_python_error("m.add(ElementList(), 1.2)", PyExc_TypeError);
        expect_python_error("m.add({'objtype': 'op', 'parent': 'get'}, 1.2)",
                            PyExc_TypeError);
        expect_python_error("m.add({}, 1.2)", PyExc_TypeError);
        expect_python_error("m.add({'parent': 'get'}, 1.2)",
                            PyExc_TypeError);
        run_python_string("m.add({'id': '2'}, 1.2)");
        run_python_string("m.add({'id': '2'}, 1.2)");
        expect_python_error("m.add(Entity())", PyExc_IndexError);
        expect_python_error("m.add(Entity('1', parent='oak'))", PyExc_IndexError);
        run_python_string("m.add(Entity('1', parent='thing'), 1.1)");
        run_python_string("m.find_by_type('thing')");
        expect_python_error("m.get()", PyExc_IndexError);
        expect_python_error("m.get(1)", PyExc_TypeError);
        run_python_string("m.get('1')");
        run_python_string("m.get('23')");

        //test adding and recalling entity-related memories
        run_python_string("m.add_entity_memory('1', 'disposition', 30)");
        run_python_string("disposition_val = m.recall_entity_memory('1', 'disposition')");
        run_python_string("assert(disposition_val == 30)");
        //test recalling a non-existing memory
        run_python_string("non_existing = m.recall_entity_memory('1', 'foo')");
        run_python_string("assert(non_existing == None)");

        expect_python_error("m.get_add()", PyExc_IndexError);
        expect_python_error("m.get_add(3)", PyExc_TypeError);
        run_python_string("m.get_add('3')");
        run_python_string("m.update(Entity('3', type='thing'), 1.1)");
        expect_python_error("m.update()", PyExc_IndexError);
        expect_python_error("m.delete()", PyExc_IndexError);
        expect_python_error("m.delete(1)", PyExc_TypeError);
        run_python_string("m.delete('1')");

        run_python_string("m.add_entity_memory('1', 'foo', 'bar')")
        run_python_string("m.add_entity_memory('1', 'foo1', 'baz')")
        run_python_string("assert(m.recall_entity_memory('1', 'foo') == 'bar')")
        run_python_string("assert(m.recall_entity_memory('1', 'foo1') == 'baz')")
        run_python_string("m.remove_entity_memory('1', 'foo')")
        run_python_string("assert(m.recall_entity_memory('1', 'foo') == None)")
        run_python_string("assert(m.recall_entity_memory('1', 'foo1') == 'baz')")
        //This should remove all memories for '1'
        run_python_string("m.remove_entity_memory('1')")
        run_python_string("assert(m.recall_entity_memory('1', 'foo1') == None)")
    }
    shutdown_python_api();
    return 0;
}
