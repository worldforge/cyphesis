// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#ifndef RULES_PYTHON_CONTEXT_H
#define RULES_PYTHON_CONTEXT_H

#include "pycxx/CXX/Objects.hxx"
#include <string>

/// \brief Handle running python commands in the servers interpreter context
class PythonContext
{
    protected:
        Py::Module m_module;
        Py::Dict m_globals;
        Py::Dict m_locals;
    public:
        PythonContext();

        ~PythonContext();

        std::string runCommand(const std::string&);
};

#endif // RULES_PYTHON_CONTEXT_H
