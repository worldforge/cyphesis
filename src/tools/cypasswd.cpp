// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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


/// \page cypasswd_index
///
/// \section Introduction
///
/// cypasswd is an interactive commandline tool to manage account data in
/// the database. For information on the usage, please see the unix
/// manual page. The manual page is generated from docbook sources, so can
/// also be converted into other formats.

#ifdef HAVE_CONFIG_H
#endif

#include "common/Storage.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/system.h"
#include "server/Persistence.h"
#include "DatabaseCreation.h"

#include <varconf/config.h>

#include <cstring>
#include <common/DatabaseSQLite.h>

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#include <unistd.h>
#endif

// This is the currently very basic password management tool, which can
// be used to control the passwords of accounts in the main servers account
// database. This is the only way to create accounts on a server in
// restricted mode.

// TODO: Make sure the rest of the Object is preserved, rather than just
//       blatting it with a new Object.

using Atlas::Message::MapType;

#define ADD 0
#define SET 1
#define DEL 2
#define MOD 3

#define ADMIN  0
#define PLAYER 1
#define SERVER 2

namespace {
    void usage(std::ostream& stream, char* n, bool verbose = false)
    {
        stream << "usage: " << n << std::endl;
        stream << "       " << n << " [ -a | -d ] [ -s | -r | -p ] account" << std::endl;
        stream << "       " << n << " -h" << std::endl;

        if (!verbose) {
            stream << std::flush;
            return;
        }

        stream << std::endl;
        stream << "Help options" << std::endl;
        stream << "  -h                          Display this help" << std::endl;
        stream << std::endl;
        stream << "Managing accounts" << std::endl;
        stream << "  -a                          Add a new account" << std::endl;
        stream << "  -d                          Delete an account" << std::endl;
        stream << "  -s                          Make server account" << std::endl;
        stream << "  -r                          Make admin account" << std::endl;
        stream << "  -p                          Make player account (default)"
               << std::endl;
    }
}

static int get_password(const std::string & acname,
                        std::string & password,
                        std::string & password2)
{
    // TODO Catch signals, and restore terminal
#ifdef HAVE_TERMIOS_H
    termios termios_old, termios_new;

    tcgetattr( STDIN_FILENO, &termios_old );
    termios_new = termios_old;
    termios_new.c_lflag &= ~(ICANON|ECHO);
    tcsetattr( STDIN_FILENO, TCSADRAIN, &termios_new );
#endif

    std::cout << "New " << acname << " password: " << std::flush;
    std::cin >> password;
    std::cout << std::endl << "Retype " << acname << " password: " << std::flush;
    std::cin >> password2;
    std::cout << std::endl << std::flush;

#ifdef HAVE_TERMIOS_H
    tcsetattr( STDIN_FILENO, TCSADRAIN, &termios_old );
#endif

    return 0;
}
//
//static std::unique_ptr<Database> createDatabase()
//{
//    std::string databaseBackend;
//    readConfigItem(instance, "database", databaseBackend);
//
//    if (databaseBackend == "postgres") {
//#ifdef POSTGRES_FOUND
//        return std::make_unique<DatabasePostgres>();
//#else
//        log(ERROR, "Database specified as 'postgres', but this server is not built with Postgres SQL support.");
//        throw std::runtime_error("Database specified as 'postgres', but this server is not built with Postgres SQL support.");
//#endif
//    } else {
//        return std::make_unique<DatabaseSQLite>();
//    }
//}

STRING_OPTION(add, "", "", "add", "Add a new account")
STRING_OPTION(del, "", "", "del", "Delete an account")
STRING_OPTION(player, "", "", "player", "Make server account")
STRING_OPTION(server, "", "", "server", "Make admin account")
STRING_OPTION(admin, "", "", "admin", "Make player account (default)")

int main(int argc, char ** argv)
{
    varconf::Config * conf = varconf::Config::inst();

    conf->setParameterLookup('a', "add");
    conf->setParameterLookup('d', "del");
    conf->setParameterLookup('p', "player");
    conf->setParameterLookup('s', "server");
    conf->setParameterLookup('r', "admin");

    int config_status = loadConfig(argc, argv, USAGE_DBASE);
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            reportVersion(argv[0]);
            return 0;
        } else if (config_status == CONFIG_HELP) {
            usage(std::cout, argv[0], true);
            return 0;
        } else if (config_status != CONFIG_ERROR) {
            std::cerr << "Unknown error reading configuration." << std::endl;
        }
        // Fatal error loading config file
        return 1;
    }

    int extra_arg_count = argc - config_status;

    std::string acname;
    int actype = PLAYER;
    int action = SET;

    if (global_conf->findItem("", "add")) {
        action = ADD;
    }

    if (global_conf->findItem("", "del")) {
        if (action != SET) {
            usage(std::cerr, argv[0]);
            return 1;
        }
        action = DEL;
    }

    if (global_conf->findItem("", "player")) {
        if (action == SET) {
            action = MOD;
        }
        actype = PLAYER;
    }

    if (global_conf->findItem("", "server")) {
        if (action == SET) {
            action = MOD;
        }
        actype = SERVER;
    }

    if (global_conf->findItem("", "admin")) {
        if (action == SET) {
            action = MOD;
        }
        actype = ADMIN;
    }

    if (extra_arg_count == 0) {
        if (action != SET) {
            usage(std::cerr, argv[0]);
            return 1;
        }
        acname = "admin";
        actype = ADMIN;
    } else if (extra_arg_count == 1) {
        acname = argv[config_status];
    } else {
        usage(std::cerr, argv[0]);
        return 1;
    }

    if (security_init() != 0) {
        log(CRITICAL, "Security initialisation Error. Exiting.");
        return EXIT_SECURITY_ERROR;
    }

    try {
        auto database = createDatabase();
        Storage db(*database);

    // MapType data;

    // bool res = db->getAccount("admin", data);

    // if (!res) {
        // std::cout << "Admin account does not yet exist" << std::endl << std::flush;
        // acname = "admin";
        // action = ADD;
    // }
        if (action != ADD) {
            MapType o;
            int res = db.getAccount(acname, o);
            if (res != 0) {
                std::cout << "Account '" << acname << "' does not yet exist" << std::endl << std::flush;
                return 1;
            }
        }
        if (action == DEL) {
            int res = db.delAccount(acname);
            if (res == 0) {
                std::cout << "Account '" << acname << "' removed." << std::endl << std::flush;
            }
            return 0;
        }

        MapType amap;
        if (action == MOD) {
            std::string account_type("player");
            if (actype == SERVER) {
                account_type = "server";
            } else if (actype == ADMIN) {
                account_type = "admin";
            }
            amap["type"] = account_type;
            std::cout << "Changing '" << acname << "' to a " << account_type
                      << " account" << std::endl << std::flush;
        } else {
            std::string password, password2;

            get_password(acname, password, password2);

            if (password != password2) {
                std::cout << "Passwords did not match. Account database unchanged."
                          << std::endl << std::flush;
                return 1;
            }

            amap["password"] = password;
        }

        int res;
        if (action == ADD) {
            amap["username"] = acname;
            if (actype == SERVER) {
                std::cout << "Creating server account" << std::endl << std::flush;
                amap["type"] = "server";
            }
            res = db.putAccount(amap);
        } else {
            res = db.modAccount(amap, acname);
        }
        if (res == 0) {
            std::cout << "Account changed." << std::endl << std::flush;
            return 0;
        }
        return 1;
    } catch (const std::runtime_error& ex) {
        std::cout << "There was an error: " << ex.what() << std::endl;
        return 1;
    }
}
