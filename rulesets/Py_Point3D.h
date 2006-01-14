// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_POINT3D_H
#define RULESETS_PY_POINT3D_H

#include <Python.h>

#include <physics/Vector3D.h>

/// \brief Wrapper for Point3D in Python
///
/// Uses in-place construction to embed full C++ object into what is
/// basically a C structure.
typedef struct {
    PyObject_HEAD
    Point3D coords;
} PyPoint3D;

extern PyTypeObject PyPoint3D_Type;

#define PyPoint3D_Check(_o) ((_o)->ob_type == &PyPoint3D_Type)

PyPoint3D * newPyPoint3D();

#endif // RULESETS_PY_POINT3D_H
