// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_QUATERNION_H
#define RULESETS_PY_QUATERNION_H

#include <Python.h>

#include <physics/Quaternion.h>

typedef struct {
    PyObject_HEAD
    Quaternion rotation;
} PyQuaternion;

extern PyTypeObject PyQuaternion_Type;

#define PyQuaternion_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyQuaternion_Type)

PyQuaternion * newPyQuaternion();

#endif // RULESETS_PY_QUATERNION_H
