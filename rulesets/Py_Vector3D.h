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

#ifndef RULESETS_PY_VECTOR3D_H
#define RULESETS_PY_VECTOR3D_H

#include <Python.h>

#include <physics/Vector3D.h>

/// \brief Wrapper for Vector3D in Pyton
///
/// Uses in-place construction to embed full C++ object into what is
/// basically a C structure.
typedef struct {
    PyObject_HEAD
    Vector3D coords;
} PyVector3D;

extern PyTypeObject PyVector3D_Type;

#define PyVector3D_Check(_o) ((_o)->ob_type == &PyVector3D_Type)

PyVector3D * newPyVector3D();

#endif // RULESETS_PY_VECTOR3D_H
