// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommServer.h"
#include "ServerRouting.h"
#include "EntityFactory.h"
#include "Persistance.h"

#include <rulesets/Python_API.h>
#include <rulesets/MindFactory.h>

#include <common/log.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/inheritance.h>
#include <common/system.h>

#include <common/Load.h>

#include <varconf/Config.h>

static const bool debug_flag = false;

int main(int argc, char ** argv)
{
    interactive_signals();

    if (loadConfig(argc, argv, true)) {
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

    initLogger();

    // Initialise the persistance subsystem. If we have been built with
    // database support, this will open the various databases used to
    // store server data.
    bool dbInit = Persistance::init();
    if (!dbInit) {
        log(CRITICAL, "Critical error opening databases. Init failed.");
        log(INFO, "Please ensure that the database tables can be created or accessed by cyphesis.");
        return EXIT_DATABASE_ERROR;
    }

    // If the restricted flag is set in the config file, then we
    // don't allow connecting users to create accounts. Accounts must
    // be created manually by the server administrator.
    if (global_conf->findItem("cyphesis", "restricted")) {
        Persistance::restricted = global_conf->getItem("cyphesis","restricted");
        if (Persistance::restricted) {
            log(INFO, "Running in restricted mode");
        }
    }

    if (global_conf->findItem("cyphesis", "inittime")) {
        timeoffset = global_conf->getItem("cyphesis","inittime");
    }

    bool load_database = false;
    if (global_conf->findItem("cyphesis", "loadonstartup")) {
        load_database = global_conf->getItem("cyphesis","loadonstartup");
    }

    std::string serverName;
    if (global_conf->findItem("cyphesis", "servername")) {
        serverName = global_conf->getItem("cyphesis","servername");
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
    CommServer s(rulesets.front(), serverName);

    if (!s.setup(port_num)) {
        log(ERROR, "Could not create listen socket. Init failed.");
        return EXIT_SOCKET_ERROR;
    }

    if (load_database) {
        Load l(Load::Instantiate());
        l.SetFrom("admin");
        BaseEntity * admin = s.server.getObject("admin");
        if (admin == NULL) {
            log(ERROR, "Admin account not found, world not loaded.");
        } else {
            log(INFO, "Loading world from database...");
            OpVector res = admin->LoadOperation(l);
            // Delete the resulting op
            OpVector::const_iterator I = res.begin();
            for(;I != res.end(); I++) { delete *I; }
            log(INFO, " world loaded");
        }
    }
    log(INFO, "Running");

    running();
    // Loop until the exit flag is set. The exit flag can be set anywhere in
    // the code easily.
    while (!exit_flag) {
        try {
            s.loop();
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

    s.shutdown();

    } // close scope of CommServer, which cause the destruction of the
      // server and world objects, and the entire world contents

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
