// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Alistair Riddoch

#include "server/ArithmeticFactory.h"

#include "rulesets/PythonArithmeticScript.h"
#include "rulesets/Character.h"

#include "common/log.h"

#include <iostream>

ArithmeticFactory::~ArithmeticFactory()
{
}

PythonArithmeticFactory::PythonArithmeticFactory(const std::string & package,
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

PythonArithmeticFactory::~PythonArithmeticFactory()
{
    if (m_module != 0) {
        Py_DECREF(m_module);
    }
    if (m_class != 0) {
        Py_DECREF(m_class);
    }
}

ArithmeticScript * PythonArithmeticFactory::newScript(Character & chr)
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
    PyObject * py_object = PyEval_CallFunction(m_class, "()");
    
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
    chr.statistics().m_script = script;

    return script;
}
