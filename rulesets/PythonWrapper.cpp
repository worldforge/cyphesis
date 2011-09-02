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

// $Id$

#include <Python.h>

#include "PythonWrapper.h"

#include "common/compose.hpp"
#include "common/log.h"

/// \brief PythonWrapper constructor
PythonWrapper::PythonWrapper(PyObject * wrapper) : m_wrapper(wrapper)
{
    Py_INCREF(m_wrapper);
}

PythonWrapper::~PythonWrapper()
{
    if (m_wrapper->ob_refcnt != 1) {
        log(WARNING, String::compose("Deleting entity with %1 > 1 refs to it's wrapper/script", m_wrapper->ob_refcnt));
    }
    Py_DECREF(m_wrapper);
}
