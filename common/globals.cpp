// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "globals.h"

#include <varconf/Config.h>

varconf::Config * global_conf = NULL;
std::string share_directory = SHAREDIR;
std::list<std::string> rulesets;
bool exit_flag = false;
bool daemon_flag = false;
int timeoffset = 0;
int port_num = 6767;

bool loadConfig(int argc, char ** argv)
{
    global_conf = varconf::Config::inst();

    // See if the user has set the install directory on the command line
    char * home;
    bool home_dir_config = false;
    if ((home = getenv("HOME")) != NULL) {
        home_dir_config = global_conf->readFromFile(std::string(home) + "/.cyphesis.vconf");
    }
    // Check the command line options, and if anything
    // has been overriden, store this value in the users home directory.
    // The effect of this code is that config settings, once
    // chosen are fixed.
    global_conf->getCmdline(argc, argv);
    if (global_conf->findItem("cyphesis", "directory")) {
        share_directory = global_conf->getItem("cyphesis", "directory");
    }
    if (argc > 1) {
        if (home != NULL) {
            global_conf->writeToFile(std::string(home) + "/.cyphesis.vconf");
        }
    }
    // Load up the rest of the system config file, and then ensure that
    // settings are overridden in the users config file, and the command line
    bool main_config = global_conf->readFromFile(share_directory +
                                                 "/cyphesis/cyphesis.vconf");
    if (!main_config) {
        std::cerr << "FATAL: Unable to read main config file "
                  << share_directory << "/cyphesis/cyphesis.vconf."
                  << std::endl;
        if (home_dir_config) {
            std::cerr << "Try removing .cyphesis.vconf from your home directory as it may specify an invalid installation directory, and then restart cyphesis."
                      << std::endl << std::flush;
        } else {
            std::cerr << "Please ensure that cyphesis has been installed correctly."
                      << std::endl << std::flush;
        }
        return true;
    }
    if (home_dir_config) {
        global_conf->readFromFile(std::string(home) + "/.cyphesis.vconf");
    }
    global_conf->getCmdline(argc, argv);

    // Config is now loaded. Now set the values of some globals.

    if (global_conf->findItem("cyphesis", "daemon")) {
        daemon_flag = global_conf->getItem("cyphesis","daemon");
    }

    if (global_conf->findItem("cyphesis", "tcpport")) {
        port_num=global_conf->getItem("cyphesis","tcpport");
    }

    // Load up the rulesets. Rulesets are hierarchical, and are read in until
    // one is read in that does not specify its parent ruleset.
    std::string ruleset = "cyphesis";
    while (global_conf->findItem(ruleset, "ruleset")) {
        ruleset = global_conf->getItem(ruleset, "ruleset");
        rulesets.push_back(ruleset);
    };

    return false;
}
