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

// $Id: Py_Mind.h,v 1.10 2006-12-24 17:18:55 alriddoch Exp $

#ifndef RULESETS_PY_MIND_H
#define RULESETS_PY_MIND_H

#include <Python.h>

class BaseMind;

/// \brief Wrapper for BaseMind in Python
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief Storage for additional attributes
    PyObject * Mind_attr;    // Attributes dictionary
    /// \brief BaseMind object handled by this wrapper
    BaseMind * m_mind;
} PyMind;

extern PyTypeObject PyMind_Type;

#define PyMind_Check(_o) ((_o)->ob_type == &PyMind_Type)

PyMind * newPyMind();

#endif // RULESETS_PY_MIND_H
