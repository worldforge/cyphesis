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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "CommServer.h"
#include "CommTCPListener.h"
#include "CommClientFactory_impl.h"
#include "CommUserClient.h"
#ifdef HAVE_SYS_UN_H
#include "CommAdminClient.h"
#endif
#include "CommHttpClientFactory.h"
#include "CommPythonClientFactory.h"
#include "CommUnixListener.h"
#include "CommPSQLSocket.h"
#include "CommMetaClient.h"
#include "CommMDNSPublisher.h"
#include "Connection.h"
#include "ServerRouting.h"
#include "EntityBuilder.h"
#include "ArithmeticBuilder.h"
#include "Persistence.h"
#include "WorldRouter.h"
#include "Ruleset.h"
#include "StorageManager.h"
#include "IdleConnector.h"
#include "UpdateTester.h"
#include "Admin.h"
#include "TCPListenFactory.h"
#include "TeleportAuthenticator.h"
#include "TrustedConnection.h"

#include "rulesets/BulletDomain.h"
#include "rulesets/Python_API.h"
#include "rulesets/LocatedEntity.h"

#include "common/id.h"
#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/Inheritance.h"
#include "common/compose.hpp"
#include "common/system.h"
#include "common/nls.h"
#include "common/sockets.h"
#include "common/utils.h"
#include "common/serialno.h"
#include "common/SystemTime.h"


#include <varconf/config.h>

#include <sigc++/functors/mem_fun.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <skstream/skaddress.h>

#include <boost/make_shared.hpp>

#include <thread>
#include <cstdlib>
#include <fstream>

using String::compose;

class TrustedConnection;
class Peer;

static const bool debug_flag = false;

INT_OPTION(http_port_num, 6780, CYPHESIS, "httpport",
           "Network listen port for http connection to the server");

BOOL_OPTION(useMetaserver, true, CYPHESIS, "usemetaserver",
            "Flag to control registration with the metaserver");

STRING_OPTION(mserver, "metaserver.worldforge.org", CYPHESIS, "metaserver",
              "Hostname to use as the metaserver");

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

    //Turn on soft exits so we get a chance to persist external clients' thoughts.
    exit_soft_enabled = true;

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

    // Initialise the persistence subsystem. If we have been built with
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
    if (restricted_flag) {
        log(INFO, "Setting restricted mode.");
    }

    readConfigItem(instance, "inittime", timeoffset);

    std::string server_name;
    if (readConfigItem(instance, "servername", server_name) != 0) {
        if (instance == CYPHESIS) {
            server_name = get_hostname();
        } else {
            server_name = instance;
        }
    }

    int nice = 1;
    readConfigItem(instance, "nice", nice);
    
    // Start up the Python subsystem.
    init_python_api(ruleset_name);

    Inheritance::instance();
    new BulletDomain;

    SystemTime time;
    time.update();

    WorldRouter * world = new WorldRouter(time);

    Ruleset::init(ruleset_name);

    TeleportAuthenticator::init();

    StorageManager * store = new StorageManager(*world);

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
    ServerRouting * server = new ServerRouting(*world, ruleset_name,
                                               server_name,
                                               server_id, int_id,
                                               lobby_id, lobby_int_id);

    // Create commserver instance that will handle connections from clients.
    // The commserver will create the other server related objects, and the
    // world object pair (World + WorldRouter), and initialise the admin
    // account.
    CommServer * commServer = new CommServer;

    if (commServer->setup() != 0) {
        log(CRITICAL, "Internal error setting up network infrastructure");
        return EXIT_SOCKET_ERROR;
    }

    // This is where we should restore the database, before
    // the listen sockets are open. Unlike earlier code, we are
    // attempting to construct the internal state from the database,
    // not creating a new world using the contents of the database as a
    // template

    IdleConnector* storage_idle = nullptr;

    if (database_flag) {
        // log(INFO, _("Restoring world from database..."));

        store->restoreWorld();
        // FIXME Do the following steps.
        // Read the world entity if any from the database, or set it up.
        // If it was there, make sure it did not get any of the wrong
        // position or orientation data.
        store->initWorld();

        // log(INFO, _("Restored world."));

        CommPSQLSocket * dbsocket = new CommPSQLSocket(*commServer,
                                        Persistence::instance()->m_db);
        commServer->addSocket(dbsocket);
        commServer->addIdle(dbsocket);

        storage_idle = new IdleConnector(*commServer);
        storage_idle->idling.connect(sigc::mem_fun(store, &StorageManager::tick));
        commServer->addIdle(storage_idle);
    } else {
        std::string adminId;
        long intId = newId(adminId);
        assert(intId >= 0);

        Admin * admin = new Admin(0, "admin", "BAD_HASH", adminId, intId);
        server->addAccount(admin);
    }

    // Add the test object, and call it regularly so it can do what it does.
    // UpdateTester * update_tester = new UpdateTester(*commServer);
    // commServer->addIdle(update_tester);

    boost::shared_ptr<CommClientFactory<CommUserClient, Connection> > atlas_clients =
            boost::make_shared<CommClientFactory<CommUserClient, Connection>,
                      ServerRouting & >(*server);
    if (client_port_num < 0) {
        client_port_num = dynamic_port_start;
        for (; client_port_num <= dynamic_port_end; client_port_num++) {
            if (TCPListenFactory::listen(*commServer,
                                         client_port_num,
                                         atlas_clients) == 0) {
                break;
            }
        }
        if (client_port_num > dynamic_port_end) {
            log(ERROR, String::compose("Could not find free client listen "
                                       "socket in range %1-%2. Init failed.",
                                       dynamic_port_start, dynamic_port_end));
            log(INFO, String::compose("To allocate 8 more ports please run:"
                                      "\n\n    cyconfig "
                                      "--cyphesis:dynamic_port_end=%1\n\n",
                                      dynamic_port_end + 8));
            return EXIT_PORT_ERROR;
        }
        log(INFO, String::compose("Auto configuring new instance \"%1\" "
                                  "to use port %2.",
                                  instance, client_port_num));
        global_conf->setItem(instance, "tcpport", client_port_num,
                             varconf::USER);
        global_conf->setItem(CYPHESIS, "dynamic_port_start",
                             client_port_num + 1, varconf::USER);
    } else if (TCPListenFactory::listen(*commServer,
                                        client_port_num,
                                        atlas_clients) != 0) {
        log(ERROR, String::compose("Could not create client listen socket "
                                   "on port %1. Init failed.",
                                   client_port_num));
        return EXIT_SOCKET_ERROR;
    }

