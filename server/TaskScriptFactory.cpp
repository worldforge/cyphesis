
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

#include "TaskScriptFactory.h"

#include "rulesets/Py_Task.h"
#include "rulesets/Python_Script_Utils.h"
#include "rulesets/PythonEntityScript.h"

#include "rulesets/TaskScript.h"

#include "common/log.h"
#include "common/compose.hpp"

/// \brief TaskScriptKit constructor
/// 
/// @param package name of the script package scripts are to be created from
/// @param type name of the type instanced to create scripts
TaskScriptKit::TaskScriptKit(const std::string & package,
                     const std::string & type) : m_package(package),
                                                 m_type(type)
{
}

TaskScriptKit::~TaskScriptKit()
{
}

/// \brief Retrieve the pythonclass object from the module which has
/// already been loaded.
int PythonTaskScriptFactory::getClass()
{
    m_class = Get_PyClass(m_module, m_package, m_type);
    if (m_class == 0) {
        return -1;
    }
    if (!PyType_IsSubtype((PyTypeObject*)m_class, &PyTask_Type)) {
        log(ERROR, String::compose("Python class does not inherit from "
                                   "a core server type. \"%1.%2\"",
                                   m_package, m_type));
        Py_DECREF(m_class);
        m_class = 0;
        return -1;
    }

    return 0;
}

/// \brief PythonTaskScriptFactory constructor
///
/// @param package Name of the script package where the script type is
/// @param type Name of the scrpt types instanced by this factory
PythonTaskScriptFactory::PythonTaskScriptFactory(const std::string & package,
                                                 const std::string & type) :
                                                 TaskScriptKit(package, type),
                                                 m_module(0), m_class(0)
{
    // FIXME #4 This sort of code should not be in the constructor
    m_module = Get_PyModule(m_package);
    if (m_module == NULL) {
        return;
    }
    getClass();
}

PythonTaskScriptFactory::~PythonTaskScriptFactory()
{
    if (m_class != 0) {
        Py_DECREF(m_class);
    }
    if (m_module != 0) {
        Py_DECREF(m_module);
    }
}

int PythonTaskScriptFactory::addScript(TaskScript * entity)
{
    if (m_class == 0) {
        return -1;
    }
    PyTask * wrapper = newPyTask();
    wrapper->m_task = entity;

    PyObject * script = Create_PyScript((PyObject *)wrapper, m_class);

    Py_DECREF(wrapper);

    if (script != NULL) {
        entity->setScript(new PythonEntityScript(script));

        Py_DECREF(script);
    }

    return (script == NULL) ? -1 : 0;
}

int PythonTaskScriptFactory::refreshClass()
{
    if (m_module == 0) {
        return -1;
    }
    PyObject * new_module = PyImport_ReloadModule(m_module);
    if (new_module == 0) {
        log(ERROR, String::compose("Error reloading python module \"%1\"",
                                   m_package));
        PyErr_Clear();
        return -1;
    }
    Py_DECREF(m_module);
    m_module = new_module;
    return getClass();
}
