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
#include <rules/simulation/python/CyPy_Server.h>
#include <rules/python/CyPy_Rules.h>
#include <rules/python/CyPy_Physics.h>
#include <rules/python/CyPy_Atlas.h>
#include <rules/python/CyPy_Common.h>
#include "pythonbase/PythonMalloc.h"

#include "../python_testers.h"

#include "../TestWorld.h"

#include "pythonbase/Python_API.h"
#include "rules/simulation/Entity.h"
#include "rules/simulation/LineProperty.h"
#include "rules/simulation/TerrainProperty.h"
#include "rules/simulation/python/CyPy_Entity.h"

#include "pycxx/CXX/Extensions.hxx"


class TestProp : public Py::ExtensionModule<TestProp>
{
    public:
        Py::Object add_properties(const Py::Tuple& args)
        {
            auto ent = CyPy_Entity::value(args.front());

            auto p = ent->setProperty("terrain", std::make_unique<TerrainProperty>());
            p->install(*ent, "terrain");
            p->apply(*ent);
            ent->propertyApplied("terrain", *p);
            p = ent->setProperty("line", std::make_unique<LineProperty>());
            p->install(*ent, "line");
            p->apply(*ent);
            ent->propertyApplied("line", *p);

            return Py::None();
        }

        TestProp() : ExtensionModule("testprop")
        {

            add_varargs_method("add_properties", &TestProp::add_properties, "");


            initialize("testprop");
        }

};

int main()
{
    setupPythonMalloc();
    {
        PyImport_AppendInittab("testprop", []() {
            static TestProp testProp;
            return testProp.module().ptr();
        });
        init_python_api({&CyPy_Server::init,
                         &CyPy_Rules::init,
                         &CyPy_Atlas::init,
                         &CyPy_Physics::init,
                         &CyPy_Common::init});


        Ref<Entity> wrld(new Entity(0));
        TestWorld tw(wrld);

        run_python_string("from server import *");
        run_python_string("import testprop");
        run_python_string("t=Thing('1')");
        run_python_string("t.props.line == None");
        run_python_string("t.props.statistics == None");
        run_python_string("t.props.terrain == None");
        run_python_string("testprop.add_properties(t)");
        run_python_string("t.props.line");
        run_python_string("t.props.statistics");
        run_python_string("t.props.terrain");

    }
    shutdown_python_api();
    return 0;
}


