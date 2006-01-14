// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

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
