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

#include "python_testers.h"

#include "rulesets/Python_API.h"
#include "rulesets/Py_Thing.h"
#include "rulesets/Entity.h"
#include "rulesets/LineProperty.h"
#include "rulesets/StatisticsProperty.h"
#include "rulesets/TerrainProperty.h"

#include <cassert>

static PyObject * add_properties(PyObject * self, PyEntity * o)
{
    if (!PyEntity_Check(o) && !PyCharacter_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Unknown Object type");
        return NULL;
    }

    Entity * ent = o->m_entity.e;
    
    PropertyBase * p = ent->setProperty("statistics", new StatisticsProperty);
    p->install(ent);
    p->apply(ent);
    p = ent->setProperty("terrain", new TerrainProperty);
    p->install(ent);
    p->apply(ent);
    p = ent->setProperty("line", new LineProperty);
    p->install(ent);
    p->apply(ent);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef testprop_methods[] = {
    {"add_properties", (PyCFunction)add_properties,                 METH_O},
    {NULL,          NULL}                       /* Sentinel */
};

static void setup_test_functions()
{
    PyObject * testprop = Py_InitModule("testprop", testprop_methods);
    assert(testprop != 0);
}

int main()
{
    init_python_api();

    setup_test_functions();

    run_python_string("from server import *");
    run_python_string("import testprop");
    run_python_string("t=Thing('1')");
    fail_python_string("t.line");
    fail_python_string("t.statistics");
    fail_python_string("t.terrain");
    run_python_string("testprop.add_properties(t)");
    run_python_string("t.line");
    run_python_string("t.statistics");
    run_python_string("t.terrain");


    shutdown_python_api();
    return 0;
}
