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
#include "rules/Py_Thing.h"
#include "rules/Py_Property.h"
#include "rules/Entity.h"
#include "rules/simulation/TerrainModProperty.h"

#include "physics/Shape.h"

#include <wfmath/polygon.h>

#include <cassert>
#include <rules/Py_Property.h>

#include "pycxx/CXX/Extensions.hxx"
#include "rules/simulation/python/CyPy_Entity.h"


class TestProp : public Py::ExtensionModule<TestProp>
{
    public:
        Py::Object add_properties(const Py::Tuple& args)
        {
            auto ent = CyPy_Entity::value(args.front());

            PropertyBase * p = ent->setProperty("terrainmod", new TerrainModProperty);
            p->install(ent, "terrainmod");
            p->apply(ent);
            ent->propertyApplied("terrainmod", *p);

            return Py::None();
        }

        Py::Object add_terrainmod_shape(const Py::Tuple& args)
        {
            auto ent = CyPy_Entity::value(args.front());

            //FIXME: if this is to be enabled we need to implement TerrainModProperty support in Python bindings
            //auto prop = dynamic_cast<TerrainModProperty *>(o->m_entity->modProperty(TerrainModProperty::property_name));
            //o->m_p.terrainmod = prop;

            WFMath::Polygon<2> raw_polygon;
            raw_polygon.addCorner(0, WFMath::Point<2>(1,1));
            raw_polygon.addCorner(0, WFMath::Point<2>(1,0));
            raw_polygon.addCorner(0, WFMath::Point<2>(0,0));
            MathShape<WFMath::Polygon, 2> polygon(raw_polygon);
            Atlas::Message::MapType shape_data;
            polygon.toAtlas(shape_data);

            prop->setAttr("shape", shape_data);
            prop->setAttr("nonshape", "testval");

            return Py::None();
        }

        TestProp() : ExtensionModule("testprop")
        {
            add_varargs_method("add_properties", &TestProp::add_properties, "");
            add_varargs_method("add_terrainmod_shape", &TestProp::add_terrainmod_shape, "");

            initialize("testprop");
        }

};

int main()
{
    setupPythonMalloc();
    {
        PyImport_AppendInittab("testprop", []() {
            auto module = new TestProp();
            return module->module().ptr();
        });
        init_python_api("db35f202-3ebb-4df6-bf9e-4e840f6d7eb3");

        run_python_string("from server import *");
        run_python_string("import physics");
        run_python_string("import testprop");
        run_python_string("t=Thing('1')");
        run_python_string("t.props.terrainmod == None");
        run_python_string("testprop.add_properties(t)");
        run_python_string("terrainmod = t.props.terrainmod");
        expect_python_error("terrainmod.foo = 1", PyExc_AttributeError);
        expect_python_error("terrainmod.foo", PyExc_AttributeError);
        expect_python_error("terrainmod.shape", PyExc_AttributeError);
        expect_python_error("terrainmod.nonshape", PyExc_AttributeError);
        run_python_string("testprop.add_terrainmod_shape(terrainmod)");
        run_python_string("assert type(terrainmod.shape) == physics.Area");
        run_python_string("assert terrainmod.nonshape == 'testval'");
        run_python_string("print('test1')");
        run_python_string("terrainmod.shape = physics.Polygon([[ -0.7, -0.7],"
                          "[ -1.0,  0.0],"
                          "[ -0.7,  0.7]])");

    }
    shutdown_python_api();
    return 0;
}
