// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#ifndef RULESETS_PYTHON_API_H
#define RULESETS_PYTHON_API_H

#include <string>
#include <set>
#include <sigc++/signal.h>
#include "common/io_context.h"
#include "common/log.h"

class AssetsManager;

class BaseWorld;


/**
 * Emitted when python scripts needs reloading.
 */
extern sigc::signal<void()> python_reload_scripts;


/**
 * Registers and unregisters the supplied log injection function.
 * Use this to wrap calls into Python so that informative messages are
 * prepended to any log output.
 */
struct PythonLogGuard
{

    explicit PythonLogGuard(const std::function<std::string()>& logFn)
    {
        s_logPrefixFn = logFn;
    }

    ~PythonLogGuard()
    {
        s_logPrefixFn = nullptr;
    }
};


/**
 * Initializes the Python embedded interpreter and scripting engine.
 * @param initFunctions A list of functions to call for registering modules.
 * @param scriptDirectories A list of file system directories in which to look for Python scripts to run at startup.
 * @param log_stdout True if Python should write log messages to std::cout.
 */
void init_python_api(std::vector<std::function<std::string()>> initFunctions, std::vector<std::string> scriptDirectories = {}, bool log_stdout = true);

void shutdown_python_api();

/**
 * When called will look in the XDG user directory for any scripts to run, for the supplied prefix.
 * @param prefix
 */
void run_user_scripts(const std::string& prefix);

void observe_python_directories(boost::asio::io_context& io_context, AssetsManager& assetsManager);


#endif // RULESETS_PYTHON_API_H
