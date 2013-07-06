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


#include <Python.h>
#include <Python-ast.h>

#include "PythonContext.h"

#include "common/log.h"

PythonContext::PythonContext()
{
    PyObject * m = PyImport_AddModule("__main__");
    if (m == NULL) {
        log(ERROR, "Could not import __main__");
        return;
    }
    m_globals = PyModule_GetDict(m);
    m_locals = PyDict_New();
    m_arena = PyArena_New();
}

static PyObject *
run_mod(mod_ty mod, const char *filename, PyObject *globals, PyObject *locals,
         PyCompilerFlags *flags, PyArena *arena)
{
    PyCodeObject *co;
    PyObject *v;
    co = PyAST_Compile(mod, filename, flags, arena);
    if (co == NULL)
        return NULL;
    v = PyEval_EvalCode(co, globals, locals);
    Py_DECREF(co);
    return v;
}

std::string PythonContext::runCommand(const std::string & s)
{
    // This is expanded from PyRun_SimpleString in the Python library
    // so that we can report errors better at the parsing stage
    PyObject *ret = NULL;
    mod_ty mod;

    mod = PyParser_ASTFromString(s.c_str(),
                                 "<string>",
                                 Py_single_input,
                                 NULL,
                                 m_arena);
    if (mod == NULL) {
        PyErr_Print();
        return "[parseerror]";
    }
    ret = run_mod(mod, "<string>", m_globals, m_locals, NULL, m_arena);
    if (ret == NULL) {
        PyErr_Print();
        return "ERROR";
    }
    PyObject * repr = PyObject_Repr(ret);
    if (repr == 0) {
        return "[undecodable]";
    }
    return PyString_AsString(repr);

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
    return PyString_AsString(repr);
#endif
}
