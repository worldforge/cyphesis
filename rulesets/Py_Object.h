// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_OBJECT_H
#define RULESETS_PY_OBJECT_H

#include <Python.h>

#include <Atlas/Message/Element.h>

typedef struct {
    PyObject_HEAD
    PyObject * Object_attr;  // Attributes dictionary
    Atlas::Message::Element * m_obj;
} PyMessageElement;

extern PyTypeObject PyMessageElement_Type;

#define PyMessageElement_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyMessageElement_Type)

//
// Object creation function.
//

PyMessageElement * newPyMessageElement();

//
// Utility functions to munge between Object related types and python types
//

// PyObject * MapType_asPyObject(const Atlas::Message::MapType & map);
// PyObject * ListType_asPyObject(const Atlas::Message::ListType & list);
PyObject * MessageElement_asPyObject(const Atlas::Message::Element & obj);
Atlas::Message::ListType PyListObject_asElementList(PyObject * list);
Atlas::Message::MapType PyDictObject_asElementMap(PyObject * dict);
Atlas::Message::Element PyObject_asMessageElement(PyObject * o);

#endif // RULESETS_PY_OBJECT_H
