// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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


#ifndef RULESETS_PYTHON_CLASS_H
#define RULESETS_PYTHON_CLASS_H

#include <string>
#include <boost/optional.hpp>
#include "pycxx/CXX/Objects.hxx"

/// \brief Factory interface for creating scripts to attach to in game
/// entity objects.
class PythonClass
{
    protected:
        /// \brief Name of the script package where the script type is
        std::string m_package;
        /// \brief Name of the script type instanced by this factory
        std::string m_type;
        /// \brief Base class object this should inherit from
        //struct _typeobject * m_base;

        /// \brief Module object which contains the script class
        Py::Module m_module;
        /// \brief Class object to be instanced when creating scripts
        boost::optional<Py::Callable> m_class;

        PythonClass(const std::string& package,
                    const std::string& type);

        int getClass(const Py::Module& module);

        int load();

        int refresh();

    public:
        ~PythonClass();
};

#endif // RULESETS_PYTHON_CLASS_H
