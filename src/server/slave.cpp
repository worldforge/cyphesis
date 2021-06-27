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


#include "CommMaster.h"
#include "Master.h"
#include "ServerRouting.h"
#include "EntityBuilder.h"
#include "Persistence.h"
#include "rules/simulation/WorldRouter.h"

#include "pythonbase/Python_API.h"
#include "rules/ai/MindFactory.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/Inheritance.h"
#include "common/system.h"
#include "common/compose.hpp"

#include <varconf/config.h>

class SlaveClientConnection;

static const bool debug_flag = false;

int main(int argc, char ** argv)
{
    interactive_signals();

    if (loadConfig(argc, argv, true) < 0) {
        // Fatal error loading config file
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

    // If we are a daemon logging to syslog, we need to set it up.
    initLogger();

    // Initialise the persistance subsystem. If we have been built with
    // database support, this will open the various databases used to
    // store server data.
    if (database_flag) {
        Persistence * p = Persistence::instance();
        int dbstatus = p->init();
        if (dbstatus < 0) {
            database_flag = false;
            log(ERROR, "Error opening database. Database disabled.");
            if (dbstatus == DATABASE_TABERR) {
                log(INFO, "Database connection established, "
                          "but unable to create required tables.");
                log(INFO, "Please ensure that any obsolete database "
                          "tables have been removed.");
            } else {
                log(INFO, "Unable to connect to the RDBMS.");
                log(INFO, "Please ensure that the RDBMS is running, "
                          "the cyphesis database exists and is accessible "
                          "to the user running cyphesis.");
            }
            log(INFO, String::compose("To disable this message please run:\n\n"
                                      "    cyconfig --%1:usedatabase=false\n\n"
                                      "to permanently disable database usage.",
                                      instance));
        }
    }

    // If the restricted flag is set in the config file, then we
    // don't allow connecting users to create accounts. Accounts must
    // be created manually by the server administrator.
    if (readConfigItem("cyphesis","restricted", restricted_flag) == 0) {
        if (restricted_flag) {
            log(INFO, "Setting restricted mode.");
        }
    }

    readConfigItem("cyphesis","inittime", timeoffset);

    std::string mserver("metaserver.worldforge.org");
    readConfigItem("cyphesis", "metaserver", mserver);

    std::string serverName;
    if (readConfigItem("cyphesis","servername", serverName) != 0) {
        serverName = get_hostname();
    }

    std::string serverHostname("localhost");
    readConfigItem("slave","server", serverHostname);
    
    // Start up the python subsystem.
    init_python_api();

    { // scope for CommServer

        EntityBuilder entityBuilder;
    // Create commserver instance that will handle connections from clients.
    // The commserver will create the other server related objects, and the
    // world object pair (World + WorldRouter), and initialise the admin
    // account. The primary ruleset name is passed in so it
    // can be stored and queried by clients.
    WorldRouter world;

    // This ID is currently generated every time, but should perhaps be
    // persistent in future.
    std::string server_id, lobby_id;
    long int_id, lobby_int_id;

    if (((int_id = newId(server_id)) < 0) ||
        ((lobby_int_id = newId(lobby_id)) < 0)) {
        log(CRITICAL, "Unable to get server IDs from Database");
        return EXIT_DATABASE_ERROR;
    }

    ServerRouting server(world, ruleset, serverName,
                         server_id, int_id,
                         lobby_id, lobby_int_id);

    CommServer commServer(server);

    // This is where we should restore the database, before
    // the listen sockets are open. Unlike earlier code, we are
    // attempting to construct the internal state from the database,
    // not creating a new world using the contents of the database as a
    // template

    CommUnixListener * listener = new CommUnixListener(commServer,
          *new CommClientFactory<SlaveClientConnection>());
    if (listener->setup(slave_socket_name) != 0) {
        log(ERROR, "Could not create listen socket. Init failed.");
        return EXIT_SOCKET_ERROR;
    }
    commServer.addSocket(listener);

    std::string master_id;
    if (newId(master_id) < 0) {
        log(CRITICAL, "Unable to get master ID from Database");
        return EXIT_DATABASE_ERROR;
    }

    CommMaster * master = new CommMaster(commServer);
    if (master->connect(serverHostname) != 0) {
        log(ERROR, "Could not connect to master. Init failed.");
        return EXIT_SOCKET_ERROR;
    }
    master->setup(new Master(*master, commServer.m_server, master_id));
    commServer.addSocket(master);

    log(INFO, "Running");

    // Inform things that want to know that we are running.
    running();

    // Loop until the exit flag is set. The exit flag can be set anywhere in
    // the code easily.
    while (!exit_flag) {
        try {
            commServer.poll();
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

    } // close scope of CommServer, which cause the destruction of the
      // server and world objects, and the entire world contents

    Persistence::instance().shutdown();

    entityBuilder.flushFactories();
    MindFactory::del();

    Inheritance::clear();

    // Shutdown the python interpretter. This frees lots of memory, and if
    // the malloc heap is in any way corrupt, a segfault is likely to
    // occur at this point. Previous occassions where pointers have been
    // deleted twice elsewhere in the code, have resulted in a segfault
    // at this point. AlRiddoch 10th November 2001
    shutdown_python_api();

    delete global_conf;

    log(INFO, "Clean shutdown complete.");
    return 0;
}
