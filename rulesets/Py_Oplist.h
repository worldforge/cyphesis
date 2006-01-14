// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_OPLIST_H
#define RULESETS_PY_OPLIST_H

#include <Python.h>

#include <vector>

#include <Atlas/Objects/RootOperation.h>

typedef std::vector<Atlas::Objects::Operation::RootOperation> OpVector;

/// \brief Wrapper for OpVector in Python
typedef struct {
    PyObject_HEAD
    OpVector * ops;
} PyOplist;

extern PyTypeObject PyOplist_Type;

#define PyOplist_Check(_o) ((_o)->ob_type == &PyOplist_Type)

PyOplist * newPyOplist();

#endif // RULESETS_PY_VECTOR3D_H
