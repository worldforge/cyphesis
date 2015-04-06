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

#include "CommHttpClient.h"
#include "CommPythonClient.h"
#include "CommPSQLSocket.h"
#include "CommMetaClient.h"
#include "CommMDNSPublisher.h"
#include "CommAsioListener_impl.h"
#include "CommAsioClient.h"
#include "Connection.h"
#include "ServerRouting.h"
#include "EntityBuilder.h"
#include "ArithmeticBuilder.h"
#include "Persistence.h"
#include "WorldRouter.h"
#include "Ruleset.h"
#include "StorageManager.h"
#include "IdleConnector.h"
#include "Admin.h"
#include "TeleportAuthenticator.h"
#include "TrustedConnection.h"

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
#include "common/Monitors.h"

#include <varconf/config.h>

#include <sigc++/functors/mem_fun.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <boost/asio/io_service.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <thread>
#include <cstdlib>
#include <fstream>

using String::compose;
using namespace boost::asio;

class TrustedConnection;
class Peer;

static const bool debug_flag = false;

INT_OPTION(http_port_num, 6780, CYPHESIS, "httpport",
        "Network listen port for http connection to the server")
;

BOOL_OPTION(useMetaserver, true, CYPHESIS, "usemetaserver",
        "Flag to control registration with the metaserver")
;

STRING_OPTION(mserver, "metaserver.worldforge.org", CYPHESIS, "metaserver",
        "Hostname to use as the metaserver")
;

// Keep a reference to the global io_service so that it can be awoken
// in our signals callback.
boost::asio::io_service* sGlobalIoService = nullptr;

/**
 * A signals callback which will make sure the main io_service is awoken.
 * This should be hooked into the signals processing.
 */
void ioServiceExitCallback() {
    //Just post an empty handler. This makes sure that the io_service is awaken
    //if it's currently waiting inside a call to run_once.
    sGlobalIoService->post([](){});
}

