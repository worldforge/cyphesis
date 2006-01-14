// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_OBJECT_H
#define RULESETS_PY_OBJECT_H

#include <Python.h>

#include <Atlas/Message/Element.h>

/// \brief Wrapper for Atlas::Message::Element in Python
typedef struct {
    PyObject_HEAD
    Atlas::Message::Element * m_obj;
} PyMessageElement;

extern PyTypeObject PyMessageElement_Type;

#define PyMessageElement_Check(_o) ((_o)->ob_type == &PyMessageElement_Type)

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
Atlas::Message::Element PyListObject_asElement(PyObject * list);
Atlas::Message::Element PyDictObject_asElement(PyObject * dict);
Atlas::Message::Element PyObject_asMessageElement(PyObject * o);

#endif // RULESETS_PY_OBJECT_H
