// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_THING_H
#define RULESETS_PY_THING_H

#include <Python.h>

class Entity;

typedef struct {
    PyObject_HEAD
    PyObject	* Thing_attr;	// Attributes dictionary
    Entity	* m_thing;
} ThingObject;

extern PyTypeObject Thing_Type;

#define PyThing_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Thing_Type)

ThingObject * newThingObject(PyObject *);

#endif // RULESETS_PY_THING_H
