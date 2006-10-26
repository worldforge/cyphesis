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

// $Id: Py_Property.h,v 1.3 2006-10-26 00:48:11 alriddoch Exp $

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
