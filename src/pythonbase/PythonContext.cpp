// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include "PythonContext.h"


#include "common/log.h"
#include <Python.h>

PythonContext::PythonContext()
        : m_module("__main__"),
          m_globals(m_module.getDict()),
          m_locals()
{
}

PythonContext::~PythonContext() = default;


std::string PythonContext::runCommand(const std::string& s)
{
    Py::String filename("<string>");
    Py::Object parsed(Py_CompileStringObject(s.c_str(), *filename, Py_single_input, nullptr, 0), true);

    if (PyErr_Occurred()) {
        PyErr_Print();
        return "[parseerror]";
    }
    Py::Object ret(PyEval_EvalCode(*parsed, *m_globals, *m_locals), true);

    if (PyErr_Occurred()) {
        PyErr_Print();
        return "ERROR";
    }
    return ret.repr().as_std_string();

#if 0
    PyObject * res = PyRun_String(s.c_str(),
                                  Py_single_input,
                                  m_globals,
                                  m_locals);
    if (res == 0) {
        if (PyErr_Occurred() == 0) {
            return "WHAT";
        } else {
            PyErr_Print();
            return "ERROR";
        }
    }
    PyObject * repr = PyObject_Repr(res);
    if (repr == 0) {
        return "[undecodable]";
    }
    return PyUnicode_AsUTF8(repr);
#endif
}
