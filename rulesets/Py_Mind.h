// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_MIND_H
#define RULESETS_PY_MIND_H

#include <Python.h>

class BaseMind;

typedef struct {
    PyObject_HEAD
    PyObject	* Mind_attr;	// Attributes dictionary
    BaseMind	* m_mind;
} MindObject;

extern PyTypeObject Mind_Type;

#define PyMind_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Mind_Type)

MindObject * newMindObject(PyObject *);

#endif // RULESETS_PY_MIND_H
