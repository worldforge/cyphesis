// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_OPERATION_H
#define RULESETS_PY_OPERATION_H

#include <Atlas/Objects/Operation/RootOperation.h>

#include <Python.h>

namespace Atlas { namespace Objects { namespace Operation {
  class RootOperation;
} } }

class Entity;

typedef struct {
    PyObject_HEAD
    Atlas::Objects::Operation::RootOperation	* operation;
    int						own;
    Entity					* from;
    Entity					* to;
} OperationObject;

typedef struct {
    PyObject_HEAD
    const Atlas::Objects::Operation::RootOperation	* operation;
    int							own;
    Entity						* from;
    Entity						* to;
} ConstOperationObject;

extern PyTypeObject Operation_Type;
extern PyTypeObject ConstOperation_Type;

#define PyOperation_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Operation_Type)
#define PyConstOperation_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&ConstOperation_Type)

OperationObject * newAtlasRootOperation(PyObject *arg);
ConstOperationObject * newAtlasConstRootOperation(PyObject *arg);

#endif // RULESETS_PY_OPERATION_H
