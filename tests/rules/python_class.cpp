// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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

#include "pythonbase/Python_API.h"
#include "rules/Py_Vector3D.h"

#include "common/globals.h"

int main(int argc, char ** argv)
{
    loadConfig(argc, argv);

    init_python_api("f5a8a981-e9ac-4f3b-a8f6-528add44da87");

    PyVector3D * pv = newPyVector3D();

    if (PyErr_Occurred() != 0) {
        PyErr_Print();
    }

    PyObject * pv2 = PyInstance_New((PyObject*)&PyVector3D_Type, 0, 0);

    if (PyErr_Occurred() != 0) {
        PyErr_Print();
    }

    shutdown_python_api();
}
