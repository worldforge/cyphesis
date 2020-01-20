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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "../python_testers.h"

#include "rules/python/PythonArithmeticScript.h"

#include "common/compose.hpp"
#include "common/log.h"

#include <cassert>
#include <rules/python/Python_Script_Utils.h>
#include "pycxx/CXX/Extensions.hxx"

Py::Object Get_PyClass(const Py::Module& module,
                       const std::string & package,
                       const std::string & type)
{
    auto py_class = module.getAttr(type);
    if (py_class.isNull()) {
        log(ERROR, String::compose("Could not find python class \"%1.%2\"",
                                   package, type));
        PyErr_Print();
        return Py::Null();
    }
    if (!py_class.isCallable()) {
        log(ERROR, String::compose("Could not instance python class \"%1.%2\"",
                                   package, type));
        return Py::Null();
    }
    return py_class;
}

Py::Module Get_PyModule(const std::string & package)
{
    Py::String package_name(package);
    PyObject * module = PyImport_Import(package_name.ptr());
    if (module == nullptr) {
        log(ERROR, String::compose("Missing python module \"%1\"", package));
        PyErr_Print();
        return Py::Module(nullptr);
    }
    return Py::Module(module);
}

struct TestMod : public Py::ExtensionModule<TestMod>
{
    TestMod() : ExtensionModule("testmod")
    {
        initialize("testmod");
    }
};



int main()
{

    {
        PyImport_AppendInittab("testmod", []() {
            auto module = new TestMod();
            return module->module().ptr();
        });
        Py_Initialize();

        run_python_string("import testmod");
        run_python_string("class TestArithmeticScript(object):\n"
                          " def __init__(self):\n"
                          "  self.foo=1\n"
                          "  self.bar=1.1\n"
                          "  self.baz=None\n"
                          "  self.qux='1'\n"
                         );
        run_python_string("testmod.TestArithmeticScript=TestArithmeticScript");

        auto testmod = Get_PyModule("testmod");

        assert(!testmod.isNull());

        auto clss = Get_PyClass(testmod, "testmod", "TestArithmeticScript");

        assert(!clss.isNull());

        auto instance = Py::Callable(clss).apply();

        assert(!instance.isNull());

        PythonArithmeticScript pas(instance);

        float val;
        pas.attribute("foo", val);
        pas.attribute("bar", val);
        pas.attribute("baz", val);
        pas.attribute("qux", val);
        pas.attribute("nonexistent", val);

        pas.set("foo", 1.0f);
        pas.set("mim", 1.0f);
    }

    Py_Finalize();
    return 0;
}

// stubs

PyObject * Get_PyClass(PyObject * module,
                       const std::string & package,
                       const std::string & type)
{
    PyObject * py_class = PyObject_GetAttrString(module, (char *)type.c_str());
    if (py_class == nullptr) {
        log(ERROR, String::compose("Could not find python class \"%1.%2\"",
                                   package, type));
        PyErr_Print();
        return nullptr;
    }
    if (PyCallable_Check(py_class) == 0) {
        log(ERROR, String::compose("Could not instance python class \"%1.%2\"",
                                   package, type));
        Py_DECREF(py_class);
        return nullptr;
    }
    if (PyType_Check(py_class) == 0) {
        log(ERROR, String::compose("PyCallable_Check returned true, "
                                   "but PyType_Check returned false \"%1.%2\"",
                                   package, type));
        Py_DECREF(py_class);
        return nullptr;
    }
    return py_class;
}

#include "../stubs/common/stublog.h"