#ifdef HAVE_SYS_UN_H
    CommUnixListener * localListener = new CommUnixListener(*commServer,
            boost::make_shared<CommClientFactory<CommAdminClient, TrustedConnection>,
                      ServerRouting &>(*server));
    if (localListener->setup(client_socket_name) != 0) {
        log(ERROR, String::compose("Could not create local listen socket "
                                   "with address \"%1\"",
                                   localListener->getPath()));
        delete localListener;
    } else {
        commServer->addSocket(localListener);
    }

    CommUnixListener * pythonListener = new CommUnixListener(*commServer,
            boost::make_shared<CommPythonClientFactory>());
    if (pythonListener->setup(python_socket_name) != 0) {
        log(ERROR, String::compose("Could not create python listen socket "
                                   "with address %1.",
                                   pythonListener->getPath()));
        delete pythonListener;
    } else {
        commServer->addSocket(pythonListener);
    }
#endif

    if (TCPListenFactory::listen(*commServer,
                                 http_port_num,
                                 boost::make_shared<CommHttpClientFactory>()) != 0) {
        log(ERROR, String::compose("Could not create http listen"
                                   " socket on port %1.", http_port_num));

    }

    if (useMetaserver) {
        CommMetaClient * cmc = new CommMetaClient(*commServer);
        if (cmc->setup(mserver) == 0) {
            commServer->addIdle(cmc);
        } else {
            log(ERROR, "Error creating metaserver comm channel.");
            delete cmc;
        }
    }

#if defined(HAVE_AVAHI)

    CommMDNSPublisher * cmdns = new CommMDNSPublisher(*commServer,
                                                      *server);
    if (cmdns->setup() == 0) {
        commServer->addSocket(cmdns);
        commServer->addIdle(cmdns);
    } else {
        log(ERROR, "Unable to register service with MDNS daemon.");
        delete cmdns;
    }

