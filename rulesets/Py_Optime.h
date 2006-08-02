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

#error This file has been removed from the build
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef RULESETS_PY_OPTIME_H
#define RULESETS_PY_OPTIME_H

#include <Python.h>

namespace Atlas { namespace Objects { namespace Operation {
    class RootOperation;
} } }

/// \brief Wrapper for handling operation time in python
///
/// Python scripts use this type in order to modify the time attribute
/// of an operation, so its necessary to keep a pointer to the operation
/// itself.
typedef struct {
    PyObject_HEAD
    Atlas::Objects::Operation::RootOperation * operation;
} PyOptime;

extern PyTypeObject PyOptime_Type;

#define PyOptime_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyOptime_Type)

PyOptime * newPyOptime();

#endif // RULESETS_PY_OPTIME_H
