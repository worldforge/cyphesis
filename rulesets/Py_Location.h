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

// $Id: Py_Location.h,v 1.15 2006-12-24 17:18:55 alriddoch Exp $

#ifndef RULESETS_PY_LOCATION_H
#define RULESETS_PY_LOCATION_H

#include <Python.h>

class Location;
class Entity;

/// \brief Wrapper for Location in Python
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief Location object handled by this wrapper
    Location * location;
    /// \brief Entiy object that the Location object is a member of
    Entity * owner;
} PyLocation;

extern PyTypeObject PyLocation_Type;

#define PyLocation_Check(_o) ((_o)->ob_type == &PyLocation_Type)

PyLocation * newPyLocation();

#endif // RULESETS_PY_LOCATION_H
