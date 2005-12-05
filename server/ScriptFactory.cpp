// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "ScriptFactory.h"

#include "rulesets/Py_Thing.h"
#include "rulesets/Python_Script_Utils.h"
#include "rulesets/PythonThingScript.h"

#include "rulesets/Entity.h"

#include "common/log.h"

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
    PyEntity * pyEntity = newPyEntity();
    pyEntity->m_entity = entity;
    Subscribe_Script(entity, m_class, m_package);
    PyObject * script = Create_PyScript((PyObject *)pyEntity, m_class);

    if (script == NULL) {
        return -1;
    }

    entity->setScript(new PythonEntityScript(script));

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
