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

// $Id: Py_Thing.h,v 1.16 2006-10-26 00:48:11 alriddoch Exp $

#ifndef RULESETS_PY_THING_H
#define RULESETS_PY_THING_H

#include <Python.h>

class Entity;
class Character;

/// \brief Wrapper for Entity in Python
typedef struct {
    PyObject_HEAD
    PyObject * Entity_attr;  // Attributes dictionary
    Entity * m_entity;
    struct PyMethodDef * m_methods;
} PyEntity;

/// \brief Wrapper for Character in Python
typedef struct {
    PyObject_HEAD
    PyObject * Entity_attr;  // Attributes dictionary
    Character * m_entity;
    struct PyMethodDef * m_methods;
} PyCharacter;

extern PyTypeObject PyEntity_Type;

#define PyEntity_Check(_o) ((_o)->ob_type == &PyEntity_Type)

PyObject * wrapEntity(Entity * entity);
PyEntity * newPyEntity();
PyCharacter * newPyCharacter();

#endif // RULESETS_PY_THING_H
