// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_VECTOR3D_H
#define RULESETS_PY_VECTOR3D_H

#include <physics/Vector3D.h>

#include <Python.h>

typedef struct {
    PyObject_HEAD
    Vector3D coords;
} Vector3DObject;

extern PyTypeObject Vector3D_Type;

#define PyVector3D_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Vector3D_Type)

Vector3DObject * newVector3DObject(PyObject *);

#endif // RULESETS_PY_VECTOR3D_H
