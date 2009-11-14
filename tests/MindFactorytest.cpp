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
#include "rulesets/MindFactory.h"

#include "common/TypeNode.h"

#include <cassert>

static PyMethodDef no_methods[] = {
    {NULL,          NULL}                       /* Sentinel */
};

static const char * etype = "settler";

int main()
{
    init_python_api();

    Py_InitModule("testmod", no_methods);

    assert(PyRun_SimpleString("import server") == 0);
    assert(PyRun_SimpleString("import testmod") == 0);
    assert(PyRun_SimpleString("from atlas import Operation") == 0);
    assert(PyRun_SimpleString("class settlerMind(server.Thing):\n"
                              " def look_operation(self, op): pass\n"
                              " def delete_operation(self, op):\n"
                              "  return Operation('sight') + Operation('move')\n"
                              " def test_hook(self, ent): pass\n") == 0);
    assert(PyRun_SimpleString("testmod.settlerMind=settlerMind") == 0);

    TypeNode * tn = new TypeNode();
    tn->name() = etype;

    MindFactory * mf = MindFactory::instance();

    mf->addMindType(etype, "testmod");

    mf->newMind("1", 1, tn);

    tn->name() = etype;

    mf->newMind("2", 2, tn);

    shutdown_python_api();
    return 0;
}
