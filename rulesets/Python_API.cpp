// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
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


#include "Python.h"

#include "Python_API.h"
#include "Python_Script_Utils.h"

#include "PythonEntityScript.h"
#include "BaseMind.h"

#include "common/globals.h"
#include "common/const.h"
#include "common/debug.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <common/AssetsManager.h>
#include <boost/algorithm/string.hpp>
#include <boost/asio/steady_timer.hpp>
#include <rulesets/python/CyPy_EntityFilter.h>
#include <rulesets/python/CyPy_Common.h>
#include <rulesets/python/CyPy_Atlas.h>
#include <rulesets/python/CyPy_Physics.h>
#include <rulesets/python/CyPy_Server.h>
#include <rulesets/python/WrapperBase.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

/// \defgroup PythonWrappers Python Wrapper Types
///
/// Structure types based on the PyObject header used to wrap C++ objects
/// in Python.


//////////////////////////////////////////////////////////////////////////
// Logger replaces sys.stdout and sys.stderr so the nothing goes to output
//////////////////////////////////////////////////////////////////////////


/// \brief Python type to handle output from python scripts
///
/// In instance of this type is used to replace sys.stdout and sys.stderr
/// in the Python interpreter so that all script output goes to the cyphesis
/// log subsystem
struct LogWriter : public WrapperBase<LogLevel, LogWriter>
{
    LogWriter(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
        : WrapperBase(self, args, kwds)
    {

    }

    LogWriter(Py::PythonClassInstance* self, LogLevel value)
        : WrapperBase(self, value)
    {

    }

    void python_log(LogLevel lvl, std::string message)
    {
        std::string::size_type n = 0;
        std::string::size_type p;
        for (p = message.find_first_of('\n');
             p != std::string::npos;
             p = message.find_first_of('\n', n)) {
            log(lvl, message.substr(n, p - n));
            n = p + 1;
        }
        if (message.size() > n) {
            message = message.substr(n, message.size() - n);
        } else {
            message.clear();
        }
    }

    Py::Object write(const Py::Tuple& args)
    {
        args.verify_length(1);

        python_log(m_value, std::move(verifyString(args.front())));

        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL(LogWriter, write)

    Py::Object flush()
    {
        return Py::None();
    }
    PYCXX_NOARGS_METHOD_DECL(LogWriter, flush)

    static void init_type() {
        behaviors().name("LogWriter");
        behaviors().doc("");

        PYCXX_ADD_NOARGS_METHOD(flush, flush, "");
        PYCXX_ADD_VARARGS_METHOD(write, write, "");

        behaviors().readyType();
    }

};

/// \brief Find a class in a Python module
///
/// @param module the imported Python module
/// @param package the name of the module for error reporting
/// @param type the name of the class or type
/// @return new reference
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

/// \brief Import a Python module
///
/// @param package the name of the module
/// @return new reference
PyObject * Get_PyModule(const std::string & package)
{
    PyObject * package_name = PyUnicode_FromString((char *)package.c_str());
    PyObject * module = PyImport_Import(package_name);
    Py_DECREF(package_name);
    if (module == nullptr) {
        log(ERROR, String::compose("Missing python module \"%1\"", package));
        PyErr_Print();
    }
    return module;
}

PyObject * Create_PyScript(PyObject * wrapper, PyObject * py_class)
{
    PyObject * pyob = PyEval_CallFunction(py_class,"(O)", wrapper);

    if (pyob == nullptr) {
        if (PyErr_Occurred() == nullptr) {
            log(ERROR, "Could not create python instance");
        } else {
            log(ERROR, "Reporting python error");
            PyErr_Print();
        }
    }
    return pyob;
}

sigc::signal<void> python_reload_scripts;
std::vector<std::string> python_directories;

std::map<boost::filesystem::path, std::string> changedPaths;

void reloadChangedPaths() {
    if (!changedPaths.empty()) {

        for (auto& entry : changedPaths) {
            auto& package = entry.second;
            auto& path = entry.first;
            auto module = Get_PyModule(package);
            if (module != nullptr) {
                log(INFO, String::compose("Reloading module \"%1\" from file %2.", package, path));
                auto result = PyImport_ReloadModule(module);
                if (result != module) {
                    log(WARNING, String::compose("New pointer returned when reloading module \"%1\".", package));
                }
                Py_DECREF(module);

            }

        }
        python_reload_scripts();
        changedPaths.clear();
    }
}

void observe_python_directories(boost::asio::io_service& io_service, AssetsManager& assetsManager) {

    for (auto& directory : python_directories) {
        AssetsManager::instance().observeDirectory(directory, [=, &io_service](const boost::filesystem::path& path) {
            //Trim the ".py" extension
            if (boost::ends_with(path.string(), ".py")) {
                auto relative = path.string().substr(directory.length() + 1);
                relative = relative.substr(0, relative.size() - 3);
                static char separator[] = {boost::filesystem::path::preferred_separator, 0};
                auto package = boost::replace_all_copy(relative, separator, ".");
                changedPaths[path] = package;

                auto timer = std::make_shared<boost::asio::steady_timer>(io_service);
                timer->expires_from_now(std::chrono::milliseconds(20));
                timer->async_wait([&, timer](const boost::system::error_code& ec) {
                    if (!ec) {
                        reloadChangedPaths();
                    }
                });
            }

        });
    }
}


CyPy_Server* server;

void register_baseworld_with_python(BaseWorld* baseWorld)
{
    server->registerWorld(baseWorld);
}

void init_python_api(const std::string & ruleset, bool log_stdout)
{

    PyImport_AppendInittab("entity_filter", [](){
        static auto module = new CyPy_EntityFilter();
        return module->module().ptr();
    });
    PyImport_AppendInittab("atlas", [](){
        static auto module = new CyPy_Atlas();
        return module->module().ptr();
    });
    PyImport_AppendInittab("physics", [](){
        static auto module = new CyPy_Physics();
        return module->module().ptr();
    });
    PyImport_AppendInittab("common", [](){
        static auto module = new CyPy_Common();
        return module->module().ptr();
    });
    PyImport_AppendInittab("server", [](){
        server = new CyPy_Server();
        return server->module().ptr();
    });

    Py_InitializeEx(0);

    //Make sure that all modules are imported, since this is needed to initialize all types.
    //Otherwise we risk that we try to invoke the types from the C++ code before they have
    //been populated.
    PyImport_ImportModule("atlas");
    PyImport_ImportModule("common");
    PyImport_ImportModule("physics");
    PyImport_ImportModule("entity_filter");
    PyImport_ImportModule("server");

    Py::Module sys_module(PyImport_Import(Py::String("sys").ptr()));

    if (sys_module.isNull()) {
        log(CRITICAL, "Python could not import sys module");
        return;
    }

    if (log_stdout) {
        LogWriter::init_type();
        sys_module.setAttr("stdout", LogWriter::wrap(SCRIPT));
        sys_module.setAttr("stderr", LogWriter::wrap(SCRIPT_ERROR));
    }

    auto sys_path = sys_module.getAttr("path");
    if (!sys_path.isNull()) {
        if (sys_path.isList()) {
            Py::List paths(sys_path);

            // Add the path to the non-ruleset specific code.
            python_directories.push_back(share_directory + "/cyphesis/scripts");
            python_directories.push_back(share_directory + "/cyphesis/rulesets/basic/scripts");
            // Add the path to the ruleset specific code.
            python_directories.push_back(share_directory + "/cyphesis/rulesets/" + ruleset + "/scripts");

            for (auto& path : python_directories) {
                paths.append(Py::String(path));
            }

        } else {
            log(CRITICAL, "Python sys.path is not a list");
        }
    } else {
        log(CRITICAL, "Python could not import sys.path");
    }

    debug(std::cout << Py_GetPath() << std::endl << std::flush;);
}

void shutdown_python_api()
{

    Py_Finalize();
}
