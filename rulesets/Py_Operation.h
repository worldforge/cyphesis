// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_OPERATION_H
#define RULESETS_PY_OPERATION_H

#include <Python.h>

namespace Atlas { namespace Objects { namespace Operation {
  class RootOperation;
} } }

class Entity;

typedef struct {
    PyObject_HEAD
    Atlas::Objects::Operation::RootOperation * operation;
    int own;
    Entity * from;
    Entity * to;
} PyOperation;

typedef struct {
    PyObject_HEAD
    const Atlas::Objects::Operation::RootOperation * operation;
    int own;
    Entity * from;
    Entity * to;
} PyConstOperation;

extern PyTypeObject PyOperation_Type;
extern PyTypeObject PyConstOperation_Type;

#define PyOperation_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyOperation_Type)
#define PyConstOperation_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyConstOperation_Type)

PyOperation * newPyOperation();
PyConstOperation * newPyConstOperation();

#endif // RULESETS_PY_OPERATION_H
