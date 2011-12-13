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

#ifndef RULESETS_PY_THING_H
#define RULESETS_PY_THING_H

#include <Python.h>

class BaseMind;
class Character;
class Entity;
class LocatedEntity;

/// \brief Wrapper for Entity in Python
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief Storage for additional attributes
    PyObject * Entity_attr;  // Attributes dictionary
    /// \brief Entity object handled by this wrapper
    union {
        LocatedEntity * l;
        Entity * e;
        Character * c;
        BaseMind * m;
    } m_entity;
    /// \brief List of weak references
    PyObject * m_weakreflist;
} PyEntity;

extern PyTypeObject PyLocatedEntity_Type;
extern PyTypeObject PyEntity_Type;
extern PyTypeObject PyCharacter_Type;
extern PyTypeObject PyMind_Type;

#define PyLocatedEntity_Check(_o) PyObject_TypeCheck(_o, &PyLocatedEntity_Type)
#define PyLocatedEntity_CheckExact(_o) (Py_Type(_o) == &PyLocatedEntity_Type)

#define PyEntity_Check(_o) PyObject_TypeCheck(_o, &PyEntity_Type)
#define PyEntity_CheckExact(_o) (Py_Type(_o) == &PyEntity_Type)

#define PyCharacter_Check(_o) PyObject_TypeCheck(_o, &PyCharacter_Type)
#define PyCharacter_CheckExact(_o) (Py_Type(_o) == &PyCharacter_Type)

#define PyMind_Check(_o) PyObject_TypeCheck(_o, &PyMind_Type)
#define PyMind_CheckExact(_o) (Py_TYPE(_o) == &PyMind_Type)

PyObject * wrapEntity(LocatedEntity * entity);
PyEntity * newPyLocatedEntity();
PyEntity * newPyEntity();
PyEntity * newPyCharacter();
PyEntity * newPyMind();

#endif // RULESETS_PY_THING_H
