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



#include "PythonClass.h"

#include "Python_Script_Utils.h"

#include "common/log.h"
#include "common/compose.hpp"
#include <Python.h>

/// \brief ScriptKit constructor
/// 
/// @param package name of the script package scripts are to be created from
/// @param type name of the type instanced to create scripts
PythonClass::PythonClass(const std::string& package,
                         const std::string& type) :
        m_package(package),
        m_type(type),
        m_module(nullptr)
{
}

PythonClass::~PythonClass() = default;

int PythonClass::load()
{
    auto module = Get_PyModule(m_package);
    if (module.isNull()) {
        return -1;
    }
    m_module = module;
    return getClass(m_module);
}

/// \brief Retrieve the pythonclass object from the module which has
/// already been loaded.
int PythonClass::getClass(const Py::Module& module)
{
    auto new_class = Get_PyClass(module, m_package, m_type);
    if (new_class.isNull()) {
        // If this is a new class, leave it as zero and fail and this
        // should be discarded by <*>RuleHandler
        // If this is an existing class, leave it as the old value, but
        // fail the update. This should signal back to the client.
        return -1;
    }
    if (!new_class.isCallable()) {
        log(ERROR, String::compose("Python class \"%1.%2\" is not callable.",
                                   m_package, m_type));
        return -1;
    }
//    if (!PyType_IsSubtype((PyTypeObject*)new_class.type().ptr, m_base)) {
//        log(ERROR, String::compose("Python class \"%1.%2\" does not inherit "
//                                   "from a core server type \"%3\".",
//                                   m_package, m_type, m_base->tp_name));
//        return -1;
//    }
    m_class = Py::Callable(new_class);

    return 0;
}

int PythonClass::refresh()
{
    log(NOTICE, String::compose("Refreshing Python module \"%1.%2\".", m_package, m_type));
    if (m_module.isNull()) {
        log(ERROR, String::compose("Abort refresh of non loaded module \"%1.%2\".", m_package, m_type));
        return -1;
    }
    Py::Module new_module(PyImport_ReloadModule(m_module.ptr()));
    if (new_module.isNull()) {
        log(ERROR, String::compose("Error reloading python module \"%1\"",
                                   m_package));
        PyErr_Clear();
        return -1;
    }
    int ret = getClass(new_module);
    if (ret != 0) {
        log(ERROR, String::compose(R"(Error finding python class "%1" in "%2")",
                                   m_type, m_package));
        return -1;
        // After reloading, but failing to get the class, I think the old class
        // should still work, but is no longer bound to the name. It won't
        // be collected at least until we release the reference we hold
        // in m_class
    }
    if (m_module != new_module) {
        log(WARNING, String::compose("Python module \"%1.%2\" has changed "
                                     "its pointer on reload",
                                     m_package, m_type));
        m_module = new_module;
    }
    return 0;
}
