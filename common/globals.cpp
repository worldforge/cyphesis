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

// $Id: globals.cpp,v 1.56 2007-12-04 19:18:17 alriddoch Exp $

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

#include <algorithm>

#include <sys/stat.h>

#include <cassert>

static const char * DEFAULT_RULESET = "mason";
static const char * DEFAULT_CLIENT_SOCKET = "cyphesis.sock";
static const char * DEFAULT_SLAVE_SOCKET = "cyslave.sock";
static const char * DEFAULT_INSTANCE = "cyphesis";

varconf::Config * global_conf = NULL;
std::string instance(DEFAULT_INSTANCE);
std::string share_directory(DATADIR);
std::string etc_directory(SYSCONFDIR);
std::string var_directory(LOCALSTATEDIR);
std::string client_socket_name(DEFAULT_CLIENT_SOCKET);
std::string slave_socket_name(DEFAULT_SLAVE_SOCKET);
std::string ruleset(DEFAULT_RULESET);
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
int dynamic_port_start = 6780;
int dynamic_port_end = 6799;

static const char * FALLBACK_LOCALSTATEDIR = "/var";

static const int S = USAGE_SERVER;
static const int C = USAGE_CLIENT;
static const int M = USAGE_CYCMD;
static const int D = USAGE_DBASE;

typedef struct {
    const char * section;
    const char * option;
    const char * value;
    const char * dflt;
    const char * description;
    int flags;
} usage_data;

static const usage_data usage[] = {
    { "cyphesis", "instance", "<short_name>", "\"cyphesis\"", "Unique short name for the server instance", S|C|M|D },
    { "cyphesis", "directory", "<directory>", "", "Directory where server data and scripts can be found", S|C },
    { "cyphesis", "confdir", "<directory>", "", "Directory where server config can be found", S|C|M|D },
    { "cyphesis", "vardir", "<directory>", "", "Directory where temporary files can be stored", S|C|M },
    { "cyphesis", "ruleset", "<name>", DEFAULT_RULESET, "Ruleset name", S|C|D },
    { "cyphesis", "servername", "<name>", "<hostname>", "Published name of the server", S|C },
    { "cyphesis", "tcpport", "<portnumber>", "6767", "Network listen port for client connections", S|C|M },
    { "cyphesis", "dynamic_port_start", "<portnumber>", "6780", "Lowest port to try and used for dyanmic ports", S },
    { "cyphesis", "dynamic_port_end", "<portnumber>", "6780", "Highest port to try and used for dyanmic ports", S },
    { "cyphesis", "unixport", "<filename>", DEFAULT_CLIENT_SOCKET, "Local listen socket for admin connections", S|C|M },
    { "cyphesis", "restricted", "true|false", "false", "Flag to control restricted mode", S },
    { "cyphesis", "usemetaserver", "true|false", "true", "Flag to control registration with the metaserver", S },
    { "cyphesis", "usedatabase", "true|false", "true", "Flag to control whether to use a database for persistent storage", S },
    { "cyphesis", "metaserver", "<hostname>", "metaserver.worldforge.org", "Hostname to use as the metaserver", S },
    { "cyphesis", "daemon", "true|false", "false", "Flag to control running the server in daemon mode", S },
    { "cyphesis", "nice", "<level>", "1", "Reduce the priority level of the server", S },
    { "cyphesis", "useaiclient", "true|false", "false", "Flag to control whether AI is to be driven by a client", S },
    { "cyphesis", "dbserver", "<hostname>", "", "Hostname for the PostgreSQL RDBMS", S|D },
    { "cyphesis", "dbname", "<name>", "\"cyphesis\"", "Name of the database to use", S|D },
    { "cyphesis", "dbuser", "<dbusername>", "<username>", "Database user name for access", S|D },
    { "cyphesis", "dbpasswd", "<dbusername>", "", "Database password for access", S|D },
    { "client", "package", "<package_name>", "define_world", "Python package which contains the world initialisation code", C },
    { "client", "function", "<function_name>", "default", "Python function to initialise the world", C },
    { "client", "serverhost", "<hostname>", "localhost", "Hostname of the server to connect to", S|C|M },
    { "client", "account", "<username>", "admin", "Account name to use to authenticate to the server", S|C },
    { "client", "password", "<password>", "", "Password to use to authenticate to the server", S|C },
    { "client", "useslave", "true|false", "false", "Flag to control connecting to an AI slave server, not master world server" , S|M },
    { "slave", "tcpport", "<portnumber>", "6768", "Network listen port for client connections to the AI slave server", M },
    { "slave", "unixport", "<filename>", DEFAULT_SLAVE_SOCKET, "Local listen socket for admin connections to the AI slave server", M },
    { "slave", "server", "<hostname>", "localhost", "Master server to connect the slave to", M },
    { 0, 0, 0, 0 }
};

