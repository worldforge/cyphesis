// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommServer.h"

#include <varconf/Config.h>

#include <iostream>

extern "C" {
    #include <sys/utsname.h>
    #include <mcheck.h>
    #include <signal.h>
    #include <syslog.h>
    #include <fcntl.h>
}

#include <rulesets/EntityFactory.h>
#include <rulesets/Python_API.h>

#include <common/debug.h>
#include <common/globals.h>
#include <common/inheritance.h>

#include <common/Load.h>

#include "ServerRouting_methods.h"
#include "Persistance.h"

using Atlas::Message::Object;

static const bool debug_flag = false;

const std::string get_hostname()
{
    struct utsname host_ident;
    if (uname(&host_ident) != 0) {
        return "UNKNOWN";
    }
    return std::string(host_ident.nodename);
}

extern "C" void signal_received(int signo)
{
    exit_flag = true;
    signal(signo, SIG_IGN);
}

void interactive_signals()
{
    signal(SIGINT, signal_received);
    signal(SIGTERM, signal_received);
    signal(SIGQUIT, signal_received);
}

void daemon_signals()
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, signal_received);
    signal(SIGQUIT, SIG_IGN);
}

int daemonise()
{
    int pid = fork();
    int new_stdio;
    switch (pid) {
        case 0:
            // Child
            // Switch signal behavoir
            daemon_signals();
            // Get rid if stdio
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            // Get rid of controlling tty, and start new session
            setsid();
            // Open /dev/null on the stdio file descriptors to avoid problems
            new_stdio = open("/dev/null", O_RDWR);
            dup2(new_stdio, STDIN_FILENO);
            dup2(new_stdio, STDOUT_FILENO);
            dup2(new_stdio, STDERR_FILENO);
            // Initialise syslog for serious errors
            openlog("WorldForge Cyphesis", LOG_PID, LOG_DAEMON);
            break;
        case -1:
            // Error
            std::cerr << "ERROR: Failed to fork() to go to the background"
                      << std::endl << std::flush;
            break;
        default:
            break;
    }
    return pid;
}

int main(int argc, char ** argv)
{
    mtrace();
    interactive_signals();
    // Initialise the varconf system, and get a pointer to the config database
    global_conf = varconf::Config::inst();

    if (loadConfig(argc, argv)) {
        // Fatal error loading config file
        return 1;
    }

    EntityFactory::instance()->installBaseClasses();

    // Load up the rulesets. Rulesets are hierarchical, and are read in until
    // a file is read in that does not specify its parent ruleset.
    std::string ruleset;
    while (global_conf->findItem("cyphesis", "ruleset")) {
        ruleset = global_conf->getItem("cyphesis", "ruleset");
        global_conf->erase("cyphesis", "ruleset");
        std::cout << "Reading in " << ruleset << std::endl;
        global_conf->readFromFile(share_directory + "/cyphesis/" + ruleset + ".vconf");
        rulesets.push_back(ruleset);
    };

    if (global_conf->findItem("cyphesis", "daemon")) {
        daemon_flag = global_conf->getItem("cyphesis","daemon");
    }

    if (daemon_flag) {
        std::cout << "Going into background" << std::endl << std::flush;
        int pid = daemonise();
        if (pid == -1) {
            exit_flag = true;
        } else if (pid > 0) {
            return 0;
        }
    }

    // Initialise the persistance subsystem. If we have been built with
    // database support, this will open the various databases used to
    // store server data.
    bool dbInit = Persistance::init();
    if (!dbInit) {
        if (daemon_flag) {
            syslog(LOG_ERR, "Critical error opening databases. Init failed.");
        } else {
            std::cerr << "FATAL: Unable to open the account and world databases"
                      << std::endl
                      << "Please ensure that the database tables "
                      << "can be created or accessed by cyphesis."
                      << std::endl << std::flush;
        }
        return 0;
    }

    // If the restricted flag is set in the config file, then we
    // don't allow connecting users to create accounts. Accounts must
    // be created manually by the server administrator.
    if (global_conf->findItem("cyphesis", "restricted")) {
        Persistance::restricted=global_conf->getItem("cyphesis","restricted");
        if (Persistance::restricted) {
            std::cout << "Running in restricted mode" << std::endl;
        }
    }
    // Read the metaserver usage flag from config file.
    bool use_metaserver = true;
    if (global_conf->findItem("cyphesis", "usemetaserver")) {
        use_metaserver = global_conf->getItem("cyphesis","usemetaserver");
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
    
    // Start up the python subsystem. FIXME This needs to sorted into a
    // a way of handling script subsystems more generically.
    init_python_api();

    { // scope for CommServer

    // Create commserver instance that will handle connections from clients.
    // The commserver will create the other server related objects, and the
    // world object pair (World + WorldRouter), and initialise the admin
    // account. The primary ruleset name is passed in so it
    // can be stored and queried by clients.
    CommServer s(rulesets.front(), serverName);
    s.useMetaserver = use_metaserver;
    // Get the port tcp port from the config file, and set up the listen socket
    int port_num = 6767;
    if (global_conf->findItem("cyphesis", "tcpport")) {
        port_num=global_conf->getItem("cyphesis","tcpport");
    }
    if (!s.setup(port_num)) {
        std::cerr << "Could not create listen socket." << std::endl << std::flush;
        return 1;
    }

    if (load_database) {
        Load l(Load::Instantiate());
        l.SetFrom("admin");
        BaseEntity * admin = s.server.getObject("admin");
        if (admin == NULL) {
            std::cout << "CRITICAL: Admin account not found." << std::endl;
        } else {
            if (!daemon_flag) {
                std::cout << "Loading world from database..." << std::flush;
            }
            oplist res = admin->LoadOperation(l);
            // Delete the resulting op
            oplist::iterator I = res.begin();
            for(;I != res.end(); I++) { delete *I; }
            if (!daemon_flag) {
                std::cout << " done" << std::endl;
            }
        }
        // FIXME ? How to send this to admin account ?
    }
    if (!daemon_flag) {
        std::cout << "Running" << std::endl << std::flush;
    }
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
            if (daemon_flag) {
                syslog(LOG_ERR, "Exception caught in main()");
            } else {
                std::cerr << "*********** EMERGENCY ***********" << std::endl;
                std::cerr << "EXCEPTION: Caught in main()" << std::endl;
                std::cerr << "         : Continuing" << std::endl << std::flush;
            }
        }
    }
    // exit flag has been set so we close down the databases, and indicate
    // to the metaserver (if we are using one) that this server is going down.
    // It is assumed that any preparation for the shutdown that is required
    // by the game has been done before exit flag was set.
    if (!daemon_flag) {
        std::cout << "Performing clean shutdown..." << std::endl << std::flush;
    }

    s.metaserverTerminate();

    } // close scope of CommServer

    Persistance::shutdown();

    EntityFactory::instance()->flushFactories();
    EntityFactory::del();

    Inheritance::clear();

    // Shutdown the python interpretter. This frees lots of memory, and if
    // the malloc heap is in any way corrupt, a segfault is likely to
    // occur at this point. Previous occassions where pointers have been
    // deleted twice elsewhere in the code, have resulted in a segfault
    // at this point. AlRiddoch 10th November 2001
    shutdown_python_api();

    delete global_conf;

    if (!daemon_flag) {
        std::cout << "Clean shutdown complete." << std::endl << std::flush;
    }
    muntrace();
    return 0;
}
