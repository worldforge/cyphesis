// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_OPTIME_H
#define RULESETS_PY_OPTIME_H

#include <Python.h>

namespace Atlas { namespace Objects { namespace Operation {
    class RootOperation;
} } }

typedef struct {
    PyObject_HEAD
    Atlas::Objects::Operation::RootOperation * operation;
} PyOptime;

extern PyTypeObject PyOptime_Type;

#define PyOptime_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyOptime_Type)

PyOptime * newPyOptime();

#endif // RULESETS_PY_OPTIME_H
