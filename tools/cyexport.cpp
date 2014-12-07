/*
 Copyright (C) 2013 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "common/compose.hpp"
#include "common/log.h"
#include "common/globals.h"
#include "common/sockets.h"
#include "common/system.h"
#include "common/AtlasStreamClient.h"

#include "EntityExporter.h"
#include "AgentCreationTask.h"

#include <varconf/config.h>

#include <iostream>
#include <cstdlib>

static void usage(char * prg)
{
    std::cerr << "usage: " << prg << " [options] filepath" << std::endl
            << std::flush;
}

BOOL_OPTION(transients, false, "export", "transients",
        "Flag to control if transients should also be exported");
BOOL_OPTION(rules, false, "export", "rules",
        "Flag to control if rules should also be exported");
BOOL_OPTION(minds, true, "export", "minds",
        "Flag to control if minds should also be exported");

int main(int argc, char ** argv)
{
    int config_status = loadConfig(argc, argv, USAGE_CYCMD);
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            reportVersion(argv[0]);
            return 0;
        } else if (config_status == CONFIG_HELP) {
            showUsage(argv[0], USAGE_CYCMD);
            return 0;
        } else if (config_status != CONFIG_ERROR) {
            log(ERROR, "Unknown error reading configuration.");
        }
        // Fatal error loading config file
        return 1;
    }

    std::string filename;
    int optind = config_status;
    if ((argc - optind) == 1) {
        filename = argv[optind];
    } else {
        usage(argv[0]);
        return 1;
    }

    std::string server;
    readConfigItem("client", "serverhost", server);

    int useslave = 0;
    readConfigItem("client", "useslave", useslave);

    AtlasStreamClient bridge;
    std::string localSocket;
    if (useslave != 0) {
        localSocket = slave_socket_name;
    } else {
        localSocket = client_socket_name;
    }

    std::cout << "Attempting local connection" << std::flush;
    if (bridge.connectLocal(localSocket) == 0) {
        if (bridge.create("sys", create_session_username(),
                String::compose("%1%2", ::rand(), ::rand())) != 0) {
            std::cerr << "Could not create sys account." << std::endl
                    << std::flush;
            return -1;
        }
        std::cout << " done." << std::endl << std::flush;
        auto loginInfo = bridge.getInfoReply();
        const std::string accountId = loginInfo->getId();

        std::cout << "Attempting creation of agent" << std::flush;
        std::string agent_id;
        bridge.runTask(new AgentCreationTask(accountId, "creator", agent_id),
                "");
        if (bridge.pollUntilTaskComplete() != 0) {
            std::cerr << "Could not create agent." << std::endl << std::flush;
            return -1;
        }
        if (agent_id == "") {
            std::cerr << "Could not create agent; no id received." << std::endl
                    << std::flush;
            return -1;
        }
        std::cout << "done." << std::endl << std::flush;

        std::cout << "Starting export" << std::flush;

        //Ownership of this is transferred to the bridge when it's run, so we shouldn't delete it
        auto exporter = new EntityExporter(accountId, agent_id);
        exporter->setExportRules(rules);
        exporter->setExportTransient(transients);
        exporter->setExportMinds(minds);

        bridge.runTask(exporter, filename);
        if (bridge.pollUntilTaskComplete() != 0) {
            std::cerr << "Could not export." << std::endl << std::flush;
            return -1;
        }
        std::cout << " done." << std::endl << std::flush;
        return 0;
    }

    return 0;

}

