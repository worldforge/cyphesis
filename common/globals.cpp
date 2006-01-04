// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "prefix.h"
#include "globals.h"
#include "log.h"

#include "modules/DateTime.h"

#include <varconf/config.h>

#include <cassert>

varconf::Config * global_conf = NULL;
std::string share_directory(DATADIR);
std::string etc_directory(SYSCONFDIR);
std::string var_directory(LOCALSTATEDIR);
std::string client_socket_name("cyphesis.sock");
std::string slave_socket_name("cyslave.sock");
std::vector<std::string> rulesets;
bool exit_flag = false;
bool daemon_flag = false;
bool restricted_flag = false;
bool pvp_flag = false;
bool pvp_offl_flag = false;
int timeoffset = DateTime::spm() * DateTime::mph() * 9; // Morning
int client_port_num = 6767;
int slave_port_num = 6768;
int peer_port_num = 6769;

int loadConfig(int argc, char ** argv, bool server)
{
    global_conf = varconf::Config::inst();

    // See if the user has set the install directory on the command line
    bool home_dir_config = false;
    char * home = getenv("HOME");

    // Read in only the users settings, and the commandline settings.
    if (home != NULL) {
        home_dir_config = global_conf->readFromFile(std::string(home) + "/.cyphesis.vconf");
    }

    global_conf->getCmdline(argc, argv);

    // Check if the config directory has been overriden at this point, as if
    // it has, that will affect loading the main config.
    if (global_conf->findItem("cyphesis", "confdir")) {
        etc_directory = global_conf->getItem("cyphesis", "confdir").as_string();
    }

    // Load up the rest of the system config file, and then ensure that
    // settings are overridden in the users config file, and the command line
    bool main_config = global_conf->readFromFile(etc_directory +
                                                 "/cyphesis/cyphesis.vconf",
                                                 varconf::GLOBAL);
    if (!main_config) {
        std::string msg("Unable to read main config file ");
        msg += etc_directory;
        msg += "/cyphesis/cyphesis.vconf.";
        log(CRITICAL, msg.c_str());
        if (home_dir_config) {
            log(INFO, "Try removing .cyphesis.vconf from your home directory as it may specify an invalid installation directory, and then restart cyphesis.");
        } else {
            log(INFO, "Please ensure that cyphesis has been installed correctly.");
        }
        return -1;
    }
    if (home_dir_config) {
        global_conf->readFromFile(std::string(home) + "/.cyphesis.vconf");
    }
    int optind = global_conf->getCmdline(argc, argv);

    // Write out any changes that have been overriden at user scope. It
    // may be a good idea to do this at shutdown.
    if (home != NULL) {
        global_conf->writeToFile(std::string(home) + "/.cyphesis.vconf", varconf::USER);
    }

    assert(optind > 0);

    // Config is now loaded. Now set the values of some globals.

    if (global_conf->findItem("cyphesis", "directory")) {
        share_directory = global_conf->getItem("cyphesis", "directory").as_string();
    }

    if (global_conf->findItem("cyphesis", "confdir")) {
        etc_directory = global_conf->getItem("cyphesis", "confdir").as_string();
    }

    if (global_conf->findItem("cyphesis", "vardir")) {
        var_directory = global_conf->getItem("cyphesis", "vardir").as_string();
    }

    if (global_conf->findItem("cyphesis", "daemon") && server) {
        daemon_flag = global_conf->getItem("cyphesis","daemon");
    }

    if (global_conf->findItem("cyphesis", "tcpport")) {
        client_port_num = global_conf->getItem("cyphesis","tcpport");
    }

    if (global_conf->findItem("cyphesis", "unixport")) {
        client_socket_name = global_conf->getItem("cyphesis","unixport").as_string();
    }

    if (global_conf->findItem("slave", "tcpport")) {
        slave_port_num = global_conf->getItem("slave","tcpport");
    }

    if (global_conf->findItem("slave", "unixport")) {
        slave_socket_name = global_conf->getItem("slave","unixport").as_string();
    }

    if (global_conf->findItem("game", "player_vs_player")) {
        pvp_flag = global_conf->getItem("game", "player_vs_player");
    }

    if (global_conf->findItem("game", "player_vs_player_offline")) {
        pvp_offl_flag = global_conf->getItem("game", "player_vs_player_offline");
    }

    // Load up the rulesets. Rulesets are hierarchical, and are read in until
    // one is read in that does not specify its parent ruleset.
    std::string ruleset = "cyphesis";
    while (global_conf->findItem(ruleset, "ruleset")) {
        ruleset = global_conf->getItem(ruleset, "ruleset").as_string();
        rulesets.push_back(ruleset);
    };

    return optind;
}
