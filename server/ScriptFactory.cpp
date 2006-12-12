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

// $Id: ScriptFactory.cpp,v 1.7 2006-12-12 15:54:24 alriddoch Exp $

#include "ScriptFactory.h"

#include "rulesets/Py_Thing.h"
#include "rulesets/Python_Script_Utils.h"
#include "rulesets/PythonThingScript.h"

#include "rulesets/Entity.h"

#include "common/log.h"

/// \brief ScriptFactory constructor
/// 
/// @param package name of the script package scripts are to be created from
/// @param type name of the type instanced to create scripts
ScriptFactory::ScriptFactory(const std::string & package,
                             const std::string & type) : m_package(package),
                                                         m_type(type)
{
}

ScriptFactory::~ScriptFactory()
{
}

int PythonScriptFactory::getClass()
{
    std::string classname(m_type);
    classname[0] = toupper(classname[0]);
    m_class = PyObject_GetAttrString(m_module, (char *)classname.c_str());
    if (m_class == NULL) {
        std::string msg = std::string("Could not find python class ")
                        + m_package + "." + classname;
        log(ERROR, msg.c_str());
        PyErr_Print();
        return -1;
    }
    if (PyCallable_Check(m_class) == 0) {
        std::string msg = std::string("Could not instance python class ")
                        + m_package + "." + classname;
        log(ERROR, msg.c_str());
        Py_DECREF(m_class);
        m_class = 0;
        return -1;
    }
    return 0;
}

PythonScriptFactory::PythonScriptFactory(const std::string & package,
                                         const std::string & type) :
                                         ScriptFactory(package, type),
                                         m_module(0), m_class(0)
{
    PyObject * package_name = PyString_FromString((char *)m_package.c_str());
    m_module = PyImport_Import(package_name);
    Py_DECREF(package_name);
    if (m_module == NULL) {
        std::string msg = std::string("Missing python module ") + m_package;
        log(ERROR, msg.c_str());
        PyErr_Print();
        return;
    }
    getClass();
}

PythonScriptFactory::~PythonScriptFactory()
{
    if (m_class != 0) {
        Py_DECREF(m_class);
    }
    if (m_module != 0) {
        Py_DECREF(m_module);
    }
}

int PythonScriptFactory::addScript(Entity * entity)
{
    if (m_class == 0) {
        return -1;
    }
    PyEntity * wrapper = newPyEntity();
    wrapper->m_entity = entity;
    Subscribe_Script(entity, m_class, m_package);
    PyObject * script = Create_PyScript((PyObject *)wrapper, m_class);

    if (script == NULL) {
        return -1;
    }

    entity->setScript(new PythonEntityScript(script, (PyObject *)wrapper));

    return 0;
}

int PythonScriptFactory::refreshClass()
{
    if (m_module == 0) {
        return -1;
    }
    PyObject * new_module = PyImport_ReloadModule(m_module);
    if (new_module == 0) {
        std::string msg = std::string("Error reloading python module ")
                                     + m_package;
        log(ERROR, msg.c_str());
        PyErr_Clear();
        return -1;
    }
    Py_DECREF(m_module);
    m_module = new_module;
    return getClass();
}
