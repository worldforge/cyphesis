// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "ObserverClient.h"

#include <common/globals.h>

void init_python_api();

int main(int argc, char ** argv)
{
    global_conf = varconf::Config::inst();

    char * home;
    bool home_dir_config = false;
    if ((home = getenv("HOME")) != NULL) {
        home_dir_config = global_conf->readFromFile(std::string(home) + "/.cyphesis.vconf");
    }

    global_conf->getCmdline(argc, argv);
    if (global_conf->findItem("cyphesis", "directory")) {
        share_directory = global_conf->getItem("cyphesis", "directory");
        if (home != NULL) {
            global_conf->writeToFile(std::string(home) + "/.cyphesis.vconf");
        }
    }

    bool main_config = global_conf->readFromFile(share_directory +
                                                 "/cyphesis/cyphesis.vconf");
    if (!main_config) {
        std::cerr << "FATAL: Unable to read main config file "
                  << share_directory << "/cyphesis/cyphesis.vconf."
                  << std::endl << std::flush;
        if (home_dir_config) {
            std::cerr << "Try removing .cyphesis.vconf from your home directory as it may specify an invalid installation directory, and then restart cyphesis."
                      << std::endl << std::flush;
        } else {
            std::cerr << "Please ensure that cyphesis has been installed correctly."
                      << std::endl << std::flush;
        }
        return 1;
    }
    if (home_dir_config) {
        global_conf->readFromFile(std::string(home) + "/.cyphesis.vconf");
    }
    global_conf->getCmdline(argc, argv);

    std::string ruleset;
    while (global_conf->findItem("cyphesis", "ruleset")) {
        ruleset = global_conf->getItem("cyphesis", "ruleset");
        global_conf->erase("cyphesis", "ruleset");
        cout << "Reading in " << ruleset << endl << flush;
        global_conf->readFromFile(share_directory + "/cyphesis/" + ruleset + ".vconf");
        rulesets.push_back(ruleset);
    };

    init_python_api();

    try {
        // if init.display:;
            // print "Use --nodisplay to hide 'text graphics'";
        ObserverClient & observer = *new ObserverClient();
        if (!observer.setup()) {
            return 1;
        }
        observer.loadDefault();
        //observer.run();
    }
    catch (...) {
        std::cerr << "EMERGENCY: cyphesis_client: Exception caught in main; exiting" << std::endl << std::flush;
    }
}
