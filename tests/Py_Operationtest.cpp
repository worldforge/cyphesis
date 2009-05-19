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

// $Id$

#include <Python.h>

#include "rulesets/Python_API.h"

#include <cassert>

int main()
{
    init_python_api();

    assert(PyRun_SimpleString("from atlas import Operation") == 0);
    assert(PyRun_SimpleString("o=Operation()") == -1);
    assert(PyRun_SimpleString("o=Operation(1)") == -1);
    assert(PyRun_SimpleString("o=Operation('get')") == 0);
    assert(PyRun_SimpleString("o.setSerialno(1)") == 0);
    assert(PyRun_SimpleString("o.setSerialno('1')") == -1);
    assert(PyRun_SimpleString("o.setRefno(1)") == 0);
    assert(PyRun_SimpleString("o.setRefno('1')") == -1);
    assert(PyRun_SimpleString("o.setTo('1')") == 0);
    assert(PyRun_SimpleString("o.setTo(1)") == -1);
    assert(PyRun_SimpleString("o.setFrom('2')") == 0);
    assert(PyRun_SimpleString("o.setFrom(2)") == -1);
    assert(PyRun_SimpleString("o.setSeconds(2)") == 0);
    assert(PyRun_SimpleString("o.setSeconds(2.0)") == 0);
    assert(PyRun_SimpleString("o.setSeconds('2.0')") == -1);
    assert(PyRun_SimpleString("o.setFutureSeconds(2)") == 0);
    assert(PyRun_SimpleString("o.setFutureSeconds(2.0)") == 0);
    assert(PyRun_SimpleString("o.setFutureSeconds('2.0')") == -1);
    assert(PyRun_SimpleString("o.setArgs()") == -1);
    assert(PyRun_SimpleString("o.setArgs([])") == 0);
    assert(PyRun_SimpleString("o.setArgs(1)") == -1);
    assert(PyRun_SimpleString("o.setArgs([1])") == -1);
    assert(PyRun_SimpleString("o.setArgs([Operation('get')])") == 0);
    assert(PyRun_SimpleString("o.getSerialno()") == 0);
    assert(PyRun_SimpleString("o.getRefno()") == 0);
    assert(PyRun_SimpleString("o.getTo()") == 0);
    assert(PyRun_SimpleString("o.getFrom()") == 0);
    assert(PyRun_SimpleString("o.getSeconds()") == 0);
    assert(PyRun_SimpleString("o.getFutureSeconds()") == 0);
    assert(PyRun_SimpleString("o.getArgs()") == 0);
    assert(PyRun_SimpleString("o.get_name()") == 0);
    assert(PyRun_SimpleString("len(o)") == 0);
    assert(PyRun_SimpleString("o[0]") == 0);
    assert(PyRun_SimpleString("o[1]") == -1);
    assert(PyRun_SimpleString("o + None") == 0);
    assert(PyRun_SimpleString("o + 1") == -1);
    assert(PyRun_SimpleString("from atlas import Message") == 0);
    assert(PyRun_SimpleString("o + Message()") == 0);
    assert(PyRun_SimpleString("o + Operation('get')") == 0);
    assert(PyRun_SimpleString("print o.from_") == 0);
    assert(PyRun_SimpleString("print o.to") == 0);
    assert(PyRun_SimpleString("print o.id") == 0);
    assert(PyRun_SimpleString("o.from_='1'") == -1);
    assert(PyRun_SimpleString("o.from_=1") == -1);
    assert(PyRun_SimpleString("o.to='1'") == -1);
    assert(PyRun_SimpleString("o.to=1") == -1);
    assert(PyRun_SimpleString("o.other=1") == -1);
    

    shutdown_python_api();
    return 0;
}
