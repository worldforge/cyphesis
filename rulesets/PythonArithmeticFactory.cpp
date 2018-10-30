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


#include "python/CyPy_LocatedEntity.h"

#include "PythonArithmeticFactory.h"

#include "Python_Script_Utils.h"
#include "PythonArithmeticScript.h"

/// \brief PythonArithmeticFactory constructor
///
/// @param package Name of the package containing the script
/// @param name Name of the type within the package for the script
PythonArithmeticFactory::PythonArithmeticFactory(const std::string & package,
                                                 const std::string & name) :
                                                 PythonClass(package, name)
{
}


PythonArithmeticFactory::~PythonArithmeticFactory() = default;

int PythonArithmeticFactory::setup()
{
    return load();
}

/// \brief Create a new arithmetic script for a character
///
/// @param chr the Character that requires a new script
ArithmeticScript * PythonArithmeticFactory::newScript(LocatedEntity * owner)
{
    if (!m_class) {
        return nullptr;
    }
    // Create the task, and use its script to add a script
    if (m_class->isNull()) {
        return nullptr;
    }

    // FIXME Pass in entity for initialisation of entity pointer in
    // EntityWrapper.
    Py::Object py_object;

    try {
        if (owner == nullptr) {
            py_object = m_class->apply();
        } else {
            py_object = m_class->apply(Py::TupleN(CyPy_LocatedEntity::wrap(owner)));
        }
    } catch (const Py::BaseException& ex) {
        if (owner) {
            log(ERROR, "Could not create python stats instance for " + owner->describeEntity());
        } else {
            log(ERROR, "Could not create python stats instance");
        }
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }

    ArithmeticScript * script = new PythonArithmeticScript(py_object);
    // FIXME This is now part of the property, not the character.
    // chr.statistics().m_script = script;

    return script;
}
