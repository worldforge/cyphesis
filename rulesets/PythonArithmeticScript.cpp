// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include <Python.h>

#include "PythonArithmeticScript.h"

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
    val = 0.5f;
    return 0;
}