#endif // defined(HAVE_AVAHI)

    // Configuration is now complete, and verified as somewhat sane, so
    // we save the updated user config.

    updateUserConfiguration();

    log(INFO, "Running");
    logEvent(START, "- - - Standalone server startup");

    // Inform things that want to know that we are running.
    running();

    // Reduce our system priority to make it easier to debug a runaway
    // server.
    if (nice != 0) {
        reduce_priority(nice);
    }

    //Check if the world is populated; if it's not we should perhaps import some entities into it.
    //Note that we only check the top level; we don't perform a full hierarchical
    //traversal. Mainly because we never need to.
    auto autoImport = global_conf->getItem(CYPHESIS, "autoimport");
    if (autoImport.is_string() && autoImport.as_string() != "") {
        std::string importPath = autoImport.as_string();
        std::ifstream file(importPath);
        if (file.good()) {
            file.close();
            //We should only try to import if the world isn't populated.
            bool isPopulated = false;
            if (world->m_gameWorld.m_contains) {
                for (auto entity : *world->m_gameWorld.m_contains) {
                    //if there's any entity that's not transient we consider it populated
                    if (!entity->hasAttr("transient")) {
                        isPopulated = true;
                        break;
                    }
                }
            }

            if (!isPopulated) {
                //Populate the server through separate process (mainly because it's easier as we've
                //already written the importer tool; we might also do it in-process, but that would
                //require some rewriting of code).
                log(INFO,
                        compose("Trying to import world from %1.", importPath));
                std::stringstream ss;
                ss << PREFIX "/bin/cyimport --resume \"" << importPath  + "\"";
                std::string command = ss.str();
                std::thread importer(
                        [=]() {
                            int result = std::system(command.c_str());
                            if (result == 0) {
                                log(INFO, "Imported world into empty server.");
                            } else {
                                log(INFO, "No world imported.");
                            }
                        });
                importer.detach();
            }
        } else {
            log(NOTICE,
                    compose("Not importing as \"%1\" could not be found",
                            importPath));
            file.close();
        }
    }


    bool soft_exit_in_progess = false;
    time_t soft_exit_deadline = 0;
    // Loop until the exit flag is set. The exit flag can be set anywhere in
    // the code easily.
    while (!exit_flag) {
        try {
            time.update();
            bool busy = world->idle(time);
            commServer->idle(time, busy);
            commServer->poll(busy);
            if (soft_exit_in_progess) {
                //If we're in soft exit mode and either the deadline has been exceeded
                //or we've persisted all minds we should shut down normally.
                if (store->numberOfOutstandingThoughtRequests() == 0) {
                    log(NOTICE, "All entity thoughts were persisted.");
                    exit_flag = true;
                }
                if (time.seconds() + time.microseconds()
                        >= soft_exit_deadline) {
                    log(WARNING,
                            "Waiting for persisting thoughts timed out. This might "
                                    "lead to lost entity thoughts.");
                    exit_flag = true;
                }
            } else if (exit_flag_soft) {
                soft_exit_in_progess = true;
                //Set a deadline for five seconds.
                static const time_t mind_persistence_deadline = 5;
                size_t requestNumber = store->requestMinds(
                        world->getEntities());
                log(INFO,
                        String::compose(
                                "Soft exit requested, persisting %1 minds.",
                                requestNumber));
                soft_exit_deadline = time.seconds() + time.microseconds()
                        + (mind_persistence_deadline * 1000000L);
                log(NOTICE,
                        String::compose(
                                "Deadline for mind persistence set to %1 seconds.",
                                mind_persistence_deadline));
            }
        // It is hoped that commonly thrown exception, particularly
        // exceptions that can be caused  by external influences
        // should be caught close to where they are thrown. If
        // an exception makes it here then it should be debugged.
        } catch (const std::exception& e) {
            log(ERROR,
                    String::compose("Exception caught in main(): %1",
                            e.what()));
        } catch (...) {
            log(ERROR, "Exception caught in main()");
        }
    }
    // exit flag has been set so we close down the databases, and indicate
    // to the metaserver (if we are using one) that this server is going down.
    // It is assumed that any preparation for the shutdown that is required
    // by the game has been done before exit flag was set.
    log(NOTICE, "Performing clean shutdown...");

    //Actually, there's no way for the world to know that it's shutting down,
    //as the shutdown signal most probably comes from a sighandler. We need to
    //tell it it's shutting down so it can do some housekeeping.
    try {
        exit_flag = false;
        if (store->shutdown(exit_flag, world->getEntities()) != 0) {
            //Ignore this error and carry on with shutting down.
            log(ERROR, "Error when shutting down");
        }
    } catch (const std::exception& e) {
        log(ERROR,
                String::compose("Exception caught when shutting down: %1",
                        e.what()));
    } catch (...) {
        //Ignore this error and carry on with shutting down.
        log(ERROR, "Exception caught when shutting down");
    }

    //the Idle destructor will remove itself from commServer;
    //thus we must delete storage_idle before we destroy commServer
    delete storage_idle;

    delete commServer;

    delete server;

    delete store;

    delete world;

    Persistence::instance()->shutdown();

    EntityBuilder::instance()->flushFactories();
    EntityBuilder::del();
    ArithmeticBuilder::del();
    TeleportAuthenticator::del();

    Inheritance::clear();

    // Shutdown the Python interpreter. This frees lots of memory, and if
    // the malloc heap is in any way corrupt, a segfault is likely to
    // occur at this point. Previous occasions where pointers have been
    // deleted twice elsewhere in the code, have resulted in a segfault
    // at this point. AlRiddoch 10th November 2001
    shutdown_python_api();

    delete global_conf;

    log(INFO, "Clean shutdown complete.");
    logEvent(STOP, "- - - Standalone server shutdown");
    return 0;
}
