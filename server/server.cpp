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
#include "PossessionAuthenticator.h"
#include "TrustedConnection.h"
#include "HttpCache.h"

#include "rulesets/Python_API.h"
#include "rulesets/LocatedEntity.h"
#include "rulesets/World.h"

#if POSTGRES_FOUND
#include "common/DatabasePostgres.h"
#endif

#include "common/id.h"
#include "common/const.h"
#include "common/Inheritance.h"
#include "common/system.h"
#include "common/sockets.h"
#include "common/SystemTime.h"
#include "common/Monitors.h"
#include "ExternalMindsManager.h"

#include <varconf/config.h>

#include <thread>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <common/FileSystemObserver.h>
#include <common/AssetsManager.h>
#include <common/DatabaseSQLite.h>
#include <common/RepeatedTask.h>
#include <common/MainLoop.h>

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

INT_OPTION(ai_clients, 1, CYPHESIS, "aiclients",
        "Number of AI clients to spawn.")
;


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

    auto monitors = new Monitors();
    //Check if we should spawn AI clients.
    if (ai_clients) {
        log(INFO, compose("Spawning %1 AI client processes.", ai_clients));
        for (int i = 0; i < ai_clients; ++i) {
            auto pid = fork();
            if (pid == 0) {
                execl((bin_directory + "/cyaiclient").c_str(), (bin_directory + "/cyaiclient").c_str(), nullptr);
                return EXIT_FORK_ERROR;
            } else if (pid == -1) {
                log(WARNING, "Could not spawn AI client process.");
            }
        }
    }

    // If we are a daemon logging to syslog, we need to set it up.
    initLogger();

    //Check that there's a valid assets directory, and warn if not.
    if (!boost::filesystem::is_directory(assets_directory)) {
        log(ERROR, String::compose("Could not find any valid assets directory at '%1'.", assets_directory));
        log(ERROR, "If you've built Cyphesis yourself make sure you've run the 'make assets-download' command.");
    }


    auto io_service = new boost::asio::io_service();

    // Initialise the persistence subsystem.
    std::string databaseBackend;
    readConfigItem(instance, "database", databaseBackend);

    Database* database;
    CommPSQLSocket * dbsocket = nullptr;
    RepeatedTask* dbvacuumTask = nullptr;
    if (databaseBackend == "postgres") {
#if POSTGRES_FOUND
        database = new DatabasePostgres();
        dbsocket = new CommPSQLSocket(*io_service, *database);
#else
        log(ERROR, "Database specified as 'postgres', but this server is not built with Postgres SQL support.");
        return -1;
#endif
    } else {
        auto sqliteDatabase = new DatabaseSQLite();
        database = sqliteDatabase;
        dbvacuumTask = new RepeatedTask(*io_service, boost::posix_time::seconds(25 * 60), [=]() {sqliteDatabase->runMaintainance();});
    }

    auto persistence = new Persistence(*database);
    int dbstatus = persistence->init();
    if (dbstatus < 0) {
        log(ERROR, "Error opening database.");
        return -1;
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



    FileSystemObserver* file_system_observer = new FileSystemObserver(*io_service);

    AssetsManager* assets_manager = new AssetsManager(*file_system_observer);
    assets_manager->init();

    // Start up the Python subsystem.
    init_python_api(ruleset_name);
    observe_python_directories(*io_service, *assets_manager);

    Inheritance* inheritance = new Inheritance();

    SystemTime time{};
    time.update();

    auto entityBuilder = new EntityBuilder();
    auto arithmenticBuilder = new ArithmeticBuilder();

    Ruleset* ruleset = new Ruleset(entityBuilder, *io_service);
    ruleset->loadRules(ruleset_name);

    Ref<LocatedEntity> baseEntity = new World(consts::rootWorldId, consts::rootWorldIntId);
    baseEntity->setType(Inheritance::instance().getType("world"));

    WorldRouter * world = new WorldRouter(time, baseEntity);

    register_baseworld_with_python(world);


    auto possessionAuthenticator = new PossessionAuthenticator();

    auto externalMindsManager = new ExternalMindsManager();
    auto store = new StorageManager(*world);

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

    std::function<void(CommAsioClient<ip::tcp>&)> tcpAtlasStarter = [&](CommAsioClient<ip::tcp>& client) {
        std::string connection_id;
        long c_iid = newId(connection_id);
        //Turn off Nagle's algorithm to increase responsiveness.
        client.getSocket().set_option(ip::tcp::no_delay(true));
        //Listen to both ipv4 and ipv6
        //client.getSocket().set_option(boost::asio::ip::v6_only(false));
        client.startAccept(new Connection(client, *server, "", connection_id, c_iid));
    };

    std::list<CommAsioListener<ip::tcp, CommAsioClient<ip::tcp>>> tcp_atlas_clients;

    if (client_port_num < 0) {
        client_port_num = dynamic_port_start;
        for (; client_port_num <= dynamic_port_end; client_port_num++) {
            try {
                tcp_atlas_clients.emplace_back(tcpAtlasStarter, server->getName(), *io_service,
                                               ip::tcp::endpoint(ip::tcp::v6(), client_port_num));
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
            tcp_atlas_clients.emplace_back(tcpAtlasStarter, server->getName(), *io_service, ip::tcp::endpoint(ip::tcp::v6(), client_port_num));
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
    auto pythonListener = new CommAsioListener<local::stream_protocol, CommPythonClient>(pythonStarter, server->getName(), *io_service,
                                                                                         local::stream_protocol::endpoint(python_socket_name));

    remove(client_socket_name.c_str());
    std::function<void(CommAsioClient<local::stream_protocol>&)> localStarter = [&](CommAsioClient<local::stream_protocol>& client) {
        std::string connection_id;
        long c_iid = newId(connection_id);
        client.startAccept(new TrustedConnection(client, *server, "", connection_id, c_iid));
    };
    auto localListener = new CommAsioListener<local::stream_protocol, CommAsioClient<local::stream_protocol>>(localStarter, server->getName(), *io_service,
                                                                                                              local::stream_protocol::endpoint(client_socket_name));


    //Instantiate at startup
    auto httpCache = new HttpCache();
    std::function<void(CommHttpClient&)> httpStarter = [&](CommHttpClient& client) {
        //Listen to both ipv4 and ipv6
        //client.getSocket().set_option(boost::asio::ip::v6_only(false));
        client.serveRequest();
    };

    auto httpListener = new CommAsioListener<ip::tcp, CommHttpClient>(httpStarter, server->getName(), *io_service,
                                                                      ip::tcp::endpoint(ip::tcp::v6(), http_port_num));

    log(INFO, compose("Http service. The following endpoints are available over port %1.", http_port_num));
    log(INFO, " /config : shows server configuration");
    log(INFO, " /monitors : various monitored values, suitable for time series systems");
    log(INFO, " /monitors/numerics : only numerical values, suitable for time series system that only operates on numerical data");

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


    IdleConnector* storage_idle = nullptr;

    log(INFO, "Restoring world from database...");

    store->restoreWorld(baseEntity);
    // Read the world entity if any from the database, or set it up.
    // If it was there, make sure it did not get any of the wrong
    // position or orientation data.
    store->initWorld(baseEntity);

    log(INFO, "Restored world.");

    storage_idle = new IdleConnector(*io_service);
    storage_idle->idling.connect(
        sigc::mem_fun(store, &StorageManager::tick));


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
    if (autoImport.is_string() && !autoImport.as_string().empty()) {
        std::string importPath = autoImport.as_string();
        std::ifstream file(importPath);
        if (file.good()) {
            file.close();
            //We should only try to import if the world isn't populated.
            bool isPopulated = false;
            if (baseEntity->m_contains) {
                for (const auto& entity : *baseEntity->m_contains) {
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
                ss << bin_directory <<
                "/cyimport --resume \"" << importPath + "\"";
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

    auto softExitStart = [&]() {
        size_t requestNumber = store->requestMinds(world->getEntities());
        log(INFO, String::compose("Soft exit requested, persisting %1 minds.", requestNumber));

        //Set a deadline for five seconds.
        boost::posix_time::seconds duration(5);
        log(NOTICE, String::compose("Deadline for mind persistence set to %1 seconds.", duration.ticks()));

        return duration;
    };


    auto softExitPoll = [&](){
        if (store->numberOfOutstandingThoughtRequests() == 0) {
            log(NOTICE, "All entity thoughts were persisted.");
            return true;
        }
        return false;
    };

    auto softExitTimeout = [&]() {
        log(WARNING, String::compose("Waiting for persisting thoughts timed out. This might "
                                     "lead to lost entity thoughts. %1 thoughts outstanding.",
                                     store->numberOfOutstandingThoughtRequests()));

    };

    MainLoop::run(daemon_flag, *io_service, world->getOperationsHandler(), {softExitStart, softExitPoll, softExitTimeout});


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


    delete dbvacuumTask;

    delete assets_manager;

    delete file_system_observer;

    delete cmdns;

    if (cmc) {
        cmc->metaserverTerminate();
        delete cmc;
    }

    delete localListener;
    delete httpListener;

    delete httpCache;

    delete pythonListener;

    tcp_atlas_clients.clear();

    delete storage_idle;

    delete dbsocket;

    //Run any outstanding tasks before shutting down service.
    io_service->run();

    delete io_service;

    delete server;

    delete store;

    delete externalMindsManager;

    delete possessionAuthenticator;

    delete world;

    delete ruleset;

    entityBuilder->flushFactories();
    delete entityBuilder;

    delete arithmenticBuilder;

    delete persistence;

    delete database;

    delete inheritance;

    // Shutdown the Python interpreter. This frees lots of memory, and if
    // the malloc heap is in any way corrupt, a segfault is likely to
    // occur at this point. Previous occasions where pointers have been
    // deleted twice elsewhere in the code, have resulted in a segfault
    // at this point. AlRiddoch 10th November 2001
    shutdown_python_api();

    delete global_conf;

    delete monitors;

    log(INFO, "Clean shutdown complete.");
    logEvent(STOP, "- - - Standalone server shutdown");
    return 0;
}
