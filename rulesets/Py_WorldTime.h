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

// $Id: 07ff3587a7e63b00e60eec61dfed0c59f9eb04ab $

#ifndef RULESETS_PY_WORLDTIME_H
#define RULESETS_PY_WORLDTIME_H

#include <Python.h>

class WorldTime;

/// \brief Wrapper for WorldTime in Python
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief WorldTime object handled by this wrapper
    WorldTime * time;
    /// \brief flag to indicate if the WorldTime object is owned by the wrapper
    bool own;
} PyWorldTime;

extern PyTypeObject PyWorldTime_Type;

#define PyWorldTime_Check(_o) ((_o)->ob_type == &PyWorldTime_Type)

PyWorldTime * newPyWorldTime();

#endif // RULESETS_PY_WORLDTIME_H
