// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "ObserverClient.h"

#include "rulesets/Python_API.h"

#include "common/globals.h"

#include <varconf/Config.h>

#include <iostream>

#include <cassert>

static void usage(const char * prgname)
{
    std::cout << "usage: " << prgname << " [ [package.]function ]"
              << std::endl << std::flush;
}

int main(int argc, char ** argv)
{
    int optind;
    if ((optind = loadConfig(argc, argv)) < 0) {
        // Fatal error loading config file
        return 1;
    }

    assert(optind <= argc);

    std::string account = "admin";
    if (global_conf->findItem("client", "account")) {
        account = (std::string)global_conf->getItem("client", "account");
    }

    std::string password;
    if (global_conf->findItem("client", "password")) {
        password = (std::string)global_conf->getItem("client", "password");
    }

    std::string package;
    if (global_conf->findItem("client", "package")) {
        package = (std::string)global_conf->getItem("client", "package");
    }

    std::string function;
    if (global_conf->findItem("client", "function")) {
        function = (std::string)global_conf->getItem("client", "function");
    }

    if (optind == (argc - 1)) {
        std::string arg(argv[optind]);
        std::string::size_type pos = arg.rfind(".");
        if (pos == std::string::npos) {
            std::cout << "function " << arg << std::endl << std::flush;
            function = arg;
        } else {
            package = arg.substr(0, pos);
            function = arg.substr(pos + 1);
            std::cout << "module.function " << package << "." << function << std::endl << std::flush;
        }
    } else if (optind != argc) {
        usage(argv[0]);
        return 1;
    }

    init_python_api();

    try {
        ObserverClient & observer = *new ObserverClient();
        if (observer.setup(account, password) != 0) {
            return 1;
        }
        observer.load(package, function);
        //observer.run();
    }
    catch (...) {
        std::cerr << "EMERGENCY: cyclient: Exception caught in main; exiting" << std::endl << std::flush;
    }

    shutdown_python_api();
}
