// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_THING_H
#define RULESETS_PY_THING_H

#include <Python.h>

class Entity;

typedef struct {
    PyObject_HEAD
    PyObject * Entity_attr;  // Attributes dictionary
    Entity * m_entity;
} PyEntity;

extern PyTypeObject PyEntity_Type;

#define PyEntity_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyEntity_Type)

PyEntity * newPyEntity();

#endif // RULESETS_PY_THING_H
