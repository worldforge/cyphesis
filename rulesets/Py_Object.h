// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_OBJECT_H
#define RULESETS_PY_OBJECT_H

#include <Atlas/Message/Object.h>
#include <Python.h>

typedef struct {
    PyObject_HEAD
    PyObject			* Object_attr;	// Attributes dictionary
    Atlas::Message::Object	* m_obj;
} AtlasObject;

extern PyTypeObject Object_Type;

#define PyAtlasObject_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Object_Type)

//
// Object creation function.
//

AtlasObject * newAtlasObject(PyObject *arg);

//
// Utility functions to munge between Object related types and python types
//

// PyObject * MapType_asPyObject(const Object::MapType & map);
// PyObject * ListType_asPyObject(const Object::ListType & list);
PyObject * Object_asPyObject(const Atlas::Message::Object & obj);
Atlas::Message::Object::ListType PyListObject_asListType(PyObject * list);
Atlas::Message::Object::MapType PyDictObject_asMapType(PyObject * dict);
Atlas::Message::Object PyObject_asObject(PyObject * o);

#endif // RULESETS_PY_OBJECT_H
