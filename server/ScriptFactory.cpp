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

#include "ScriptFactory_impl.h"

#include "rulesets/Py_Thing.h"
#include "rulesets/Py_Task.h"
#include "rulesets/Python_Script_Utils.h"
#include "rulesets/PythonEntityScript.h"

#include "rulesets/Entity.h"
#include "rulesets/Task.h"

template<>
int PythonScriptFactory<Entity>::check() const
{
    if (!PyType_IsSubtype((PyTypeObject*)m_class, &PyEntity_Type) &&
-       !PyType_IsSubtype((PyTypeObject*)m_class, &PyCharacter_Type)) {
        return -1;
    }
    return 0;
}

template<>
int PythonScriptFactory<Task>::check() const
{
    if (!PyType_IsSubtype((PyTypeObject*)m_class, &PyTask_Type)) {
        return -1;
    }
    return 0;
}

template class PythonScriptFactory<Entity>;
template class PythonScriptFactory<Task>;
