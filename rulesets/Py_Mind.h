// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef PY_MIND_H
#define PY_MIND_H

extern PyTypeObject Mind_Type;

#define PyMind_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Mind_Type)

MindObject * newMindObject(PyObject *);

#endif // PY_MIND_H
