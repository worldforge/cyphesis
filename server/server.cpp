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

// $Id: server.cpp,v 1.151 2007-12-04 19:18:17 alriddoch Exp $

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "CommServer.h"
#include "CommListener.h"
#include "CommPeerListener.h"
#include "CommUnixListener.h"
#include "CommPSQLSocket.h"
#include "CommMetaClient.h"
#include "CommMDNSPublisher.h"
#include "ServerRouting.h"
#include "EntityFactory.h"
#include "Persistance.h"
#include "Restoration.h"
#include "WorldRouter.h"
#include "Admin.h"

#include "rulesets/Python_API.h"
#include "rulesets/MindFactory.h"
#include "rulesets/Entity.h"

#include "common/id.h"
#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/inheritance.h"
#include "common/compose.hpp"
#include "common/system.h"
#include "common/nls.h"

#include <varconf/config.h>

#include <sstream>

static const bool debug_flag = false;

int main(int argc, char ** argv)
{
    if (security_check() != SECURITY_OKAY) {
        log(CRITICAL, "Security Error. Exiting.");
        return EXIT_SECURITY_ERROR;
    }

    interactive_signals();

    int config_status = loadConfig(argc, argv, USAGE_SERVER);
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            std::cout << argv[0] << " (cyphesis) " << consts::version
                      << " (Cyphesis build " << consts::buildId << ")"
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

    // Initialise the persistance subsystem. If we have been built with
    // database support, this will open the various databases used to
    // store server data.
    if (database_flag) {
        int dbstatus = Persistance::init();
        if (dbstatus < 0) {
            database_flag = false;
            log(ERROR, "Error opening database. Database disabled.");
            if (dbstatus == -2) {
                log(INFO, "Database connection established, but unable to create required tables.");
                log(INFO, "Please ensure that any obsolete database tables have been removed.");
            } else {
                log(INFO, "Unable to connect to the RDBMS.");
                log(INFO, "Please ensure that the RDBMS is running, the cyphesis database exists and is accessible to the user running cyphesis.");
            }
            log(INFO, "To disable this message please run:\n\n    cyconfig --cyphesis:usedatabase=false\n\nto permanently disable database usage.");
        }
    }

    // If the restricted flag is set in the config file, then we
    // don't allow connecting users to create accounts. Accounts must
    // be created manually by the server administrator.
    if (readConfigItem(instance, "restricted", restricted_flag) == 0) {
        if (restricted_flag) {
            log(INFO, "Setting restricted mode.");
        }
    }

    readConfigItem(instance, "inittime", timeoffset);

    bool useMetaserver = false;
    readConfigItem(instance, "usemetaserver", useMetaserver);

    std::string mserver("metaserver.worldforge.org");
    readConfigItem(instance, "metaserver", mserver);

    std::string server_name;
    if (readConfigItem(instance, "servername", server_name) != 0) {
        if (instance == "cyphesis") {
            server_name = get_hostname();
        } else {
            server_name = instance;
        }
    }

    int nice = 0;
    readConfigItem(instance, "nice", nice);
    
    // Start up the python subsystem.
    init_python_api();

    { // scope for CommServer

    // Create commserver instance that will handle connections from clients.
    // The commserver will create the other server related objects, and the
    // world object pair (World + WorldRouter), and initialise the admin
    // account. The primary ruleset name is passed in so it
    // can be stored and queried by clients.
    Inheritance::instance();

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

    ServerRouting server(world, ruleset, server_name,
                         server_id, int_id,
                         lobby_id, lobby_int_id);

    CommServer commServer(server);

    // This is where we should restore the database, before
    // the listen sockets are open. Unlike earlier code, we are
    // attempting to construct the internal state from the database,
    // not creating a new world using the contents of the database as a
    // template

    if (database_flag) {
        log(INFO, _("Restoring world from database..."));

        Restoration restore(server);
        if (restore.read() == 1) {
            debug(std::cout << "Bootstrapping world" << std::endl << std::flush;);
            EntityFactory::instance()->initWorld();
            assert(!world.m_gameWorld.m_location.m_pos.isValid());
            assert(!world.m_gameWorld.m_location.m_orientation.isValid());
        } else {
            // The world should not have POS or ORIENTATION, but will have
            // picked one up when restored from the database.
            world.m_gameWorld.m_location.m_pos = Point3D(0,0,0);
            world.m_gameWorld.m_location.m_pos.setValid(false);
            world.m_gameWorld.m_location.m_orientation = Quaternion();
            // world.m_gameWorld.m_location.m_orientation.setValid(false);
            
        }

        log(INFO, _("Restored world."));

        CommPSQLSocket * dbsocket = new CommPSQLSocket(commServer,
                                        Persistance::instance()->m_connection);
        commServer.addSocket(dbsocket);
        commServer.addIdle(dbsocket);
    } else {
        std::string adminId;
        long intId = newId(adminId);
        assert(intId >= 0);

        Admin * admin = new Admin(0, "admin", "BAD_HASH", adminId, intId);
        server.addAccount(admin);
    }

    CommListener * listener = new CommListener(commServer);
    if (client_port_num < 0) {
        client_port_num = dynamic_port_start;
        for (; client_port_num <= dynamic_port_end; client_port_num++) {
            if (listener->setup(client_port_num) == 0) {
                break;
            }
        }
        if (client_port_num > dynamic_port_end) {
            log(ERROR, "Could not find free client listen socket. Init failed.");
            return EXIT_SOCKET_ERROR;
        }
    } else {
        if (listener->setup(client_port_num) != 0) {
            log(ERROR, "Could not create client listen socket. Init failed.");
            return EXIT_SOCKET_ERROR;
        }
    }
    commServer.addSocket(listener);

    CommPeerListener * peerListener = new CommPeerListener(commServer);
    if (peerListener->setup(peer_port_num) != 0) {
        log(ERROR, "Could not create peer listen socket.");
        delete peerListener;
    } else {
        commServer.addSocket(peerListener);
    }

#ifdef HAVE_SYS_UN_H
    CommUnixListener * localListener = new CommUnixListener(commServer);
    if (localListener->setup(client_socket_name) != 0) {
        log(ERROR, String::compose("Could not create local listen socket "
                                   "with address \"%1\"",
                                   localListener->getPath()));
        delete localListener;
    } else {
        commServer.addSocket(localListener);
    }
#endif

    if (useMetaserver) {
        CommMetaClient * cmc = new CommMetaClient(commServer);
        if (cmc->setup(mserver) == 0) {
            commServer.addSocket(cmc);
            commServer.addIdle(cmc);
        } else {
            log(ERROR, "Error creating metaserver comm channel.");
            delete cmc;
        }
    }

#if defined(HAVE_LIBHOWL) || defined(HAVE_AVAHI)

    CommMDNSPublisher * cmdns = new CommMDNSPublisher(commServer);
    if (cmdns->setup() == 0) {
        commServer.addSocket(cmdns);
        commServer.addIdle(cmdns);
    } else {
        log(ERROR, "Unable to register service with MDNS daemon.");
        delete cmdns;
    }

#endif // defined(HAVE_LIBHOWL)

    log(INFO, "Running");
    logEvent(START, "- - - Standalone server startup");

    // Inform things that want to know that we are running.
    running();

    // Reduce our system priority to make it easier to debug a runaway
    // server.
    if (nice != 0) {
        reduce_priority(nice);
    }

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

    } // close scope of CommServer, WorldRouter, and ServerRouting, which
      // cause the destruction of the server and world objects, and the entire
      // world contents

    Persistance::shutdown();

    EntityFactory::instance()->flushFactories();
    EntityFactory::del();
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
    logEvent(STOP, "- - - Standalone server shutdown");
    return 0;
}
