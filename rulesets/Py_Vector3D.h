// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_VECTOR3D_H
#define RULESETS_PY_VECTOR3D_H

#include <Python.h>

#include <physics/Vector3D.h>

typedef struct {
    PyObject_HEAD
    Vector3D coords;
} PyVector3D;

extern PyTypeObject PyVector3D_Type;

#define PyVector3D_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyVector3D_Type)

PyVector3D * newPyVector3D();

#endif // RULESETS_PY_VECTOR3D_H
