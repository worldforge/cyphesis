// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "server/TaskFactory.h"

#include "server/ScriptFactory.h"

#include "rulesets/Py_Task.h"
#include "rulesets/Python_Script_Utils.h"
#include "rulesets/PythonThingScript.h"
#include "rulesets/TaskScript.h"
#include "rulesets/Character.h"

#include "common/log.h"

TaskFactory::~TaskFactory()
{
}

PythonTaskScriptFactory::PythonTaskScriptFactory(const std::string & package,
                                                 const std::string & name) :
                                                 m_module(0), m_class(0),
                                                 m_package(package),
                                                 m_name(name)
{
    // Import the module
    PyObject * package_name = PyString_FromString((char *)m_package.c_str());
    m_module = PyImport_Import(package_name);
    Py_DECREF(package_name);
    if (m_module == NULL) {
        std::string msg = std::string("Missing python module ") + m_package;
        log(ERROR, msg.c_str());
        PyErr_Print();
        return;
    }

    // Get a reference to the class
    std::string classname(m_name);
    classname[0] = toupper(classname[0]);
    m_class = PyObject_GetAttrString(m_module, (char *)classname.c_str());
    if (m_class == NULL) {
        std::string msg = std::string("Could not find python class ")
                        + m_package + "." + classname;
        log(ERROR, msg.c_str());
        PyErr_Print();
        return;
    }
    if (PyCallable_Check(m_class) == 0) {
        std::string msg = std::string("Could not instance python class ")
                        + m_package + "." + classname;
        log(ERROR, msg.c_str());
        Py_DECREF(m_class);
        m_class = 0;
        return;
    }
    return;
}

PythonTaskScriptFactory::~PythonTaskScriptFactory()
{
    if (m_module != 0) {
        Py_DECREF(m_module);
    }
    if (m_class != 0) {
        Py_DECREF(m_class);
    }
}

Task * PythonTaskScriptFactory::newTask(Character & chr)
{
    // Create the task, and use its script to add a script
    if (m_class == 0) {
        std::cout << "No class" << std::endl << std::flush;
        return 0;
    }
    TaskScript * task = new TaskScript(chr);
    assert(task != 0);

    PyTask * pyTask = newPyTask();
    assert(pyTask != 0);
    pyTask->m_task = task;
    assert(pyTask->m_task != 0);
    PyObject * script = Create_PyScript((PyObject *)pyTask, m_class);
    assert(script != 0);

    task->setScript(new PythonEntityScript(script));
    
    std::cout << "created" << std::endl << std::flush;

    return task;
}
