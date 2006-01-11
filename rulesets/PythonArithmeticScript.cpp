// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include <Python.h>

#include "PythonArithmeticScript.h"

#include "common/log.h"

#include <iostream>

PythonArithmeticScript::PythonArithmeticScript(PyObject * o) : m_script(o)
{
}

PythonArithmeticScript::~PythonArithmeticScript()
{
    Py_DECREF(m_script);
}

int PythonArithmeticScript::attribute(const std::string & name, float & val)
{
    std::cout << "Request for attribute " << name << " from python script"
              << std::endl << std::flush;
    PyObject * py_name = PyString_FromString(name.c_str());
    PyObject * ret = PyObject_CallMethod(m_script, "attribute", "(O)", py_name);
    Py_DECREF(py_name);
    if (ret == NULL) {
        if (PyErr_Occurred() == NULL) {
            std::cout << "No attribute method" << std::endl << std::flush;
        } else {
            log(ERROR, "Reporting python error");
            PyErr_Print();
        }
        return -1;
    }
    if (PyFloat_Check(ret)) {
        val = PyFloat_AsDouble(ret);
    } else if (PyInt_Check(ret)) {
        val = PyInt_AsLong(ret);
    } else if (ret == Py_None) {
        return -1;
    } else {
        log(ERROR, "Invalid response from script");
        return -1;
    }
    return 0;
}
