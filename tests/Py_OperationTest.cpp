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

#include "python_testers.h"

#include "rulesets/Python_API.h"

#include <cassert>

int main()
{

    init_python_api("3622159a-de3c-42e6-858c-f6bd7cf8e7b1");


    run_python_string("from atlas import *");
    run_python_string("o=Operation('get')");
    // This should fail, but the error throwing has been disabled to
    // allow cooler stuff to be done from the client.
    // FIXME Once the client is constrained to real operations, go back
    // to disallowing arbitrary operation names.
    // expect_python_error("o=Operation('not_valid')");
    run_python_string("o=Operation('not_valid')");
    run_python_string("o=Operation('get', to='1', from_='1')");
    expect_python_error("o=Operation('get', from_=Message({'nonid': '1'}))",
                        PyExc_TypeError);
    expect_python_error("o=Operation('get', from_=Message({'id': 1}))",
                        PyExc_TypeError);
    expect_python_error("o=Operation('get', to=Message({'nonid': '1'}))",
                        PyExc_TypeError);
    expect_python_error("o=Operation('get', to=Message({'id': 1}))",
                        PyExc_TypeError);
    run_python_string("e=Entity('1')");
    run_python_string("o=Operation('get', to=e, from_=e)");
    run_python_string("o=Operation('get', Entity(), to='1', from_='1')");
    run_python_string("o=Operation('get', Operation('set'), to='1', from_='1')");
    expect_python_error("o=Operation('get', Location(), to='1', from_='1')",
                        PyExc_TypeError);
    run_python_string("o=Operation('get', Entity(), Entity(), Entity(), to='1', from_='1')");
    expect_python_error("Operation('get', Message('1'))", PyExc_TypeError);
    run_python_string("Operation('get', Message({'objtype': 'obj', 'parent': 'thing'}))");
    expect_python_error("o=Operation()", PyExc_TypeError);
    expect_python_error("o=Operation(1)", PyExc_TypeError);
    run_python_string("o=Operation('get')");
    run_python_string("o.setSerialno(1)");
    expect_python_error("o.setSerialno('1')", PyExc_TypeError);
    run_python_string("o.setRefno(1)");
    expect_python_error("o.setRefno('1')", PyExc_TypeError);
    run_python_string("o.setTo('1')");
    expect_python_error("o.setTo(1)", PyExc_TypeError);
    run_python_string("o.setFrom('2')");
    expect_python_error("o.setFrom(2)", PyExc_TypeError);
    run_python_string("o.setSeconds(2)");
    run_python_string("o.setSeconds(2.0)");
    expect_python_error("o.setSeconds('2.0')", PyExc_TypeError);
    run_python_string("o.setFutureSeconds(2)");
    run_python_string("o.setFutureSeconds(2.0)");
    expect_python_error("o.setFutureSeconds('2.0')", PyExc_TypeError);
    expect_python_error("o.setArgs()", PyExc_IndexError);
    run_python_string("o.setArgs([])");
    expect_python_error("o.setArgs(1)", PyExc_TypeError);
    expect_python_error("o.setArgs([1])", PyExc_TypeError);
    run_python_string("o.setArgs([Operation('get')])");
    run_python_string("o.setArgs([Entity(parent=\"oak\")])");
    run_python_string("o.setArgs([Message({'parent': 'root'})])");
    expect_python_error("o.setArgs([Message('1')])", PyExc_TypeError);
    run_python_string("import types");
    run_python_string("assert type(o.getSerialno()) == int");
    run_python_string("assert type(o.getRefno()) == int");
    run_python_string("assert type(o.getTo()) == str");
    run_python_string("assert type(o.getFrom()) == str");
    run_python_string("assert type(o.getSeconds()) == float");
    run_python_string("assert type(o.getFutureSeconds()) == float");
    run_python_string("assert type(o.getArgs()) == list");
    run_python_string("assert type(o.get_name()) == str");
    run_python_string("assert len(o) == 1");
    run_python_string("o.setArgs([Operation('get'), Entity(parent=\"oak\"), Message({'parent': 'root', 'objtype': 'obj'})])");
    run_python_string("assert type(o[0]) == Operation");
    run_python_string("assert type(o[1]) == Entity");
    run_python_string("assert type(o[2]) == Message");
    expect_python_error("o[3]", PyExc_IndexError);
    run_python_string("assert o + None == o");
    expect_python_error("o + 1", PyExc_TypeError);
    run_python_string("assert type(o + Oplist()) == Oplist");
    run_python_string("assert type(o + Operation('get')) == Oplist");
    run_python_string("assert type(o.from_) == str");
    run_python_string("assert type(o.to) == str");
    run_python_string("assert type(o.id) == str");
    expect_python_error("o.from_='1'", PyExc_TypeError);
    expect_python_error("o.from_=1", PyExc_TypeError);
    expect_python_error("o.from_=Message({'id': 1})", PyExc_TypeError);
    run_python_string("o.from_=Message({'id': '1'})");
    expect_python_error("o.to='1'", PyExc_TypeError);
    expect_python_error("o.to=1", PyExc_TypeError);
    expect_python_error("o.to=Message({'id': 1})", PyExc_TypeError);
    run_python_string("o.to=Message({'id': '1'})");
    expect_python_error("o.other=1", PyExc_AttributeError);

    shutdown_python_api();
    return 0;
}
