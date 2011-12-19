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

// $Id$

#include <Python.h>

#include "PythonClass.h"

#include "rulesets/Python_Script_Utils.h"

#include "common/log.h"
#include "common/compose.hpp"

/// \brief ScriptKit constructor
/// 
/// @param package name of the script package scripts are to be created from
/// @param type name of the type instanced to create scripts
PythonClass::PythonClass(const std::string & package,
                         const std::string & type,
                         PyTypeObject * base) : m_package(package),
                                                m_type(type),
                                                m_base(base),
                                                m_module(0),
                                                m_class(0)
{
}

PythonClass::~PythonClass()
{
    if (m_class != 0) {
        Py_DECREF(m_class);
    }
    if (m_module != 0) {
        Py_DECREF(m_module);
    }
}

int PythonClass::load()
{
    m_module = Get_PyModule(m_package);
    if (m_module == NULL) {
        return -1;
    }
    return getClass(m_module);
}

/// \brief Retrieve the pythonclass object from the module which has
/// already been loaded.
int PythonClass::getClass(PyObject * module)
{
    PyObject * new_class = Get_PyClass(module, m_package, m_type);
    if (new_class == 0) {
        // If this is a new class, leave it as zero and fail and this
        // should be discarded by <*>RuleHandler
        // If this is an existing class, leave it as the old value, but
        // fail the update. This should signal back to the client.
        return -1;
    }
    if (!PyType_IsSubtype((PyTypeObject*)new_class, m_base)) {
        log(ERROR, String::compose("Python class \"%1.%2\" does not inherit "
                                   "from a core server type \"%3\".",
                                   m_package, m_type, m_base->tp_name));
        Py_DECREF(new_class);
        return -1;
    }
    if (m_class != 0) {
        Py_DECREF(m_class);
    }
    m_class = new_class;

    return 0;
}

int PythonClass::refresh()
{
    if (m_module == 0) {
        log(ERROR, "bort");
        return -1;
    }
    PyObject * new_module = PyImport_ReloadModule(m_module);
    if (new_module == 0) {
        log(ERROR, String::compose("Error reloading python module \"%1\"",
                                   m_package));
        PyErr_Clear();
        return -1;
    }
    int ret = getClass(new_module);
    if (ret != 0) {
        log(ERROR, String::compose("Error finding python class \"%1\" in \"%2\"",
                                   m_type, m_package));
        return -1;
        // After reloading, but failing to get the class, I think the old class
        // should still work, but is no longer bound to the name. It won't
        // be collected at least until we release the reference we hold
        // in m_class
    }
    // We decref even though this is probably an identical pointer to the
    // module because the PyImport_ call returned a new reference
    Py_DECREF(m_module);
    if (m_module != new_module) {
        log(WARNING, String::compose("Python module \"%1.%2\" has changed "
                                     "its pointer on reload",
                                     m_package, m_type));
        m_module = new_module;
    }
    return 0;
}
