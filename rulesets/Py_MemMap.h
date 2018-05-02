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


#ifndef RULESETS_PY_MEMMAP_H
#define RULESETS_PY_MEMMAP_H

#include <Python.h>

class MemMap;

/// \brief Wrapper for MemMap in Python
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief MemMap object handled by this wrapper
    MemMap * m_map;
} PyMemMap;

extern PyTypeObject PyMemMap_Type;

#define PyMemMap_Check(_o) (Py_TYPE((_o)) == &PyMemMap_Type)

PyMemMap * newPyMemMap();

#endif // RULESETS_PY_MEMMAP_H
