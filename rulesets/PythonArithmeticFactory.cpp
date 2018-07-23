// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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
#include <rulesets/python/CyPy_LocatedEntity.h>

#include "rulesets/PythonArithmeticFactory.h"

#include "rulesets/Python_Script_Utils.h"
#include "rulesets/PythonArithmeticScript.h"
#include "rulesets/Py_Thing.h"
#include "rulesets/Character.h"

/// \brief PythonArithmeticFactory constructor
///
/// @param package Name of the package containing the script
/// @param name Name of the type within the package for the script
PythonArithmeticFactory::PythonArithmeticFactory(const std::string & package,
                                                 const std::string & name) :
                                                 PythonClass(package, name)
{
}


PythonArithmeticFactory::~PythonArithmeticFactory()
{
}

int PythonArithmeticFactory::setup()
{
    return load();
}

/// \brief Create a new arithmetic script for a character
///
/// @param chr the Character that requires a new script
ArithmeticScript * PythonArithmeticFactory::newScript(LocatedEntity * owner)
{
    // Create the task, and use its script to add a script
    if (m_class.isNull()) {
        return nullptr;
    }

    // FIXME Pass in entity for initialisation of entity pointer in
    // EntityWrapper.
    Py::Object  py_object;

    if (owner == nullptr) {
        py_object = m_class.apply();
    } else {
        py_object = m_class.apply(Py::Tuple(CyPy_LocatedEntity::wrap(owner)));
    }
    
    if (py_object.isNull()) {
        if (PyErr_Occurred() == nullptr) {
            log(ERROR, "Could not create python stats instance");
        } else {
            log(ERROR, "Reporting python error");
            PyErr_Print();
        }
    }

    ArithmeticScript * script = new PythonArithmeticScript(py_object);
    assert(script != nullptr);
    // FIXME This is now part of the property, not the character.
    // chr.statistics().m_script = script;

    return script;
}
