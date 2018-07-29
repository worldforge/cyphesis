// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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


#ifndef RULESETS_PYTHON_WRAPPER_H
#define RULESETS_PYTHON_WRAPPER_H

#include "Script.h"
#include "external/pycxx/CXX/Objects.hxx"

/// \brief Wrapper class for entities without scripts but with wrappers
/// \ingroup Scripts
class PythonWrapper : public Script {
  protected:
    /// \brief Python object that wraps the entity.
    Py::Object m_wrapper;
  public:
    explicit PythonWrapper(Py::Object wrapper);
    ~PythonWrapper() override;

    /// \brief Accessor for the python object that wraps the entity.
    const Py::Object& wrapper() const { return m_wrapper; }
};

template<class T>
struct Py::Object wrapPython(T * entity);

#endif // RULESETS_PYTHON_WRAPPER_H
