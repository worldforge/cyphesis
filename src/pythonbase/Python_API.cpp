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

#include "rules/ai/BaseMind.h"

#include "common/globals.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/AssetsManager.h"
#include "PythonMalloc.h"
#include "pythonbase/WrapperBase.h"
#include "Remotery.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <boost/algorithm/string.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/filesystem.hpp>

#include <basedir.h>

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
    std::string m_message;

    LogWriter(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
            : WrapperBase(self, args, kwds)
    {

    }

    LogWriter(Py::PythonClassInstance* self, LogLevel value)
            : WrapperBase(self, value)
    {

    }

    void python_log(LogLevel lvl, const std::string& msg)
    {

        m_message += msg;
        std::string::size_type n = 0;
        std::string::size_type p;
        for (p = m_message.find_first_of('\n');
             p != std::string::npos;
             p = m_message.find_first_of('\n', n)) {
            log(lvl, m_message.substr(n, p - n));
            n = p + 1;
        }
        if (m_message.size() > n) {
            m_message = m_message.substr(n, m_message.size() - n);
        } else {
            m_message.clear();
        }

    }

    Py::Object write(const Py::Tuple& args)
    {
        if (args.length() > 0) {
            python_log(m_value, verifyString(args.front()));
        }

        return Py::None();
    }

    PYCXX_VARARGS_METHOD_DECL(LogWriter, write)

    Py::Object flush()
    {
        return Py::None();
    }

    PYCXX_NOARGS_METHOD_DECL(LogWriter, flush)

    static void init_type()
    {
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
Py::Object Get_PyClass(const Py::Module& module,
                       const std::string& package,
                       const std::string& type)
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

/// \brief Import a Python module
///
/// @param package the name of the module
/// @return new reference
Py::Module Get_PyModule(const std::string& package)
{
    Py::String package_name(package);
    PyObject* module = PyImport_Import(package_name.ptr());
    if (module == nullptr) {
        log(ERROR, String::compose("Missing python module \"%1\"", package));
        PyErr_Print();
        return Py::Module(nullptr);
    }
    return Py::Module(module);
}

Py::Object Create_PyScript(const Py::Object& wrapper, const Py::Callable& py_class)
{
    try {
        return py_class.apply(Py::TupleN(wrapper));
    } catch (...) {
        if (PyErr_Occurred() == nullptr) {
            log(ERROR, "Could not create python instance");
        } else {
            log(ERROR, "Reporting python error");
            PyErr_Print();
        }
        return Py::None();
    }
}

sigc::signal<void()> python_reload_scripts;

std::vector<std::string> python_directories;

std::map<boost::filesystem::path, std::string> changedPaths;

namespace {
    void reloadChangedPaths()
    {
        if (!changedPaths.empty()) {

            for (auto& entry: changedPaths) {
                auto& package = entry.second;
                auto& path = entry.first;
                auto module = Get_PyModule(package);
                if (!module.isNull()) {
                    log(INFO, String::compose("Reloading module \"%1\" from file %2.", package, path));
                    auto result = PyImport_ReloadModule(module.ptr());
                    if (result != module.ptr()) {
                        log(WARNING, String::compose("New pointer returned when reloading module \"%1\".", package));
                    }

                }

            }
            python_reload_scripts();
            changedPaths.clear();
        }
    }
}

void observe_python_directories(boost::asio::io_context& io_context, AssetsManager& assetsManager)
{

    for (auto& directory: python_directories) {
        assetsManager.observeDirectory(directory, [=, &io_context](const boost::filesystem::path& path) {
            //Trim the ".py" extension
            if (boost::ends_with(path.string(), ".py")) {
                auto relative = path.string().substr(directory.length() + 1);
                relative = relative.substr(0, relative.size() - 3);
                static char separator[] = {boost::filesystem::path::preferred_separator, 0};
                auto package = boost::replace_all_copy(relative, separator, ".");
                changedPaths[path] = package;

                auto timer = std::make_shared<boost::asio::steady_timer>(io_context);
#if BOOST_VERSION >= 106600
                timer->expires_after(std::chrono::milliseconds(20));
#else
                timer->expires_from_now(std::chrono::milliseconds(20));
#endif
                timer->async_wait([&, timer](const boost::system::error_code& ec) {
                    if (!ec) {
                        rmt_ScopedCPUSample(PythonReload, 0)
                        reloadChangedPaths();
                    }
                });
            }

        });
    }
}

void init_python_api(std::vector<std::function<std::string()>> initFunctions, std::vector<std::string> scriptDirectories, bool log_stdout)
{
    //If we're using the system Python installation then everything should be setup for Python to use.
    //But if we're instead using something like a Conan version where Python is installed in a different place
    //we need to tell Python where to find its stuff. This is done through the PYTHONHOME environment variable.
    //Our build system is setup so that it will inject the preprocessor variable by that same name in that case.
#if defined(PYTHONHOME)
    log(INFO, "Setting Python home directory to " PYTHONHOME);
    setenv("PYTHONHOME", PYTHONHOME, 1);
#endif

    python_directories = std::move(scriptDirectories);

    std::vector<std::string> modules;
    for (auto& function: initFunctions) {
        modules.emplace_back(function());
    }

    char* usemalloc = getenv("PYTHONMALLOC");

    if (usemalloc) {
        setupPythonMalloc();
        log(INFO, "Python is using malloc for memory allocation.");
    }

    Py_InitializeEx(0);

    //Make sure that all modules are imported, since this is needed to initialize all types.
    //Otherwise we risk that we try to invoke the types from the C++ code before they have
    //been populated.
    for (auto& moduleName: modules) {
        PyImport_ImportModule(moduleName.c_str());
    }

    PyImport_ImportModule("sys");
    PyImport_ImportModule("traceback");

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

            for (auto& path: python_directories) {
                paths.append(Py::String(path));
            }

        } else {
            log(CRITICAL, "Python sys.path is not a list");
        }
    } else {
        log(CRITICAL, "Python could not import sys.path");
    }

    debug_print(Py_GetPath())
}

void shutdown_python_api()
{

    Py_Finalize();
}

void run_user_scripts(const std::string& prefix)
{
    xdgHandle baseDirHandle{};
    boost::filesystem::path path;
    if (xdgInitHandle(&baseDirHandle)) {
        auto dataHome = xdgDataHome(&baseDirHandle);
        if (dataHome) {
            path = dataHome;
        }
        xdgWipeHandle(&baseDirHandle);
    }

    if (!path.empty()) {
        path /= "cyphesis";
        path /= (prefix + ".d");
        log(INFO, String::compose("Looking for extra python scripts in %1.", path));
        if (boost::filesystem::is_directory(path)) {
            boost::filesystem::recursive_directory_iterator dir(path), end{};

            while (dir != end) {
                if (boost::filesystem::is_regular_file(dir->status()) && dir->path().extension() == ".py") {
                    auto fileHandle = fopen(dir->path().c_str(), "r");
                    if (fileHandle) {
                        log(INFO, String::compose("Running script '%1'.", dir->path().c_str()));
                        PyRun_SimpleFileEx(fileHandle, dir->path().c_str(), true);
                        if (PyErr_Occurred()) {
                            PyErr_Print();
                        }
                    }
                }
                ++dir;
            }
        }

    }
}
