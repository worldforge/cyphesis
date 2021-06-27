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

#include "PythonScriptFactory_impl.h"

#include "pythonbase/Python_Script_Utils.h"

#include "rules/ai/BaseMind.h"
#include "rules/simulation/Task.h"

template<>
PythonScriptFactory<LocatedEntity>::PythonScriptFactory(const std::string & p,
                                                        const std::string & t) :
                                                        PythonClass(p, t)
{
}



template class PythonScriptFactory<LocatedEntity>;

