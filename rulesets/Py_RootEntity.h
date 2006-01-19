// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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

#ifndef RULESETS_PY_ENTITY_H
#define RULESETS_PY_ENTITY_H

#include <Python.h>

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/SmartPtr.h>

/// \brief Wrapper for all Atlas entities in Python
typedef struct {
    PyObject_HEAD
    Atlas::Objects::Entity::RootEntity entity;
} PyRootEntity;

/// \brief Wrapper for read only Atlas entities in Python
typedef struct {
    PyObject_HEAD
    Atlas::Objects::Entity::RootEntity entity;
} PyConstRootEntity;

extern PyTypeObject PyRootEntity_Type;
extern PyTypeObject PyConstRootEntity_Type;

#define PyRootEntity_Check(_o) ((_o)->ob_type == &PyRootEntity_Type)
#define PyConstRootEntity_Check(_o) ((_o)->ob_type == &PyConstRootEntity_Type)

PyRootEntity * newPyRootEntity();
PyConstRootEntity * newPyConstRootEntity();

#endif // RULESETS_PY_ENTITY_H
