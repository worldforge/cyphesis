// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_OPLIST_H
#define RULESETS_PY_OPLIST_H

#include <common/types.h>
#include <Python.h>

typedef struct {
    PyObject_HEAD
    OpVector	* ops;
} OplistObject;

extern PyTypeObject Oplist_Type;

#define PyOplist_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Oplist_Type)

OplistObject * newOplistObject(PyObject *);

#endif // RULESETS_PY_VECTOR3D_H
