// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

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
