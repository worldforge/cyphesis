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

#include "ClientConnection.h"
#include "PossessionClient.h"

#include "rulesets/Python_API.h"
#include "rulesets/MindFactory.h"
#include "rulesets/PythonScriptFactory.h"

#include "common/debug.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/compose.hpp"
#include "common/sockets.h"
#include "common/Inheritance.h"
#include "common/SystemTime.h"

#include <varconf/config.h>

#include <memory>
#define _GLIBCXX_USE_NANOSLEEP 1
#include <thread>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

static void usage(const char * prgname)
{
    std::cout << "usage: " << prgname << " [ local_socket_path ]" << std::endl
            << std::flush;
}

STRING_OPTION(server, "localhost", "aiclient", "serverhost",
        "Hostname of the server to connect to")
;

STRING_OPTION(account, "", "aiclient", "account",
        "Account name to use to authenticate to the server")
;

STRING_OPTION(password, "", "aiclient", "password",
        "Password to use to authenticate to the server")
;

static bool debug_flag = false;

static int tryToConnect(PossessionClient& possessionClient)
{
    if (possessionClient.connectLocal(client_socket_name) == 0) {
        log(INFO,
                String::compose("Connected to server at %1.",
                        client_socket_name));
        Root systemAccountResponse = possessionClient.createSystemAccount();

        possessionClient.enablePossession();
        return 0;
    } else {
        return -1;
    }
}

int main(int argc, char ** argv)
{
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

    init_python_api(ruleset_name, false);

    Inheritance::instance();

    SystemTime time;
    time.update();

    MindFactory mindFactory;

    std::string script_package = "mind.NPCMind";
    std::string script_class = "NPCMind";

    if (mindFactory.m_scriptFactory != 0) {
        if (mindFactory.m_scriptFactory->package() != script_package) {
            delete mindFactory.m_scriptFactory;
            mindFactory.m_scriptFactory = 0;
        }
    }
    if (mindFactory.m_scriptFactory == 0) {
        PythonScriptFactory<BaseMind> * psf = new PythonScriptFactory<BaseMind>(
                script_package, script_class);
        if (psf->setup() == 0) {
            log(INFO,
                    String::compose(
                            "Initialized mind code with Python class %1.%2.",
                            script_package, script_class));
            mindFactory.m_scriptFactory = psf;
        } else {
            log(ERROR,
                    String::compose("Python class \"%1.%2\" failed to load",
                            script_package, script_class));
            delete psf;
        }
    }

    std::unique_ptr<PossessionClient> possessionClient(
            new PossessionClient(mindFactory));
    while (tryToConnect(*possessionClient) != 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }

    while (!exit_flag) {
        int netResult = possessionClient->handleNet();
        if (netResult == 0) {
            //As long as we're connected we'll keep on processing minds
            possessionClient->idle();
        } else {
            log(ERROR,
                    "Disconnected from server; will try to reconnect every five seconds.");
            //We're disconnected. We'll now enter a loop where we'll try to reconnect at an interval.
            //First we need to shut down the current client. Perhaps we could find a way to persist the minds in a better way?
            possessionClient.reset(new PossessionClient(mindFactory));
            while (tryToConnect(*possessionClient) != 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            }

        }
    }
}
