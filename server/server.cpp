// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

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

#include "common/id.h"
#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/inheritance.h"
#include "common/system.h"
#include "common/nls.h"

#include <varconf/Config.h>

#include <sstream>

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
    if (consts::enable_database) {
        int dbstatus = Persistance::init();
        if (dbstatus < 0) {
            log(CRITICAL, _("Critical error opening databases. Init failed."));
            if (dbstatus == -2) {
                log(INFO, "Database connection established, but unable to create required tables.");
                log(INFO, "Please ensure that any obsolete database tables have been removed.");
            } else {
                log(INFO, "Unable to connect to the RDBMS.");
                log(INFO, "Please ensure that the RDBMS is running, the cyphesis database exists and is accessible to the user running cyphesis.");
            }
            return EXIT_DATABASE_ERROR;
        }

        // If the restricted flag is set in the config file, then we
        // don't allow connecting users to create accounts. Accounts must
        // be created manually by the server administrator.
        if (global_conf->findItem("cyphesis", "restricted")) {
            restricted_flag = global_conf->getItem("cyphesis","restricted");
            if (restricted_flag) {
                log(INFO, "Setting restricted mode.");
            }
        }
    }

    if (global_conf->findItem("cyphesis", "inittime")) {
        timeoffset = global_conf->getItem("cyphesis","inittime");
    }

    bool useMetaserver = false;
    if (global_conf->findItem("cyphesis", "usemetaserver")) {
        useMetaserver = global_conf->getItem("cyphesis","usemetaserver");
    }

    std::string mserver("metaserver.worldforge.org");
    if (global_conf->findItem("cyphesis", "metaserver")) {
        mserver = global_conf->getItem("cyphesis", "metaserver").as_string();
    }

    std::string serverName;
    if (global_conf->findItem("cyphesis", "servername")) {
        serverName = global_conf->getItem("cyphesis","servername").as_string();
    } else {
        serverName = get_hostname();
    }
    
    // Start up the python subsystem.
    init_python_api();

    { // scope for CommServer

    // Create commserver instance that will handle connections from clients.
    // The commserver will create the other server related objects, and the
    // world object pair (World + WorldRouter), and initialise the admin
    // account. The primary ruleset name is passed in so it
    // can be stored and queried by clients.
    WorldRouter world;

    ServerRouting server(world, rulesets.front(), serverName);

    CommServer commServer(server);

    // This is where we should restore the database, before
    // the listen sockets are open. Unlike earlier code, we are
    // attempting to construct the internal state from the database,
    // not creating a new world using the contents of the database as a
    // template

    if (consts::enable_database) {
        log(INFO, _("Restoring world from database..."));

        Restoration restore(server);
        if (restore.read() == 1) {
            debug(std::cout << "Bootstrapping world" << std::endl << std::flush;);
            EntityFactory::instance()->initWorld();
        }

        log(INFO, _(" world restored"));

        CommPSQLSocket * dbsocket = new CommPSQLSocket(commServer,
                                        Persistance::instance()->m_connection);
        commServer.addSocket(dbsocket);
        commServer.addIdle(dbsocket);
    } else {
        std::string adminId;
        newId(adminId);
        assert(!adminId.empty());
        Admin * admin = new Admin(0, "admin", "BAD_HASH", adminId);
        server.addAccount(admin);
    }

    CommListener * listener = new CommListener(commServer);
    if (listener->setup(client_port_num) != 0) {
        log(ERROR, "Could not create client listen socket. Init failed.");
        return EXIT_SOCKET_ERROR;
    }
    commServer.addSocket(listener);

    CommPeerListener * peerListener = new CommPeerListener(commServer);
    if (peerListener->setup(peer_port_num) != 0) {
        log(ERROR, "Could not create peer listen socket.");
        delete peerListener;
    } else {
        commServer.addSocket(peerListener);
    }

    CommUnixListener * localListener = new CommUnixListener(commServer);
    if (localListener->setup(client_socket_name) != 0) {
        std::stringstream str;
        str << "Could not create local listen socket with address \"";
        str << localListener->getPath() << "\".";
        log(ERROR, str.str().c_str());
        delete localListener;
    } else {
        commServer.addSocket(localListener);
    }

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

#if defined(HAVE_LIBHOWL)

    CommMDNSPublisher * cmdns = new CommMDNSPublisher(commServer);
    if (cmdns->setup() == 0) {
        commServer.addSocket(cmdns);
    } else {
        log(ERROR, "No MDNS Responder for me.");
        delete cmdns;
    }

#endif // defined(HAVE_LIBHOWL)

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
    return 0;
}
