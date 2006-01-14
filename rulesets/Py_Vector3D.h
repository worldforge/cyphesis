// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

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
