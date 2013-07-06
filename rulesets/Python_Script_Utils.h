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

#include <string>

class BaseMind;
class Entity;

struct _object * Get_PyClass(struct _object * module,
                       const std::string & package,
                       const std::string & type);
struct _object * Get_PyModule(const std::string & package);
struct _object * Create_PyScript(struct _object *, struct _object *);

#endif // RULESETS_PYTHON_SCRIPT_UTILS_H
