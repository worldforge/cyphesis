// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "Python_ClientAPI.h"

#include "Py_CreatorClient.h"
#include "Py_ObserverClient.h"

#include "rulesets/Python_Script_Utils.h"

#include "common/log.h"

#include <iostream>

int runClientScript(CreatorClient * c, const std::string & package,
                                       const std::string & func)
{
    PyObject * module = Get_PyModule(package);
    if (module == NULL) {
        return -1;
    }
    PyObject * function = PyObject_GetAttrString(module,
                                                 (char *)func.c_str());
    Py_DECREF(module);
    if (function == NULL) {
        std::cerr << "Could not find " << func << " function" << std::endl
                  << std::flush;
        PyErr_Print();
        return -1;
    }
    if (PyCallable_Check(function) == 0) {
        std::cerr << "It does not seem to be a function at all" << std::endl
                  << std::flush;
        Py_DECREF(function);
        return -1;
    }
    PyCreatorClient * editor = newPyCreatorClient();
    if (editor == NULL) {
        Py_DECREF(function);
        return -1;
    }
    editor->m_mind = c;
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
    return 0;

}

void extend_client_python_api()
{
    PyObject * server = Get_PyModule("server");
    if (server == 0) {
        return;
    }

    PyCreatorClient_Type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyCreatorClient_Type) < 0) {
        log(CRITICAL, "Python init failed to ready CreatorClient wrapper type");
        return;
    }
    PyModule_AddObject(server, "CreatorClient", (PyObject *)&PyCreatorClient_Type);

    PyObserverClient_Type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyObserverClient_Type) < 0) {
        log(CRITICAL, "Python init failed to ready ObserverClient wrapper type");
        return;
    }
    PyModule_AddObject(server, "ObserverClient", (PyObject *)&PyObserverClient_Type);
}

void python_prompt()
{
    char * argv[1] = { "python" };

    Py_Main(1, &argv[0]);
}
