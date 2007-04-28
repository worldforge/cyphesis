// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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

// $Id: globals.cpp,v 1.42 2007-04-28 15:21:18 alriddoch Exp $

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "globals.h"

#include "prefix.h"
#include "const.h"
#include "log.h"
#include "compose.hpp"

#include "modules/DateTime.h"

#include <varconf/config.h>

#include <sys/stat.h>

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
bool database_flag = true;
bool pvp_flag = false;
bool pvp_offl_flag = false;
int timeoffset = DateTime::spm() * DateTime::mph() * 9; // Morning
int client_port_num = 6767;
int slave_port_num = 6768;
int peer_port_num = 6769;

static const char * FALLBACK_LOCALSTATEDIR = "/var";

static int check_tmp_path(const std::string & dir)
{
    std::string tmp_directory = var_directory + "/tmp";
    struct stat tmp_stat;

    if (::stat(tmp_directory.c_str(), &tmp_stat) != 0) {
        return -1;
    }

    if (!S_ISDIR(tmp_stat.st_mode)) {
        return -1;
    }

    return 0;
}

int loadConfig(int argc, char ** argv, bool server)
{
    global_conf = varconf::Config::inst();

    global_conf->setParameterLookup('h', "help");
    global_conf->setParameterLookup('?', "help");

    global_conf->setParameterLookup('v', "version");

    // See if the user has set the install directory on the command line
    bool home_dir_config = false;
    char * home = getenv("HOME");

    // Read in only the users settings, and the commandline settings.
    if (home != NULL) {
        home_dir_config = global_conf->readFromFile(std::string(home) + "/.cyphesis.vconf");
    }

    global_conf->getCmdline(argc, argv);

    if (global_conf->findItem("", "version")) {
        return CONFIG_VERSION;
    }

    if (global_conf->findItem("", "help")) {
        return CONFIG_HELP;
    }

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
        return CONFIG_ERROR;
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

    if (rulesets.empty()) {
        log(ERROR, "No ruleset specified in config. Using \"basic\" rules.");
        log(INFO, "The basic rules don't allow much, so this should be rectified.");
        rulesets.push_back("basic");
    }

    if (check_tmp_path(var_directory) != 0) {
        if (var_directory != "/usr/var") {
            // Binreloc enabled builds installed system wide have localstatedir
            // set to something that is never writable, so must always fall
            // back to /var/tmp, so we should not display the message.
            log(WARNING,
                String::compose("No temporary directory found at \"%1/tmp\"",
                                var_directory).c_str());
        }
        if (check_tmp_path(FALLBACK_LOCALSTATEDIR) != 0) {
            log(CRITICAL, String::compose("No temporary directory available at \"%1/tmp\" or \"%1/tmp\".", var_directory, FALLBACK_LOCALSTATEDIR).c_str());
        } else {
            if (var_directory != "/usr/var") {
                log(NOTICE,
                    String::compose("Using \"%1/tmp\" as temporary directory",
                                    FALLBACK_LOCALSTATEDIR).c_str());
            }
            var_directory = FALLBACK_LOCALSTATEDIR;
        }
    }

    return optind;
}

void reportVersion(const char * prgname)
{
    std::cout << prgname << " (cyphesis) " << consts::version
              << " (WorldForge)" << std::endl << std::flush;
}
