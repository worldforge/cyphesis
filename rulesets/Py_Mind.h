// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_MIND_H
#define RULESETS_PY_MIND_H

#include <Python.h>

class BaseMind;

/// \brief Wrapper for BaseMind in Python
typedef struct {
    PyObject_HEAD
    PyObject * Mind_attr;    // Attributes dictionary
    BaseMind * m_mind;
} PyMind;

extern PyTypeObject PyMind_Type;

#define PyMind_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyMind_Type)

PyMind * newPyMind();

#endif // RULESETS_PY_MIND_H
