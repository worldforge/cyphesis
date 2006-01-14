// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_QUATERNION_H
#define RULESETS_PY_QUATERNION_H

#include <Python.h>

#include <physics/Quaternion.h>

/// \brief Wrapper for Quaternion in Python
///
/// Uses in-place construction to embed full C++ object into what is
/// basically a C structure.
typedef struct {
    PyObject_HEAD
    Quaternion rotation;
} PyQuaternion;

extern PyTypeObject PyQuaternion_Type;

#define PyQuaternion_Check(_o) ((_o)->ob_type == &PyQuaternion_Type)

PyQuaternion * newPyQuaternion();

#endif // RULESETS_PY_QUATERNION_H
