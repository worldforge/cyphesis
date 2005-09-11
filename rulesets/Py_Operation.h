// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_OPERATION_H
#define RULESETS_PY_OPERATION_H

#include <Python.h>

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

class Entity;

/// \brief Wrapper for all Atlas operations in Python
typedef struct {
    PyObject_HEAD
    Atlas::Objects::Operation::RootOperation operation;
} PyOperation;

/// \brief Wrapper for read only Atlas operations in Python
typedef struct {
    PyObject_HEAD
    Atlas::Objects::Operation::RootOperation operation;
} PyConstOperation;

extern PyTypeObject PyOperation_Type;
extern PyTypeObject PyConstOperation_Type;

#define PyOperation_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyOperation_Type)
#define PyConstOperation_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyConstOperation_Type)

PyOperation * newPyOperation();
PyConstOperation * newPyConstOperation();

#endif // RULESETS_PY_OPERATION_H
