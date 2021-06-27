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
#include "rules/python/CyPy_Atlas.h"
#include <cassert>
#include <rules/python/CyPy_Common.h>
#include <rules/python/CyPy_Physics.h>
#include <rules/python/CyPy_Rules.h>
#include <Atlas/Objects/Factories.h>
#include <common/Inheritance.h>
#include "pythonbase/PythonMalloc.h"

int main()
{
    Atlas::Objects::Factories factories;
    Inheritance inheritance(factories);

    setupPythonMalloc();
    {
        init_python_api({&CyPy_Atlas::init,
                         &CyPy_Physics::init,
                         &CyPy_Common::init,
                         &CyPy_Rules::init});


        run_python_string("from atlas import *");
        run_python_string("from rules import *");
        run_python_string("o=Operation('get')");
        // This should fail, but the error throwing has been disabled to
        // allow cooler stuff to be done from the client.
        // FIXME Once the client is constrained to real operations, go back
        // to disallowing arbitrary operation names.
        // expect_python_error("o=Operation('not_valid')");
        run_python_string("o=Operation('not_valid')");
        run_python_string("o=Operation('get', to='1', from_='1')");
        expect_python_error("o=Operation('get', from_={'nonid': '1'})",
                            PyExc_TypeError);
        expect_python_error("o=Operation('get', from_={'id': 1})",
                            PyExc_TypeError);
        expect_python_error("o=Operation('get', to={'nonid': '1'})",
                            PyExc_TypeError);
        expect_python_error("o=Operation('get', to={'id': 1})",
                            PyExc_TypeError);
        run_python_string("e=Entity('1')");
        run_python_string("o=Operation('get', to=e, from_=e)");
        run_python_string("o=Operation('get', Entity(), to='1', from_='1')");
        run_python_string("o=Operation('get', Operation('set'), to='1', from_='1')");
        expect_python_error("o=Operation('get', Location(), to='1', from_='1')",
                            PyExc_TypeError);
        run_python_string("o=Operation('get', Entity(), Entity(), Entity(), to='1', from_='1')");
        expect_python_error("Operation('get', '1')", PyExc_TypeError);
        run_python_string("Operation('get', {'objtype': 'obj', 'parent': 'thing'})");
        expect_python_error("o=Operation()", PyExc_TypeError);
        expect_python_error("o=Operation(1)", PyExc_TypeError);
        run_python_string("o=Operation('get')");
        run_python_string("o.set_serialno(1)");
        expect_python_error("o.set_serialno('1')", PyExc_TypeError);
        run_python_string("o.set_refno(1)");
        expect_python_error("o.set_refno('1')", PyExc_TypeError);
        run_python_string("o.set_to('1')");
        expect_python_error("o.set_to(1)", PyExc_TypeError);
        run_python_string("o.set_from('2')");
        expect_python_error("o.set_from(2)", PyExc_TypeError);
        run_python_string("o.set_seconds(2)");
        run_python_string("o.set_seconds(2.0)");
        expect_python_error("o.set_seconds('2.0')", PyExc_TypeError);
        run_python_string("o.set_future_seconds(2)");
        run_python_string("o.set_future_seconds(2.0)");
        expect_python_error("o.set_future_seconds('2.0')", PyExc_TypeError);
        expect_python_error("o.set_args()", PyExc_IndexError);
        run_python_string("o.set_args([])");
        expect_python_error("o.set_args(1)", PyExc_TypeError);
        expect_python_error("o.set_args([1])", PyExc_TypeError);
        run_python_string("o.set_args([Operation('get')])");
        run_python_string("o.set_args([Entity(parent=\"oak\")])");
        run_python_string("o.set_args([{'parent': 'root'}])");
        expect_python_error("o.set_args(['1'])", PyExc_TypeError);
        run_python_string("import types");
        run_python_string("assert type(o.get_serialno()) == int");
        run_python_string("assert type(o.get_refno()) == int");
        run_python_string("assert type(o.get_to()) == str");
        run_python_string("assert type(o.get_from()) == str");
        run_python_string("assert type(o.get_seconds()) == float");
        run_python_string("assert type(o.get_future_seconds()) == float");
        run_python_string("assert type(o.get_args()) == list");
        run_python_string("assert type(o.get_name()) == str");
        run_python_string("assert len(o) == 1");
        run_python_string("o.set_args([Operation('get'), Entity(parent=\"oak\"), {'parent': 'root', 'objtype': 'obj'}])");
        run_python_string("assert type(o[0]) == Operation");
        run_python_string("assert type(o[1]) == Entity");
        expect_python_error("o[3]", PyExc_IndexError);
        run_python_string("assert o + None == o");
        expect_python_error("o + 1", PyExc_TypeError);
        run_python_string("assert type(o + Oplist()) == Oplist");
        run_python_string("assert type(o + Operation('get')) == Oplist");
        run_python_string("assert type(o + None) == Operation");
        run_python_string("assert type(None + o) == Operation");
        run_python_string("assert type(o.from_) == str");
        run_python_string("assert type(o.to) == str");
        run_python_string("assert type(o.id) == str");
        run_python_string("o.from_='1'");
        expect_python_error("o.from_=1", PyExc_TypeError);
        expect_python_error("o.from_={'id': 1}", PyExc_TypeError);
        run_python_string("o.from_={'id': '1'}");
        run_python_string("o['from']='ble'");
        run_python_string("assert o['from']=='ble'");
        run_python_string("assert 'from' in o");
        run_python_string("o.to='1'");
        expect_python_error("o.to=1", PyExc_TypeError);
        expect_python_error("o.to={'id': 1}", PyExc_TypeError);
        run_python_string("o.to={'id': '1'}");
        expect_python_error("o.other=1", PyExc_AttributeError);
        run_python_string("opCopy=o.copy()");

    }
    shutdown_python_api();
    return 0;
}
