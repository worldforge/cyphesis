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

// $Id: PythonArithmeticScript.cpp,v 1.7 2006-12-24 17:18:55 alriddoch Exp $

#include <Python.h>

#include "PythonArithmeticScript.h"

#include "common/log.h"

#include <iostream>

/// \brief PythonArithmeticScript
///
/// @param script Python instance object implementing the script
PythonArithmeticScript::PythonArithmeticScript(PyObject * script) :
                                               m_script(script)
{
}

PythonArithmeticScript::~PythonArithmeticScript()
{
    Py_DECREF(m_script);
}

int PythonArithmeticScript::attribute(const std::string & name, float & val)
{
    PyObject * py_name = PyString_FromString(name.c_str());
    PyObject * ret = PyObject_CallMethod(m_script, "attribute", "(O)", py_name);
    Py_DECREF(py_name);
    if (ret == NULL) {
        if (PyErr_Occurred() == NULL) {
            // std::cout << "No attribute method" << std::endl << std::flush;
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
