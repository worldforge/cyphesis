// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_WORLD_H
#define RULESETS_PY_WORLD_H

#include <Python.h>

class BaseWorld;

/// \brief Wrapper for World in Python
typedef struct {
    PyObject_HEAD
    BaseWorld * world;
} PyWorld;

extern PyTypeObject PyWorld_Type;

#define PyWorld_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyWorld_Type)

PyWorld * newPyWorld();

#endif // RULESETS_PY_WORLD_H
