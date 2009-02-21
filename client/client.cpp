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

// $Id$

#include "ObserverClient.h"
#include "ClientPropertyManager.h"

#include "rulesets/Python_API.h"

#include "common/globals.h"
#include "common/log.h"

#include <varconf/config.h>

#include <iostream>

#include <cassert>

static void usage(const char * prgname)
{
    std::cout << "usage: " << prgname << " [ [package.]function ]"
              << std::endl << std::flush;
}

int main(int argc, char ** argv)
{
    int config_status = loadConfig(argc, argv, USAGE_CLIENT); 
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            reportVersion(argv[0]);
            return 0;
        } else if (config_status == CONFIG_HELP) {
            showUsage(argv[0], USAGE_CLIENT, "[ [package.]function]");
            return 0;
        } else if (config_status != CONFIG_ERROR) {
            log(ERROR, "Unknown error reading configuration.");
        }
        // Fatal error loading config file
        return 1;
    }

    int optind = config_status;

    assert(optind <= argc);

    std::string server = "localhost";
    readConfigItem("client", "serverhost", server);

    std::string account = "admin";
    readConfigItem("client", "account", account);

    std::string password;
    readConfigItem("client", "password", password);

    std::string package;
    readConfigItem("client", "package", package);

    std::string function;
    readConfigItem("client", "function", function);

    if (optind == (argc - 1)) {
        std::string arg(argv[optind]);
        std::string::size_type pos = arg.rfind(".");
        if (pos == std::string::npos) {
            // std::cout << "function " << arg << std::endl << std::flush;
            function = arg;
        } else {
            package = arg.substr(0, pos);
            function = arg.substr(pos + 1);
            // std::cout << "module.function " << package << "." << function << std::endl << std::flush;
        }
    } else if (optind != argc) {
        usage(argv[0]);
        return 1;
    }

    init_python_api();

    try {
        new ClientPropertyManager();
        ObserverClient & observer = *new ObserverClient();
        observer.setServer(server);
        if (observer.setup(account, password) != 0) {
            std::cerr << "ERROR: Connection failed."
                      << std::endl << std::flush;
            return 1;
        }
        observer.load(package, function);
        //observer.run();
        observer.logout();
        delete &observer;
    }
    catch (...) {
        std::cerr << "EMERGENCY: cyclient: Exception caught in main; exiting" << std::endl << std::flush;
    }

    shutdown_python_api();
}
