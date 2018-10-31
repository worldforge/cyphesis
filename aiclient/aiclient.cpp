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

#include "PossessionClient.h"
#include "rules/python/Python_API.h"
#include "rules/python/PythonScriptFactory.h"
#include "rules/ai/AwareMindFactory.h"
#include "rules/python/CyPy_Atlas.h"
#include "rules/python/CyPy_Common.h"
#include "rules/python/CyPy_Physics.h"
#include "rules/ai/python/CyPy_Ai.h"
#include "rules/entityfilter/python/CyPy_EntityFilter.h"

#include "common/sockets.h"
#include "common/SystemTime.h"
#include "common/system.h"
#include "common/RuleTraversalTask.h"

#define _GLIBCXX_USE_NANOSLEEP 1

#include "common/MainLoop.h"
#include "common/CommAsioClient.h"
#include "common/CommAsioClient_impl.h"
#include "common/AssetsManager.h"
#include "common/FileSystemObserver.h"
#include "common/operations/Think.h"
#include "common/globals.h"

#include <sys/prctl.h>
#include <rules/python/CyPy_Rules.h>


using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

static void usage(const char* prgname)
{
    std::cout << "usage: " << prgname << " [ local_socket_path ]" << std::endl << std::flush;
}

STRING_OPTION(server, "localhost", "aiclient", "serverhost", "Hostname of the server to connect to");

STRING_OPTION(account, "", "aiclient", "account", "Account name to use to authenticate to the server");

STRING_OPTION(password, "", "aiclient", "password", "Password to use to authenticate to the server");

static void connectToServer(boost::asio::io_service& io_service, AwareMindFactory& mindFactory)
{
    if (exit_flag_soft || exit_flag) {
        return;
    }
    auto commClient = std::make_shared<CommAsioClient<boost::asio::local::stream_protocol>>("aiclient", io_service);

    commClient->getSocket().async_connect({client_socket_name}, [&io_service, &mindFactory, commClient](boost::system::error_code ec) {
        if (!ec) {
            log(INFO, "Connection detected; creating possession client.");
            commClient->startConnect(new PossessionClient(*commClient, mindFactory, [&]() {
                connectToServer(io_service, mindFactory);
            }));
        } else {
            //If we couldn't connect we'll wait five seconds and try again.
            auto timer = std::make_shared<boost::asio::steady_timer>(io_service);
            timer->expires_after(std::chrono::seconds(5));
            timer->async_wait([&io_service, &mindFactory, timer](boost::system::error_code ec) {
                if (!ec) {
                    connectToServer(io_service, mindFactory);
                }
            });
        }
    });
}


int main(int argc, char** argv)
{

    //Kill ourselves if our parent is killed.
    prctl(PR_SET_PDEATHSIG, SIGTERM);

    std::unique_ptr<Monitors> monitors(new Monitors());

    int config_status = loadConfig(argc, argv, USAGE_AICLIENT);
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            reportVersion(argv[0]);
            return 0;
        } else if (config_status == CONFIG_HELP) {
            showUsage(argv[0], USAGE_AICLIENT, "[ local_socket_path ]");
            return 0;
        } else if (config_status != CONFIG_ERROR) {
            log(ERROR, "Unknown error reading configuration.");
        }
        // Fatal error loading config file
        return 1;
    }

    int optind = config_status;

    assert(optind <= argc);

    if (optind == (argc - 1)) {
        std::string arg(argv[optind]);

    } else if (optind != argc) {
        usage(argv[0]);
        return 1;
    }

    AwareMindFactory mindFactory;

    boost::asio::io_service io_service;

    {
        FileSystemObserver file_system_observer(io_service);

        AssetsManager assets_manager(file_system_observer);
        assets_manager.init();


        init_python_api({&CyPy_Mind::init,
                         &CyPy_Rules::init,
                         &CyPy_Physics::init,
                         &CyPy_EntityFilter::init,
                         &CyPy_Atlas::init,
                         &CyPy_Common::init},
                        ruleset_name, false);
        observe_python_directories(io_service, assets_manager);


        run_user_scripts("cyaiclient");

        SystemTime time{};
        time.update();


        //TODO: perhaps don't hardcode this; instead allowing for different classes for different minds?
        std::string script_package = "mind.NPCMind";
        std::string script_class = "NPCMind";

        if (mindFactory.m_scriptFactory != nullptr) {
            if (mindFactory.m_scriptFactory->package() != script_package) {
                delete mindFactory.m_scriptFactory;
                mindFactory.m_scriptFactory = nullptr;
            }
        }
        if (mindFactory.m_scriptFactory == nullptr) {
            auto* psf = new PythonScriptFactory<BaseMind>(script_package, script_class);
            if (psf->setup() == 0) {
                log(INFO, String::compose("Initialized mind code with Python class %1.%2.", script_package, script_class));
                mindFactory.m_scriptFactory = psf;
            } else {
                log(ERROR, String::compose("Python class \"%1.%2\" failed to load", script_package, script_class));
                delete psf;
            }
        }

        boost::asio::signal_set signalSet(io_service);
        //If we're not running as a daemon we should use the interactive signal handler.
        if (!daemon_flag) {
            signalSet.add(SIGINT);
            signalSet.add(SIGTERM);
            signalSet.add(SIGHUP);
            signalSet.add(SIGQUIT);

        } else {
            signalSet.add(SIGTERM);
        }
        signalSet.async_wait([&](boost::system::error_code ec, int signal) {
            if (!ec) {
                exit_flag = true;
                exit_flag_soft = true;
                io_service.stop();
            }
        });


        //Reload the script factory when scripts changes.
        //Any PossessionAccount instance will also take care of reloading the script instances.
        python_reload_scripts.connect([&]() {
            mindFactory.m_scriptFactory->refreshClass();
        });

        log(INFO, "Trying to connect to server.");
        connectToServer(io_service, mindFactory);

        /// \brief Use a "work" instance to make sure the io_service never runs out of work and is stopped.
        boost::asio::io_service::work m_io_work(io_service);

        io_service.run();

        signalSet.clear();

        log(INFO, "Shutting down.");
    }
}
