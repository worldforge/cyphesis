// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_OPTIME_H
#define RULESETS_PY_OPTIME_H

#include <Atlas/Objects/Operation/RootOperation.h>
#include <Python.h>

typedef struct {
    PyObject_HEAD
    Atlas::Objects::Operation::RootOperation	* operation;
} OptimeObject;

extern PyTypeObject Optime_Type;

#define PyOptime_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Optime_Type)

OptimeObject * newOptimeObject(PyObject *);

#endif // RULESETS_PY_OPTIME_H
