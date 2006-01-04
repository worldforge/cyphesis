// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "ObserverClient.h"

#include "rulesets/Python_API.h"

#include "common/globals.h"

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
    int optind;
    if ((optind = loadConfig(argc, argv)) < 0) {
        // Fatal error loading config file
        return 1;
    }

    assert(optind <= argc);

    std::string server = "localhost";
    if (global_conf->findItem("client", "serverhost")) {
        server = global_conf->getItem("client", "serverhost").as_string();
    }

    std::string account = "admin";
    if (global_conf->findItem("client", "account")) {
        account = global_conf->getItem("client", "account").as_string();
    }

    std::string password;
    if (global_conf->findItem("client", "password")) {
        password = global_conf->getItem("client", "password").as_string();
    }

    std::string package;
    if (global_conf->findItem("client", "package")) {
        package = global_conf->getItem("client", "package").as_string();
    }

    std::string function;
    if (global_conf->findItem("client", "function")) {
        function = global_conf->getItem("client", "function").as_string();
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
        observer.setServer(server);
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
