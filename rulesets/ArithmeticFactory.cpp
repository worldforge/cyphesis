// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

#include "rulesets/ArithmeticFactory.h"

#include "rulesets/Python_Script_Utils.h"
#include "rulesets/PythonArithmeticScript.h"
#include "rulesets/Py_Thing.h"
#include "rulesets/Character.h"

#include "common/log.h"
#include "common/compose.hpp"

#include <iostream>

ArithmeticFactory::~ArithmeticFactory()
{
}

/// \brief PythonArithmeticFactory constructor
///
/// @param package Name of the package containing the script
/// @param name Name of the type within the package for the script
PythonArithmeticFactory::PythonArithmeticFactory(const std::string & package,
                                                 const std::string & name) :
                                                 m_module(0), m_class(0),
                                                 m_package(package),
                                                 m_type(name)
{
    // Import the module
    m_module = Get_PyModule(m_package);
    if (m_module == NULL) {
        return;
    }

    // Get a reference to the class
    m_class = Get_PyClass(m_module, m_package, m_type);
}

PythonArithmeticFactory::~PythonArithmeticFactory()
{
    if (m_module != 0) {
        Py_DECREF(m_module);
    }
    if (m_class != 0) {
        Py_DECREF(m_class);
    }
}

/// \brief Create a new arithmetic script for a character
///
/// @param chr the Character that requires a new script
ArithmeticScript * PythonArithmeticFactory::newScript(Entity * owner)
{
    // Create the task, and use its script to add a script
    if (m_class == 0) {
        std::cout << "No class" << std::endl << std::flush;
        return 0;
    }

#if 0
    PyTask * wrapper = newPyTask();
    assert(wrapper != 0);
    wrapper->m_task = task;
    assert(wrapper->m_task != 0);
    PyObject * script = Create_PyScript((PyObject *)wrapper, m_class);
    assert(script != 0);

    task->setScript(new PythonEntityScript(script, (PyObject *)wrapper));
#endif

    // FIXME Pass in entity for initialisation of entity pointer in
    // EntityWrapper.
    PyObject * py_object;

    if (owner == 0) {
        py_object = PyEval_CallFunction(m_class, "()");
    } else {
        PyObject * entity = wrapEntity(owner);
        if (entity == 0) {
            log(ERROR, "Could not wrap character when installing statistics");
            return 0;
        }
        py_object = PyEval_CallFunction(m_class, "(O)", entity);
    }
    
    if (py_object == 0) {
        if (PyErr_Occurred() == NULL) {
            log(ERROR, "Could not create python stats instance");
        } else {
            log(ERROR, "Reporting python error");
            PyErr_Print();
        }
    }

    ArithmeticScript * script = new PythonArithmeticScript(py_object);
    assert(script != 0);
    // FIXME This is now part of the property, not the character.
    // chr.statistics().m_script = script;

    return script;
}
