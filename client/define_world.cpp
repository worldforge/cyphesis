// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "define_world.h"
#include "Py_CreatorClient.h"

#include <iostream>

namespace DefineWorld {

bool define(CreatorClient * character)
{
    PyObject * package_name = PyString_FromString("define_world");
    PyObject * mod_dict = PyImport_Import(package_name);
    Py_DECREF(package_name);
    if (mod_dict == NULL) {
        std::cerr << "Cld not find python module define_world"
                  << std::endl << std::flush;
        PyErr_Print();
        return false;
    }
    PyObject * function = PyObject_GetAttrString(mod_dict, "default");
    Py_DECREF(mod_dict);
    if (function == NULL) {
        std::cerr << "Could not find default function" << std::endl
                  << std::flush;
        PyErr_Print();
        return false;
    }
    if (PyCallable_Check(function) == 0) {
        std::cerr << "It does not seem to be a function at all" << std::endl
                  << std::flush;
        Py_DECREF(function);
        return false;
    }
    CreatorClientObject * editor = newCreatorClientObject(NULL);
    editor->m_mind = character;
    PyObject * pyob = PyEval_CallFunction(function, "(O)", editor);

    if (pyob == NULL) {
        if (PyErr_Occurred() == NULL) {
            std::cerr << "Could not call function" << std::endl << std::flush;
        } else {
            std::cerr << "Reporting python error" << std::endl << std::flush;
            PyErr_Print();
        }
    }
    Py_DECREF(function);
    return true;

}

}
