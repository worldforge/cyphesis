// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_BBOX_H
#define RULESETS_PY_BBOX_H

#include <physics/BBox.h>

#include <Python.h>

typedef struct {
    PyObject_HEAD
    BBox box;
} PyBBox;

extern PyTypeObject PyBBox_Type;

#define PyBBox_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyBBox_Type)

PyBBox * newPyBBox();

#endif // RULESETS_PY_BBOX_H
