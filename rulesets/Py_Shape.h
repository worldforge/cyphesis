// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

#ifndef RULESETS_PY_SHAPE_H
#define RULESETS_PY_SHAPE_H

#include <Python.h>

class Shape;

/// \brief Wrapper for Shape in Python
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief Shape object handled by this wrapper
    Shape * shape;
} PyShape;

extern PyTypeObject PyShape_Type;

#define PyShape_Check(_o) ((_o)->ob_type == &PyShape_Type)

PyShape * newPyShape();

#endif // RULESETS_PY_SHAPE_H
