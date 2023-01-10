// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "CommMDNSPublisher.h"

#include "common/id.h"
#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/Inheritance.h"
#include "common/compose.hpp"
#include "common/system.h"
#include "common/sockets.h"
#include "common/utils.h"
#include "common/serialno.h"
#include "common/SystemTime.h"

#include <varconf/config.h>

#include <sigc++/functors/mem_fun.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sstream>

static const bool debug_flag = false;

INT_OPTION(http_port_num, 6780, CYPHESIS, "httpport",
           "Network listen port for http connection to the server");

int main(int argc, char ** argv)
{
    if (security_init() != 0) {
        log(CRITICAL, "Security initialisation Error. Exiting.");
        return EXIT_SECURITY_ERROR;
    }

    if (security_check() != SECURITY_OKAY) {
        log(CRITICAL, "Security check error. Exiting.");
        return EXIT_SECURITY_ERROR;
    }

    interactive_signals();

    int config_status = loadConfig(argc, argv, USAGE_SERVER);
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            std::cout << argv[0] << " (cyphesis) " << consts::version
                      << " (Cyphesis build: " << consts::buildId << ")"
                      << std::endl << std::flush;

            return 0;
        } else if (config_status == CONFIG_HELP) {
            showUsage(argv[0], USAGE_SERVER);
            return 0;
        } else if (config_status != CONFIG_ERROR) {
            log(ERROR, "Unknown error reading configuration.");
        }
        // Fatal error loading config file.
        return EXIT_CONFIG_ERROR;
    }

    if (daemon_flag) {
        int pid = daemonise();
        if (pid == -1) {
            return EXIT_FORK_ERROR;
        } else if (pid > 0) {
            return EXIT_SUCCESS;
        }
    }

    readConfigItem(instance, "usedatabase", database_flag);

    // If we are a daemon logging to syslog, we need to set it up.
    initLogger();

    Inheritance::instance();

    SystemTime time;
    time.update();

    // This ID is currently generated every time, but should perhaps be
    // persistent in future.
    std::string server_id, lobby_id;
    long int_id, lobby_int_id;

    if (((int_id = newId(server_id)) < 0) ||
        ((lobby_int_id = newId(lobby_id)) < 0)) {
        log(CRITICAL, "Unable to get server IDs from Database");
        return EXIT_DATABASE_ERROR;
    }

    // Create the core server object, which stores central data,
    // and track objects
    //ServerRouting * server = new ServerRouting(*world, ruleset_name,
                                               //server_name,
                                               //server_id, int_id,
                                               //lobby_id, lobby_int_id);

    // Create commserver instance that will handle connections from clients.
    // The commserver will create the other server related objects, and the
    // world object pair (World + WorldRouter), and initialise the admin
    // account. The primary ruleset name is passed in so it
    // can be stored and queried by clients.
    CommServer * commServer = new CommServer;

    if (commServer->setup() != 0) {
        log(CRITICAL, "Internal error setting up network infrastructure");
        return EXIT_SOCKET_ERROR;
    }

    CommTCPListener * httpListener = new CommTCPListener(*commServer,
          *new CommHttpClientFactory());
    if (httpListener->setup(http_port_num) != 0) {
        log(ERROR, String::compose("Could not create http listen socket on "
                                   "port %1.", http_port_num));
        delete httpListener;
    } else {
        commServer->addSocket(httpListener);
    }

    log(INFO, "Running");
    logEvent(START, "- - - Standalone server startup");

    // Inform things that want to know that we are running.
    running();

    // Loop until the exit flag is set. The exit flag can be set anywhere in
    // the code easily.
    while (!exit_flag) {
        try {
            time.update();
            commServer->idle(time, false);
            commServer->poll(false);
        }
        catch (...) {
            // It is hoped that commonly thrown exception, particularly
            // exceptions that can be caused  by external influences
            // should be caught close to where they are thrown. If
            // an exception makes it here then it should be debugged.
            log(ERROR, "Exception caught in main()");
        }
    }
    // exit flag has been set so we close down the databases, and indicate
    // to the metaserver (if we are using one) that this server is going down.
    // It is assumed that any preparation for the shutdown that is required
    // by the game has been done before exit flag was set.
    log(NOTICE, "Performing clean shutdown...");

    delete commServer;

    Inheritance::clear();

    delete global_conf;

    log(INFO, "Clean shutdown complete.");
    logEvent(STOP, "- - - Standalone server shutdown");
    return 0;
}
