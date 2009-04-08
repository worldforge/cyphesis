// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#ifndef RULESETS_PY_OBJECT_H
#define RULESETS_PY_OBJECT_H

#include <Python.h>

#include <Atlas/Message/Element.h>

/// \brief Wrapper for Atlas::Message::Element in Python
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief Atlas::Message::Element object handled by this wrapper
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

PyObject * MessageElement_asPyObject(const Atlas::Message::Element & obj);
Atlas::Message::Element PyObject_asMessageElement(PyObject * o, bool simple = false);

#endif // RULESETS_PY_OBJECT_H
