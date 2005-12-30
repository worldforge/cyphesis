// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_PY_PROPERTY_H
#define RULESETS_PY_PROPERTY_H

#include <Python.h>

class Entity;
class PropertyBase;
class StatisticsProperty;

/// \brief Wrapper for generic properties. This is in effect a base class
/// for wrappers for properties of more specific type, all of which will
/// have an m_property which points to an object which inherits from
/// PropertyBase.
typedef struct {
    PyObject_HEAD
    Entity * m_entity;
    PropertyBase * m_property;
} PyProperty;

typedef struct {
    PyObject_HEAD
    Entity * m_entity;
    StatisticsProperty * m_property;
} PyStatisticsProperty;

PyObject * Property_asPyObject(PropertyBase * property, Entity * owner);

#endif // RULESETS_PY_PROPERTY_H
