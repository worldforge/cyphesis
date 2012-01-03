// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>
#include <Python-ast.h>

#include "python_testers.h"

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

// This function is expanded out from the Python library function
// PyRun_SimpleStringFlags(). We do this so we can tell the difference,
// and fail in the case of a parse error in the code text. The unit tests
// do contain code which should fail, but never any code that fails because
// it won't parse.
int CyPyRun_SimpleString(const char * command)
{
    PyCompilerFlags *flags = NULL;
    PyObject * m = PyImport_AddModule("__main__");
    if (m == NULL)
        return -1;
    PyObject * d = PyModule_GetDict(m);
    // v = PyRun_StringFlags(command, Py_file_input, d, d, flags);
    PyArena *arena = PyArena_New();
    if (arena == NULL)
        return NULL;

    mod_ty mod = PyParser_ASTFromString(command, "<string>", Py_file_input, flags, arena);
    if (mod == NULL) {
        PyArena_Free(arena);
        PyErr_Print();
        return -2;
    }

    PyObject *ret = run_mod(mod, "<string>", d, d, flags, arena);
    PyArena_Free(arena);

    if (ret == NULL) {
        PyErr_Print();
        return -1;
    }
    Py_DECREF(ret);
    if (Py_FlushLine())
        PyErr_Clear();
    return 0;
}