int main(int argc, char ** argv)
{
    if (security_init() != 0) {
        log(CRITICAL, "Security initialization Error. Exiting.");
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
                    "to permanently disable database usage.", instance));
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

    io_service* io_service = new boost::asio::io_service();
    //Register the io_server with the signals callback, thus making sure that if
    //a relevant SIG* signal is received the io_service will be awoken from any
    //run_one call it might be waiting on.
    sGlobalIoService = io_service;
    setExitSignalCallback(ioServiceExitCallback);

    // Start up the Python subsystem.
    init_python_api(ruleset_name);

    Inheritance::instance();

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

    if (((int_id = newId(server_id)) < 0)
            || ((lobby_int_id = newId(lobby_id)) < 0)) {
        log(CRITICAL, "Unable to get server IDs from Database");
        return EXIT_DATABASE_ERROR;
    }

    // Create the core server object, which stores central data,
    // and track objects
    ServerRouting * server = new ServerRouting(*world, ruleset_name,
            server_name, server_id, int_id, lobby_id, lobby_int_id);

    // This is where we should restore the database, before
    // the listen sockets are open. Unlike earlier code, we are
    // attempting to construct the internal state from the database,
    // not creating a new world using the contents of the database as a
    // template

    IdleConnector* storage_idle = nullptr;

    CommPSQLSocket * dbsocket = nullptr;
    if (database_flag) {
        // log(INFO, _("Restoring world from database..."));

        store->restoreWorld();
        // FIXME Do the following steps.
        // Read the world entity if any from the database, or set it up.
        // If it was there, make sure it did not get any of the wrong
        // position or orientation data.
        store->initWorld();

        // log(INFO, _("Restored world."));

        dbsocket = new CommPSQLSocket(*io_service,
                Persistence::instance()->m_db);

        storage_idle = new IdleConnector(*io_service);
        storage_idle->idling.connect(
                sigc::mem_fun(store, &StorageManager::tick));
    } else {
        std::string adminId;
        long intId = newId(adminId);
        assert(intId >= 0);

        Admin * admin = new Admin(0, "admin", "BAD_HASH", adminId, intId);
        server->addAccount(admin);
    }

    std::function<void(CommAsioClient<ip::tcp>&)> tcpAtlasStarter =
            [&](CommAsioClient<ip::tcp>& client) {

                std::string connection_id;
                long c_iid = newId(connection_id);
                //Turn off Nagle's algorithm to increase responsiveness.
                client.getSocket().set_option(ip::tcp::no_delay(true));
                client.startAccept(
                        new Connection(client, *server, "", connection_id, c_iid));
            };

    std::list<
            CommAsioListener<ip::tcp,
                    CommAsioClient<ip::tcp>> > tcp_atlas_clients;

    if (client_port_num < 0) {
        client_port_num = dynamic_port_start;
        for (; client_port_num <= dynamic_port_end; client_port_num++) {
            try {
                tcp_atlas_clients.emplace_back(tcpAtlasStarter,
                        server->getName(), *io_service,
                        ip::tcp::endpoint(
                                ip::tcp::v4(), client_port_num));
            } catch (const std::exception& e) {
                break;
            }
        }
        if (client_port_num < dynamic_port_end) {
            log(ERROR,
                    String::compose("Could not find free client listen "
                            "socket in range %1-%2. Init failed.",
                            dynamic_port_start, dynamic_port_end));
            log(INFO,
                    String::compose("To allocate 8 more ports please run:"
                            "\n\n    cyconfig "
                            "--cyphesis:dynamic_port_end=%1\n\n",
                            dynamic_port_end + 8));
            return EXIT_PORT_ERROR;
        }
        log(INFO, String::compose("Auto configuring new instance \"%1\" "
                "to use port %2.", instance, client_port_num));
        global_conf->setItem(instance, "tcpport", client_port_num,
                varconf::USER);
        global_conf->setItem(CYPHESIS, "dynamic_port_start",
                client_port_num + 1, varconf::USER);
    } else {
        try {
            tcp_atlas_clients.emplace_back(tcpAtlasStarter, server->getName(),
                    *io_service,
                    ip::tcp::endpoint(ip::tcp::v4(),
                            client_port_num));
        } catch (const std::exception& e) {
            log(ERROR, String::compose("Could not create client listen socket "
                    "on port %1. Init failed. The most common reason for this "
                    "is that you're already running an instance of Cyphesis.",
                    client_port_num));
            return EXIT_SOCKET_ERROR;
        }
    }

    remove(python_socket_name.c_str());
    std::function<void(CommPythonClient&)> pythonStarter =
            [&](CommPythonClient& client) {
                client.startAccept();
            };
    auto pythonListener = new CommAsioListener<local::stream_protocol,
            CommPythonClient>(pythonStarter,
            server->getName(), *io_service,
            local::stream_protocol::endpoint(python_socket_name));

    remove(client_socket_name.c_str());
    std::function<void(CommAsioClient<local::stream_protocol>&)> localStarter =
            [&](CommAsioClient<local::stream_protocol>& client) {

                std::string connection_id;
                long c_iid = newId(connection_id);
                client.startAccept(
                        new TrustedConnection(client, *server, "", connection_id, c_iid));
            };
    auto localListener = new CommAsioListener<local::stream_protocol,
            CommAsioClient<local::stream_protocol>>(localStarter,
            server->getName(), *io_service,
            local::stream_protocol::endpoint(client_socket_name));


    std::function<void(CommHttpClient&)> httpStarter =
            [&](CommHttpClient& client) {
                client.serveRequest();
            };

    auto httpListener = new CommAsioListener<ip::tcp, CommHttpClient>(httpStarter,
            server->getName(), *io_service,
            ip::tcp::endpoint(ip::tcp::v4(), http_port_num));

    CommMetaClient * cmc(nullptr);
    if (useMetaserver) {
        cmc = new CommMetaClient(*io_service);
        if (cmc->setup(mserver) != 0) {
            log(ERROR, "Error creating metaserver comm channel.");
            delete cmc;
            cmc = nullptr;
        }
    }

    CommMDNSPublisher * cmdns = nullptr;
