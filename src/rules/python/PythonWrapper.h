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

#include "rules/Script.h"
#include "pycxx/CXX/Objects.hxx"
#include <sigc++/connection.h>

/// \brief A Python script wrapping a C++ class.
/// \ingroup Scripts
class PythonWrapper : public Script
{
    protected:
        /// \brief Python object that wraps the entity.
        Py::Object m_wrapper;
        std::vector<sigc::connection> m_propertyUpdateConnections;
    public:
        explicit PythonWrapper(const Py::Object& wrapper);

        ~PythonWrapper() override;

        HandlerResult operation(const std::string& opname,
                                const Atlas::Objects::Operation::RootOperation& op,
                                OpVector& res) override;

        void hook(const std::string& function, LocatedEntity* entity, OpVector& res) override;

        void attachPropertyCallbacks(LocatedEntity& entity) override;

        static HandlerResult processScriptResult(const std::string& scriptName, const Py::Object& ret, OpVector& res);


        /// \brief Accessor for the python object that wraps the entity.
        const Py::Object& wrapper() const
        { return m_wrapper; }
};

template<class T>
Py::Object wrapPython(T* entity);

#endif // RULESETS_PYTHON_WRAPPER_H
