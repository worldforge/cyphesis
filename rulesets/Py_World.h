// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000 Alistair Riddoch
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

// $Id: Py_World.h,v 1.13 2006-12-22 02:14:44 alriddoch Exp $

#ifndef RULESETS_PY_WORLD_H
#define RULESETS_PY_WORLD_H

#include <Python.h>

class BaseWorld;

/// \brief Wrapper for World in Python
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    BaseWorld * world;
} PyWorld;

extern PyTypeObject PyWorld_Type;

#define PyWorld_Check(_o) ((_o)->ob_type == &PyWorld_Type)

PyWorld * newPyWorld();

#endif // RULESETS_PY_WORLD_H
