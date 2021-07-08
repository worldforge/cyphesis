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
#include <rules/python/CyPy_Physics.h>
#include <rules/python/CyPy_Rules.h>
#include <rules/python/CyPy_Common.h>
#include <rules/python/CyPy_Element.h>
#include <Atlas/Objects/Factories.h>
#include <common/Inheritance.h>
#include "pythonbase/PythonMalloc.h"
#include "common/debug.h"
Atlas::Objects::Factories factories;
Inheritance inheritance(factories);

using Atlas::Message::Element;

int main()
{

    setupPythonMalloc();
    class Test : public Py::ExtensionModule<Test>
    {
        public:
            Py::Object convert(const Py::Tuple& args)
            {
                auto element = CyPy_Element::asElement(args.front());
                return CyPy_Element::asPyObject(element, args.getItem(1).as_bool());
            }

            Test() : ExtensionModule("test")
            {

                add_varargs_method("convert", &Test::convert, "");


                initialize("test");
            }

    };
    {
        PyImport_AppendInittab("test", []() {
            static Test testModule;
            return testModule.module().ptr();
        });

        init_python_api({&CyPy_Rules::init,
                         &CyPy_Atlas::init,
                         &CyPy_Physics::init,
                         &CyPy_Common::init});

        assert(CyPy_Element::asElement(Py::Long(4)) == Element(4));
        assert(CyPy_Element::asElement(Py::Long(4)) != Element(4.0));
        assert(CyPy_Element::asElement(Py::Long(4)) != Element("foo"));
        assert(CyPy_Element::asElement(Py::Float(4.0)) == Element(4.0));
        assert(CyPy_Element::asElement(Py::Float(4.0)) != Element(4));
        assert(CyPy_Element::asElement(Py::Float(4.0)) != Element("foo"));
        assert(CyPy_Element::asElement(Py::String("foo")) == Element("foo"));
        assert(CyPy_Element::asElement(Py::String("foo")) != Element(4));
        assert(CyPy_Element::asElement(Py::String("foo")) != Element(4.0));


        run_python_string("from test import convert");
        run_python_string("from atlas import ElementList");
        run_python_string("from atlas import ElementMap");
        run_python_string("from atlas import Operation");
        run_python_string("from atlas import Oplist");
        run_python_string("from rules import Location");
        run_python_string("from physics import Vector3D");
        run_python_string("assert convert(None, True) == None");
        run_python_string("assert convert(2, True) == 2");
        run_python_string("assert convert(2.0, True) == 2.0");
        run_python_string("assert convert('foo', True) == 'foo'");
        run_python_string("assert convert([], True) == []");
        run_python_string("assert convert([], False) == []");
        run_python_string("assert convert({}, True) == {}");
        run_python_string("assert convert({}, False) == {}");
        run_python_string("assert convert([1, 'foo', [2]], True) == [1, 'foo', [2]]");
        run_python_string("assert convert([1, 'foo', [2]], False) == [1, 'foo', ElementList(2)]");
        run_python_string("assert convert({'foo': 'bar', 'baz': 2, 'biz': {'wee': 1}}, True) == {'foo': 'bar', 'baz': 2, 'biz': {'wee': 1}}");
        run_python_string("assert convert({'foo': 'bar', 'baz': 2, 'biz': {'wee': 1}}, False) == {'foo': 'bar', 'baz': 2, 'biz': ElementMap(wee=1)}");
        run_python_string("assert convert(Operation('get'), True) == {'objtype': 'op', 'parent': 'get'}");
        run_python_string("assert convert(Operation('get'), True) == {'objtype': 'op', 'parent': 'get'}");
        run_python_string("assert convert(Oplist(Operation('get')), True) == [{'objtype': 'op', 'parent': 'get'}]");
        run_python_string("assert convert(Oplist(Operation('get')), False) == [ElementMap(objtype='op', parent='get')]");
        run_python_string("assert convert(Location(), True) == {}");
        run_python_string("assert convert(Vector3D(), True) == []");
        run_python_string("assert convert(Vector3D(1.0, 2.0, 3.0), True) == [1.0, 2.0, 3.0]");
        run_python_string("aList = ElementList(1, 2)")
        run_python_string("anotherList = []")
        run_python_string("for i in aList: anotherList.append(i)")
        run_python_string("assert anotherList == [1, 2]");
        run_python_string("aMap = ElementMap(foo='bar', foz=1)")
        run_python_string("anotherMap = {}")
        run_python_string("for key, value in aMap.items(): anotherMap[key] = value")
        run_python_string("assert anotherMap == {'foo': 'bar', 'foz': 1}")

    }

    shutdown_python_api();
    return 0;
}