#if defined(HAVE_AVAHI)

    cmdns = new CommMDNSPublisher(*io_service, *server);
    if (cmdns->setup() != 0) {
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
            if (world->getRootEntity().m_contains) {
                for (auto entity : *world->getRootEntity().m_contains) {
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
                ss << PREFIX
                "/bin/cyimport --resume \"" << importPath + "\"";
                std::string command = ss.str();
                std::thread importer([=]() {
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

    bool soft_exit_in_progress = false;

    //Make sure that the io_service never runs out of work.
    boost::asio::io_service::work work(*io_service);
    //This timer is used to wake the io_service when next op needs to be handled.
    boost::asio::deadline_timer nextOpTimer(*io_service);
    //This timer will set a deadline for any mind persistence during soft exits.
    boost::asio::deadline_timer softExitTimer(*io_service);
    // Loop until the exit flag is set. The exit flag can be set anywhere in
    // the code easily.
    while (!exit_flag) {
        try {
            time.update();
            bool busy = world->idle();
            world->markQueueAsClean();
            //If the world is busy we should just poll.
            if (busy) {
                io_service->poll();
            } else {
                //If it's not busy however we should run until we get a task.
                //We will either get an io task, or we will be triggered by the timer
                //which is set to expire when the next op should be dispatched.
                double secondsUntilNextOp = world->secondsUntilNextOp();
                if (secondsUntilNextOp <= 0.0) {
                    io_service->poll();
                } else {
                    bool nextOpTimeExpired = false;
                    boost::posix_time::microseconds waitTime((long long)(secondsUntilNextOp * 1000000));
                    nextOpTimer.expires_from_now(waitTime);
                    nextOpTimer.async_wait([&](boost::system::error_code ec){
                        if (ec != boost::asio::error::operation_aborted) {
                            nextOpTimeExpired = true;
                        }
                    });
                    //Keep on running IO handlers until either the queue is dirty (i.e. we need to handle
                    //any new operation) or the timer has expired.
                    do {
                        io_service->run_one();
                    } while (!world->isQueueDirty() && !nextOpTimeExpired &&
                            !exit_flag_soft && !exit_flag && !soft_exit_in_progress);
                    nextOpTimer.cancel();
                }
            }
            if (soft_exit_in_progress) {
                //If we're in soft exit mode and either the deadline has been exceeded
                //or we've persisted all minds we should shut down normally.
                if (store->numberOfOutstandingThoughtRequests() == 0) {
                    log(NOTICE, "All entity thoughts were persisted.");
                    exit_flag = true;
                    softExitTimer.cancel();
                }
            } else if (exit_flag_soft) {
                exit_flag_soft = false;
                soft_exit_in_progress = true;
                size_t requestNumber = store->requestMinds(
                        world->getEntities());
                log(INFO,
                        String::compose(
                                "Soft exit requested, persisting %1 minds.",
                                requestNumber));
                //Set a deadline for five seconds.
                softExitTimer.expires_from_now(boost::posix_time::seconds(5));
                softExitTimer.async_wait([&](boost::system::error_code ec){
                    if (!ec) {
                        log(WARNING,
                                "Waiting for persisting thoughts timed out. This might "
                                        "lead to lost entity thoughts.");
                        exit_flag = true;
                    }
                });
                log(NOTICE,
                        String::compose(
                                "Deadline for mind persistence set to %1 seconds.",
                                5));
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

    delete cmdns;

    if (cmc) {
        cmc->metaserverTerminate();
        delete cmc;
    }

    delete localListener;
    delete httpListener;
    delete pythonListener;

    tcp_atlas_clients.clear();

    delete storage_idle;

    delete io_service;

    delete server;

    delete store;

    delete world;

    delete dbsocket;

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

    Monitors::cleanup();

    log(INFO, "Clean shutdown complete.");
    logEvent(STOP, "- - - Standalone server shutdown");
    setExitSignalCallback(nullptr);
    sGlobalIoService = nullptr;
    return 0;
}
