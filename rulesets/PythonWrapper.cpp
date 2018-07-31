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


#include <Python.h>

#include "PythonWrapper.h"

#include "common/compose.hpp"
#include "common/log.h"

/// \brief PythonWrapper constructor
PythonWrapper::PythonWrapper(const Py::Object& wrapper)
    : m_wrapper(wrapper)
{
}

PythonWrapper::~PythonWrapper()
{
    if (m_wrapper.reference_count() != 1) {
        log(WARNING, String::compose("Deleting Python object of type '%1' with %2 > 1 refs to its wrapper/script", "FIXME: expose type in Py::Type", m_wrapper.reference_count()));
    }
}
