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

#include "TestWorld.h"

#include "rulesets/Python_API.h"
#include "rulesets/Entity.h"
#include "rulesets/LineProperty.h"
#include "rulesets/StatisticsProperty.h"
#include "rulesets/TerrainProperty.h"
#include "rulesets/python/CyPy_Entity.h"

#include "external/pycxx/CXX/Extensions.hxx"


class TestProp : public Py::ExtensionModule<TestProp>
{
    public:
        Py::Object add_properties(const Py::Tuple& args)
        {
            auto ent = CyPy_Entity::value(args.front());

            PropertyBase * p = ent->setProperty("statistics", new StatisticsProperty);
            p->install(ent, "statistics");
            p->apply(ent);
            ent->propertyApplied("statistics", *p);
            p = ent->setProperty("terrain", new TerrainProperty);
            p->install(ent, "terrain");
            p->apply(ent);
            ent->propertyApplied("terrain", *p);
            p = ent->setProperty("line", new LineProperty);
            p->install(ent, "line");
            p->apply(ent);
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
    PyImport_AppendInittab("testprop", [](){
        auto module = new TestProp();
        return module->module().ptr();
    });
    init_python_api("b513b7b1-b0d8-4495-b3f0-54c2ef3f27f6");


    Entity wrld("0", 0);
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


    shutdown_python_api();
    return 0;
}


