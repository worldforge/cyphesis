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
#include "rules/python/CyPy_RootEntity.h"

#include <cassert>
#include <rules/python/CyPy_Atlas.h>
#include <rules/python/CyPy_Physics.h>
#include <rules/python/CyPy_Common.h>
#include <rules/python/CyPy_Rules.h>
#include "pythonbase/PythonMalloc.h"


int main()
{
    setupPythonMalloc();
    {
        init_python_api({&CyPy_Atlas::init,
                         &CyPy_Physics::init,
                         &CyPy_Common::init,
                         &CyPy_Rules::init});


        run_python_string("from atlas import Entity");
        run_python_string("from rules import Location");

        auto ent = (Py::PythonClassObject<CyPy_RootEntity>) (CyPy_RootEntity::wrap(Atlas::Objects::Entity::RootEntity()));
        assert(ent.getCxxObject() != nullptr);
        assert(ent.getCxxObject()->m_value.isValid());
        assert(Py::PythonClassObject<CyPy_RootEntity>::getCxxObject(ent) != nullptr);
        assert(Py::PythonClassObject<CyPy_RootEntity>::getCxxObject(ent)->m_value.isValid());

        run_python_string("Entity('1')");
        expect_python_error("Entity(1)", PyExc_TypeError);
        expect_python_error("Entity('1', location='loc')", PyExc_TypeError);
        run_python_string("l=Location()");
        run_python_string("Entity('1', location=l)");
        run_python_string("Entity('1', pos=())");
        run_python_string("Entity('1', pos=[])");
        expect_python_error("Entity('1', pos=(1,1.0,'1'))", PyExc_TypeError);
        expect_python_error("Entity('1', pos=[1,1.0,'1'])", PyExc_TypeError);
        run_python_string("Entity('1', tasks=[{'name': 'twist', 'param': 'value'}])");
        expect_python_error("Entity('1', pos=1)", PyExc_TypeError);
        expect_python_error("Entity('1', parent=1)", PyExc_TypeError);
        run_python_string("Entity('1', parent='0')");
        expect_python_error("Entity('1', objtype=1)", PyExc_TypeError);
        run_python_string("Entity('1', objtype='pig')");
        run_python_string("Entity('1', other=1)");
        expect_python_error("Entity('1', other=set([1,1]))", PyExc_TypeError);
        run_python_string("e=Entity()");
        run_python_string("e.get_name()");
        run_python_string("e.name");
        run_python_string("e.id");
        expect_python_error("e.foo", PyExc_AttributeError);
        run_python_string("e.name='Bob'");
        expect_python_error("e.name=1", PyExc_TypeError);
        run_python_string("e.foo='Bob'");
        run_python_string("e.bar=1");
        run_python_string("e['bla']=1");
        run_python_string("assert e['bla']==1");
        run_python_string("assert 'bla' in e");
        run_python_string("e.baz=[1,2.0,'three']");
        run_python_string("e.qux={'mim': 23}");
        expect_python_error("e.ptr=set([1,2])", PyExc_TypeError);
        run_python_string("e.foo");
        expect_python_error("e.ptr", PyExc_AttributeError);

        run_python_string("e=Entity('1', {'wee!append': 1})");
        run_python_string("assert e['wee!append']==1");
        run_python_string("e=Entity({'wee!append': 1})");
        run_python_string("assert e['wee!append']==1");

    }
    shutdown_python_api();
    return 0;
}
