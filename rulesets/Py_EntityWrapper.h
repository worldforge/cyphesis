// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_PY_ENTITY_WRAPPER_H
#define RULESETS_PY_ENTITY_WRAPPER_H

#include <Python.h>

class Entity;

/// \brief Wrapper for specific entity submodules in Python
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
typedef struct {
    PyObject_HEAD
    Entity * m_entity;
} PyEntityWrapper;

#endif // RULESETS_PY_ENTITY_WRAPPER_H