static int check_tmp_path(const std::string & dir)
{
    std::string tmp_directory = dir + "/tmp";
    struct stat tmp_stat;

    if (::stat(tmp_directory.c_str(), &tmp_stat) != 0) {
        return -1;
    }

    if (!S_ISDIR(tmp_stat.st_mode)) {
        return -1;
    }

    if (::access(tmp_directory.c_str(), W_OK) != 0) {
        return -1;
    }

    return 0;
}

template <typename T>
int readConfigItem(const std::string & section, const std::string & key, T & storage)
{
    if (global_conf->findItem(section, key)) {
        storage = global_conf->getItem(section, key);
        return 0;
    }
    return -1;
}

template<>
int readConfigItem<std::string>(const std::string & section, const std::string & key, std::string & storage)
{
    if (global_conf->findItem(section, key)) {
        storage = global_conf->getItem(section, key).as_string();
        return 0;
    }
    return -1;
}

typedef std::map<std::string, std::string> OptionHelp;
typedef std::map<std::string, OptionHelp> UsageHelp;

static int check_config(varconf::Config & config,
                        int usage_groups = USAGE_SERVER|
                                           USAGE_CLIENT|
                                           USAGE_CYCMD|
                                           USAGE_DBASE)
{
    UsageHelp usage_help;

    const usage_data * ud = &usage[0];
    for (; ud->section != 0; ++ud) {
        if ((ud->flags & usage_groups) == 0) {
            continue;
        }
        usage_help[ud->section].insert(std::make_pair(ud->option, ud->description));
    }
    
    UsageHelp::const_iterator I = usage_help.begin();
    UsageHelp::const_iterator Iend = usage_help.end();
    for (; I != Iend; ++I) {
        const std::string & section_name = I->first;
        const OptionHelp & section_help = I->second;
        const varconf::sec_map & section = config.getSection(section_name);

        varconf::sec_map::const_iterator J = section.begin();
        varconf::sec_map::const_iterator Jend = section.end();
        for (; J != Jend; ++J) {
            const std::string & option_name = J->first;
            if (section_help.find(J->first) == section_help.end()) {
                log(WARNING, String::compose("Invalid option -- %1:%2",
                                             section_name, option_name));
            }
        }
    }
    return 0;
}

void readInstanceConfiguration(const std::string & section);

int loadConfig(int argc, char ** argv, int usage)
{
    global_conf = varconf::Config::inst();

    global_conf->setParameterLookup('h', "help");
    global_conf->setParameterLookup('?', "help");

    global_conf->setParameterLookup('v', "version");

    // Check the commmand line config doesn't contain any unknown or
    // inappropriate options.
    varconf::Config test_cmdline;
    test_cmdline.getCmdline(argc, argv);
    check_config(test_cmdline, usage);

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
    readConfigItem("cyphesis", "confdir", etc_directory);

    // Load up the rest of the system config file, and then ensure that
    // settings are overridden in the users config file, and the command line
    bool main_config = global_conf->readFromFile(etc_directory +
                                                 "/cyphesis/cyphesis.vconf",
                                                 varconf::GLOBAL);
    if (!main_config) {
        log(ERROR, String::compose("Unable to read main config file \"%1\"",
                                      etc_directory +
                                      "/cyphesis/cyphesis.vconf"));
        if (home_dir_config) {
            log(INFO, "Try removing .cyphesis.vconf from your home directory as it may specify an invalid installation directory, and then restart cyphesis.");
        } else {
            log(INFO, "Please ensure that cyphesis has been installed correctly.");
        }
    }
    if (home_dir_config) {
        global_conf->readFromFile(std::string(home) + "/.cyphesis.vconf");
    }

    int optind = global_conf->getCmdline(argc, argv);

    check_config(*global_conf);

    // Write out any changes that have been overriden at user scope. It
    // may be a good idea to do this at shutdown.
    if (home != NULL) {
        global_conf->writeToFile(std::string(home) + "/.cyphesis.vconf", varconf::USER);
    }

    assert(optind > 0);

    readConfigItem("cyphesis", "instance", instance);

    readConfigItem("cyphesis", "dynamic_port_start", dynamic_port_start);
    readConfigItem("cyphesis", "dynamic_port_end", dynamic_port_end);

    readInstanceConfiguration(instance);

    return optind;
}

