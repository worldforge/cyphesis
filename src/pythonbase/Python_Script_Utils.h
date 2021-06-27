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


#ifndef RULESETS_PYTHON_SCRIPT_UTILS_H
#define RULESETS_PYTHON_SCRIPT_UTILS_H

#include "pycxx/CXX/Objects.hxx"
#include <string>

class Entity;

Py::Object Get_PyClass(const Py::Module& module,
                       const std::string& package,
                       const std::string& type);

Py::Module Get_PyModule(const std::string& package);

Py::Object Create_PyScript(const Py::Object& wrapper, const Py::Callable& py_class);

#endif // RULESETS_PYTHON_SCRIPT_UTILS_H
