// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"

extern PyTypeObject Object_Type;

#define PyAtlasObject_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Object_Type)

/*
 * Object creation function.
 */

AtlasObject * newAtlasObject(PyObject *arg);

/*
 * Utility functions to munge between Object related types and python types
 */

PyObject * MapType_asPyObject(Object::MapType & map);
PyObject * ListType_asPyObject(Object::ListType & list);
PyObject * Object_asPyObject(Object & obj);
Object::ListType PyListObject_asListType(PyObject * list);
Object::MapType PyDictObject_asMapType(PyObject * dict);
Object PyObject_asObject(PyObject * o);