void readInstanceConfiguration(const std::string & section)
{
    // Config is now loaded. Now set the values of some globals.

    readConfigItem(section, "directory", share_directory);

    readConfigItem(section, "confdir", etc_directory);

    readConfigItem(section, "vardir", var_directory);

    readConfigItem(section, "daemon", daemon_flag);

    if (readConfigItem(section, "tcpport", client_port_num) != 0) {
        if (section != DEFAULT_INSTANCE) {
            client_port_num = -1;
        }
    }

    if (readConfigItem(section, "unixport", client_socket_name) != 0) {
        client_socket_name = String::compose("cyphesis_%1.sock", section);
    }

    readConfigItem("slave", "tcpport", slave_port_num);

    readConfigItem("slave", "unixport", slave_socket_name);

    readConfigItem("game", "player_vs_player", pvp_flag);

    readConfigItem("game", "player_vs_player_offline", pvp_offl_flag);

    // Load up the ruleset.
    if (readConfigItem(section, "ruleset", ruleset)) {
        log(ERROR, String::compose("No ruleset specified in config. "
                                   "Using \"%1\" rules.", DEFAULT_RULESET));
    }

    if (check_tmp_path(var_directory) != 0) {
        if (var_directory != "/usr/var") {
            // Binreloc enabled builds installed system wide have localstatedir
            // set to something that is never writable, so must always fall
            // back to /var/tmp, so we should not display the message.
            log(WARNING,
                String::compose("No temporary directory found at \"%1/tmp\"",
                                var_directory));
        }
        if (check_tmp_path(FALLBACK_LOCALSTATEDIR) != 0) {
            log(CRITICAL, String::compose("No temporary directory available "
                                          "at \"%1/tmp\" or \"%2/tmp\".",
                                          var_directory,
                                          FALLBACK_LOCALSTATEDIR));
        } else {
            if (var_directory != "/usr/var") {
                log(NOTICE,
                    String::compose("Using \"%1/tmp\" as temporary directory",
                                    FALLBACK_LOCALSTATEDIR));
            }
            var_directory = FALLBACK_LOCALSTATEDIR;
        }
    }

}

void reportVersion(const char * prgname)
{
    std::cout << prgname << " (cyphesis) " << consts::version
              << " (WorldForge)" << std::endl << std::flush;
}

void showUsage(const char * prgname, int usage_flags, const char * extras)
{
    std::cout << "Usage: " << prgname << " [options]";
    if (extras != 0) {
        std::cout << " " << extras;
    }
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    
    size_t column_width = 0;

    const usage_data * ud = &usage[0];
    for (; ud->section != 0; ++ud) {
        column_width = std::max(column_width, strlen(ud->section) + strlen(ud->option) + strlen(ud->value) + 2);
    }

    std::cout << "  --help, -h        Display this information"
              << std::endl;

    std::cout << "  --version, -v     Display the version information and exit"
              << std::endl << std::endl;

    ud = &usage[0];
    for (; ud->section != 0; ++ud) {
        if ((ud->flags & usage_flags) == 0) {
            continue;
        }
        std::cout << "  --" << ud->section << ":" << ud->option;
        if (ud->value != 0 && strlen(ud->value) != 0) {
            std::cout << "=" << ud->value;
        }
        if (ud->dflt != 0 && strlen(ud->dflt) != 0) {
            size_t len = strlen(ud->section) + 1 + strlen(ud->option);
            if (ud->value != 0 && strlen(ud->value) != 0) {
                len += (strlen(ud->value) + 1);
            }
            std::cout << std::string(column_width - len + 2, ' ')
                      << "= " << ud->dflt;
        }
        std::cout << std::endl;
        std::cout << "      " << ud->description << std::endl;
    }
    std::cout << std::flush;
}
