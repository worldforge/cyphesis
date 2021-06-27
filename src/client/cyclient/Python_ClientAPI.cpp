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


#include <Python.h>

#include "Python_ClientAPI.h"

#include "pythonbase/Python_Script_Utils.h"

#include "common/log.h"
#include "CyPy_CreatorClient.h"
#include "CyPy_ObserverClient.h"

#include <iostream>
#include <string>
#include "pycxx/CXX/Objects.hxx"

int python_client_script(const std::string& package,
                         const std::string& func,
                         ObserverClient& client)
{
    auto module = Get_PyModule(package);
    if (module.isNull()) {
        return -1;
    }

    auto function = module.getAttr(func);
    if (function.isNull()) {
        std::cerr << "Could not find " << func << " function" << std::endl
                  << std::flush;
        PyErr_Print();
        return -1;
    }
    if (!function.isCallable()) {
        std::cerr << "It does not seem to be a function at all" << std::endl
                  << std::flush;
        return -1;
    }
    Py::Callable callable(function);
    Py::Dict kwds;
    Py::TupleN args(CyPy_ObserverClient::wrap(& client));
    try {
        auto ret = callable.apply(args, kwds);
    } catch (...) {
        if (PyErr_Occurred() == nullptr) {
            log(ERROR, "Could not call function");
        } else {
            log(ERROR, "Reporting python error");
            PyErr_Print();
        }
        return -1;
    }

    return 0;

}

void extend_client_python_api()
{
    Py::Module server("server");
    if (server.isNull()) {
        return;
    }
    CyPy_CreatorClient::init_type();
    CyPy_ObserverClient::init_type();

    server.setAttr("CreatorClient", CyPy_CreatorClient::type());
    server.setAttr("ObserverClient", CyPy_ObserverClient::type());

}

void python_prompt()
{

    std::wstring prgname = L"python";

    auto bytes = (prgname.size() * sizeof(wchar_t)) + 1;
    auto mem = new wchar_t[bytes];
    memcpy(mem, prgname.c_str(), bytes);

    Py_Main(1, &mem);

    delete[] mem;

}
