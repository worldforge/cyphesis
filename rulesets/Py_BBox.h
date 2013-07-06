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


#ifndef RULESETS_PY_B_BOX_H
#define RULESETS_PY_B_BOX_H

#include <Python.h>

#include "physics/BBox.h"

#include <wfmath/axisbox.h>
#include <wfmath/point.h>

/// \brief Wrapper for BBox for Python
///
/// Uses in-place construction to embed full C++ object into what is
/// basically a C structure.
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief BBox object handled by this wrapper
    BBox box;
} PyBBox;

extern PyTypeObject PyBBox_Type;

#define PyBBox_Check(_o) ((_o)->ob_type == &PyBBox_Type)

PyBBox * newPyBBox();

#endif // RULESETS_PY_B_BOX_H
