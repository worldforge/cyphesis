// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "define_world.h"

#include <Python.h>

#include <iostream>

class BaseEntity;

namespace DefineWorld {

void init_python_api();

bool define(BaseEntity * character)
{
    PyObject * package_name = PyString_FromString("define_world");
    PyObject * mod_dict = PyImport_Import(package_name);
    Py_DECREF(package_name);
    if (mod_dict == NULL) {
        cerr << "Cld not find python module define_world"
             << endl << flush;
        PyErr_Print();
        return false;
    }
    PyObject * function = PyObject_GetAttrString(mod_dict, "default");
    Py_DECREF(mod_dict);
    if (function == NULL) {
        cerr << "Could not find default function" << endl << flush;
        PyErr_Print();
        return false;
    }
    if (PyCallable_Check(function) == 0) {
        cerr << "It does not seem to be a function at all" << endl << flush;
        Py_DECREF(function);
        return false;
    }
    PyObject * pyob = PyEval_CallFunction(function, "()");

    if (pyob == NULL) {
        if (PyErr_Occurred() == NULL) {
            cerr << "Could not call function" << endl << flush;
        } else {
            cerr << "Reporting python error" << endl << flush;
            PyErr_Print();
        }
    }
    Py_DECREF(function);
    return true;

}

}
