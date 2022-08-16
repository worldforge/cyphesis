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

static void usage(char* prg)
{
    std::cerr << "usage: " << prg << " [options] filepath" << std::endl;
}

BOOL_OPTION(transients, false, "export", "transients", "Flag to control if transients should also be exported");
BOOL_OPTION(minds, true, "export", "minds", "Flag to control if minds should also be exported");

int main(int argc, char** argv)
{
    setLoggingPrefix("EXPORT");

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
    int optindex = config_status;
    if ((argc - optindex) == 1) {
        filename = argv[optindex];
    } else {
        usage(argv[0]);
        return 1;
    }

    std::string server;
    readConfigItem("client", "serverhost", server);

    int useslave = 0;
    readConfigItem("client", "useslave", useslave);

    Atlas::Objects::Factories factories;

    boost::asio::io_context io_context;
    AtlasStreamClient bridge(io_context, factories);
    std::string localSocket;
    if (useslave != 0) {
        localSocket = slave_socket_name;
    } else {
        localSocket = client_socket_name;
    }

    std::cout << "Attempting local connection" << std::endl;
    if (bridge.connectLocal(localSocket) == 0) {
        if (bridge.create("sys", create_session_username(), String::compose("%1%2", ::rand(), ::rand())) != 0) {
            std::cerr << "Could not create sys account." << std::endl;
            return -1;
        }
        std::cout << " done." << std::endl;
        auto loginInfo = bridge.getInfoReply();
        auto accountId = loginInfo->getId();
        auto accountName = loginInfo->getName();

        std::cout << "Attempting creation of agent" << std::endl;
        auto agentCreationTask = std::make_shared<AgentCreationTask>(accountId, accountName, "creator");
        bridge.runTask(agentCreationTask, "");
        if (bridge.pollUntilTaskComplete() != 0) {
            std::cerr << "Could not create agent." << std::endl;
            return -1;
        }
        if (!agentCreationTask->m_agent_id || !agentCreationTask->m_mind_id) {
            std::cerr << "Could not create agent; no id received." << std::endl;
            return -1;
        }
        std::cout << "done." << std::endl;

        auto agent_id = *agentCreationTask->m_agent_id;
        auto mind_id = *agentCreationTask->m_mind_id;
        std::cout << "done." << std::endl;

        std::cout << "Starting export" << std::endl;

        // Ownership of this is transferred to the bridge when it's run, so we shouldn't delete it
        auto exporter = std::make_shared<EntityExporter>(accountId, mind_id);
        exporter->setExportTransient(transients);

        bridge.runTask(exporter, filename);
        if (bridge.pollUntilTaskComplete() != 0) {
            std::cerr << "Could not export." << std::endl;
            return -1;
        }
        std::cout << " done." << std::endl;
        return 0;
    }

    return 0;
}
