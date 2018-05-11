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


#ifndef RULESETS_PY_PROPERTY_H
#define RULESETS_PY_PROPERTY_H

#include <Python.h>

class LocatedEntity;
class PropertyBase;
class StatisticsProperty;
class TerrainModProperty;
class TerrainProperty;

/// \brief Wrapper for generic properties. This is in effect a base class
/// for wrappers for properties of more specific type, all of which will
/// have an m_property which points to an object which inherits from
/// PropertyBase.
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief Entity object that owns the Property
    LocatedEntity * m_entity;
    /// \brief Property object handled by this wrapper
    union {
        const PropertyBase * base;
        const TerrainProperty * terrain;
        const TerrainModProperty * terrainmod;
    } m_p;
} PyProperty;

extern PyTypeObject PyProperty_Type;
extern PyTypeObject PyTerrainProperty_Type;
extern PyTypeObject PyTerrainModProperty_Type;

#define PyTerrainProperty_Check(_o) PyObject_TypeCheck(_o, &PyTerrainProperty_Type)
#define PyTerrainProperty_CheckExact(_o) (Py_Type(_o) == &PyTerrainProperty_Type)

#define PyTerrainModProperty_Check(_o) PyObject_TypeCheck(_o, &PyTerrainModProperty_Type)
#define PyTerrainModProperty_CheckExact(_o) (Py_Type(_o) == &PyTerrainModProperty_Type)

PyObject * Property_asPyObject(const PropertyBase * property, LocatedEntity * owner);

PyProperty * newPyTerrainProperty();
PyProperty * newPyTerrainModProperty();

#endif // RULESETS_PY_PROPERTY_H
