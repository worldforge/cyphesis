// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
//               2001-2011 Python Software Foundation
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

// Portions of this file have been copied from Python 2.6.7 and so this file
// is a derivative work. The derivative is distributed under the GPL as
// permitted by the license used by Python Software Foundation.


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "pycxx/CXX/Objects.hxx"


#include "python_testers.h"

#include <iostream>

#include <Python.h>

// This function is expanded out from the Python library function
// PyRun_SimpleStringFlags(). We do this so we can tell the difference,
// and fail in the case of a parse error in the code text. The unit tests
// do contain code which should fail, but never any code that fails because
// it won't parse.
int CyPyRun_SimpleString(const char* command, PyObject* exception)
{
    Py::Module m("__main__");
    if (m.isNull()) {
        return -1;
    }
    Py::Dict d = m.getDict();

    Py::String filename("<string>");
    Py::Object parsed(Py_CompileStringObject(command, *filename, Py_file_input, nullptr, 0), true);

    if (PyErr_Occurred()) {
        PyErr_Print();
        return -2;
    }
    Py::Object ret(PyEval_EvalCode(*parsed, *d, *d), true);

    if (ret.isNull()) {
        int errcode = -1;
        if (exception != nullptr) {
            if (PyErr_ExceptionMatches(exception)) {
                errcode = -3;
            }
        }
        PyErr_Print();
        return errcode;
    }

    PyObject* f = PySys_GetObject("stdout");
    if (PyFile_WriteString((std::string("Exec: ") + command + "\n").c_str(), f)) {
        PyErr_Clear();
    }
    return 0;
}
