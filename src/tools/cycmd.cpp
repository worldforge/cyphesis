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


/// \page cycmd_index
///
/// \section Introduction
///
/// cycmd is a commandline tool to administrate the running server. For
/// information on the commands available, please see the unix manual page.
/// The manual page is generated from docbook sources, so can
/// also be converted into other formats.
///
/// The majority of the functionality is encapsulated by the Interactive
/// class template.

#include "Interactive.h"

#include "common/compose.hpp"
#include "common/log.h"
#include "common/globals.h"
#include "common/sockets.h"
#include "common/system.h"

#include <varconf/config.h>

using String::compose;

static void usage(char * prg)
{
    std::cerr << "usage: " << prg << " [options] [ cmd [ server ] ]" << std::endl << std::flush;
}

int main(int argc, char ** argv)
{
    setLoggingPrefix("CMD");

    int config_status = loadConfig(argc, argv, USAGE_CYCMD);
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            reportVersion(argv[0]);
            return 0;
        } else if (config_status == CONFIG_HELP) {
            showUsage(argv[0], USAGE_CYCMD, "[ cmd [ server ] ]");
            return 0;
        } else if (config_status != CONFIG_ERROR) {
            log(ERROR, "Unknown error reading configuration.");
        }
        // Fatal error loading config file
        return 1;
    }

    int optindex = config_status;

    std::string server;
    readConfigItem("client", "serverhost", server);

    int useslave = 0;
    readConfigItem("client", "useslave", useslave);

    bool interactive = true;
    std::string cmd;
    if (optindex < argc) {
        if ((argc - optindex) == 2) {
            server = argv[optindex + 1];
        } else if ((argc - optindex) > 2) {
            usage(argv[0]);
            return 1;
        }
        cmd = argv[optindex];
        interactive = false;
    }

    Atlas::Objects::Factories factories;

    boost::asio::io_context io_context;
    Interactive bridge(factories, io_context);

    if (server.empty()) {
        std::string localSocket;
        if (useslave != 0) {
            localSocket = slave_socket_name;
        } else {
            localSocket = client_socket_name;
        }

        log(NOTICE, "Attempting local connection");
        if (bridge.connectLocal(localSocket) == 0) {
            bridge.setup();
            if (bridge.create("sys",
                              create_session_username(),
                              compose("%1%2", ::rand(), ::rand())) != 0) {
                bridge.getLogin();
                if (bridge.login() != 0) {
                    std::cout << "failed." << std::endl << std::flush;
                    return 1;
                }
            }
            if (!interactive) {
                bridge.exec(cmd, "");
            } else {
                bridge.updatePrompt();
                bridge.loop();
            }
            return 0;
        }
        server = "localhost";
    }
    
    log(NOTICE, "Attempting tcp connection");

    if (bridge.connect(server) != 0) {
        return 1;
    }
    bridge.setup();
    if (!interactive) {
        std::cerr << "WARNING: No login details available for remote host"
                  << std::endl
                  << "WARNING: Attempting command without logging in"
                  << std::endl << std::flush;
    } else {
        bridge.getLogin();
        if (bridge.login() != 0) {
            return 1;
        }
    }
    if (!interactive) {
        bridge.exec(cmd, "");
    } else {
        bridge.updatePrompt();
        bridge.loop();
    }
    delete global_conf;
    return 0;
}
