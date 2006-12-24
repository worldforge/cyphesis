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

// $Id: Py_EntityWrapper.h,v 1.5 2006-12-24 17:18:55 alriddoch Exp $

#ifndef RULESETS_PY_ENTITY_WRAPPER_H
#define RULESETS_PY_ENTITY_WRAPPER_H

#include <Python.h>

class Entity;

/// \brief Wrapper for specific entity submodules in Python
///
/// This has been written to wrap Statistics, but as Statistics is always
/// a member of entity, and entity is ref counted it makes sense to
/// store a reference to the entity, so this wrapper can simple hold a
/// reference to the entity, rather than risk pointing to the statistics
/// member for a deleted entity. As there is nothing in the structure
/// for this type that is specific to Statistics, the struct has been
/// names PyEntityWrapper, so it can be used to solve similar requirements
/// for other submodules of entity. The methods for a python object are
/// defined by its PyTypeObject, which is independant of the struct
/// used.
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief Entity object handled by this wrapper
    Entity * m_entity;
} PyEntityWrapper;

#endif // RULESETS_PY_ENTITY_WRAPPER_H
