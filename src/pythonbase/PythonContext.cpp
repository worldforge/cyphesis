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
#include <Python-ast.h>

PythonContext::PythonContext()
        : m_module("__main__"),
          m_globals(m_module.getDict()),
          m_locals(),
          m_arena(PyArena_New())
{
}

PythonContext::~PythonContext()
{
    PyArena_Free(m_arena);
}

static Py::Object
run_mod(mod_ty mod, const char* filename, const Py::Object& globals, const Py::Object& locals,
        PyCompilerFlags* flags, PyArena* arena)
{
    Py::Object co((PyObject*) PyAST_Compile(mod, filename, flags, arena), true);
    if (co.isNull()) {
        return Py::None();
    }

    return Py::Object(PyEval_EvalCode(*co, *globals, *locals));
}

std::string PythonContext::runCommand(const std::string& s)
{
    // This is expanded from PyRun_SimpleString in the Python library
    // so that we can report errors better at the parsing stage
    mod_ty mod;

    mod = PyParser_ASTFromString(s.c_str(),
                                 "<string>",
                                 Py_single_input,
                                 nullptr,
                                 m_arena);
    if (mod == nullptr) {
        PyErr_Print();
        return "[parseerror]";
    }
    auto ret = run_mod(mod, "<string>", m_globals, m_locals, nullptr, m_arena);
    if (ret.isNull()) {
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
