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

#include "EntityImporter.h"
#include "AgentCreationTask.h"
#include "EntityTraversalTask.h"
#include "WaitForDeletionTask.h"

#include <varconf/config.h>

using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Talk;
using Atlas::Objects::Operation::Get;
using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

//Note that we check for the existence of these keys instead
//of inspecting the registered variables, since we want to make it easy
//for users, just having them supply the flag rather than setting the
//value to 1. I.e. rather "--clear" than "--clear=1"
BOOL_OPTION(_clear, false, "", "clear",
            "Delete all existing entities before importing.")
BOOL_OPTION(_merge, false, "", "merge",
            "Try to merge contents in export with existing entities.")
BOOL_OPTION(_resume, false, "", "resume",
            "If the world is suspended, resume after import.")
BOOL_OPTION(_suspend, false, "", "suspend",
            "Suspend the world after import.")

static void usage(char* prg)
{
    std::cerr << "usage: " << prg << " [options] filepath" << std::endl
              << std::flush;
}

int main(int argc, char** argv)
{
    setLoggingPrefix("IMPORT");

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

    log(NOTICE, "Attempting local connection");
    if (bridge.connectLocal(localSocket) == 0) {
        if (bridge.create("sys", create_session_username(),
                          String::compose("%1%2", ::rand(), ::rand())) != 0) {
            log(ERROR, "Could not create sys account.");
            return -1;
        }
        auto loginInfo = bridge.getInfoReply();
        std::string accountId = loginInfo->getId();
        auto accountName = loginInfo->getName();

        log(NOTICE, "Attempting creation of agent");
        auto agentCreationTask = std::make_shared<AgentCreationTask>(accountId, accountName, "creator");
        bridge.runTask(agentCreationTask, "");
        if (bridge.pollUntilTaskComplete() != 0) {
            log(ERROR, "Could not create agent.");
            return -1;
        }
        if (!agentCreationTask->m_agent_id || !agentCreationTask->m_mind_id) {
            log(ERROR, "Could not create agent; no id received.");
            return -1;
        }

        auto agent_id = *agentCreationTask->m_agent_id;
        auto mind_id = *agentCreationTask->m_mind_id;

        //Check to see if the world is empty. This is done by looking for any entity that's not
        //the root one and that isn't transient.
        bool clear = varconf::Config::inst()->find("", "clear");
        bool merge = varconf::Config::inst()->find("", "merge");

        bool resume = varconf::Config::inst()->find("", "resume");
        bool suspend = varconf::Config::inst()->find("", "suspend");

        if (clear && merge) {
            std::cerr
                    << "'--clear' and '--merge' are mutually exclusive; you can't specify both."
                    << std::endl << std::flush;
            return -1;
        }

        if (!clear && !merge) {
            bool isPopulated = false;
            std::function<bool(const RootEntity&)> visitor =
                    [&](const RootEntity& entity) -> bool {
                        if (entity->getId() != "0" && entity->getId() != agent_id && !entity->hasAttr("transient")) {
                            isPopulated = true;
                            return false;
                        }
                        return true;
                    };

            log(NOTICE, "Checking if world already is populated.");
            auto populationCheck = std::make_shared<EntityTraversalTask>(accountId, visitor);
            bridge.runTask(populationCheck, "0");
            if (bridge.pollUntilTaskComplete() != 0) {
                std::cerr
                        << "Error when checking if the server already is populated."
                        << std::endl << std::flush;
                return -1;
            }

            if (isPopulated) {
                std::cerr << "Server is already populated, aborting.\n"
                             "Either use the "
                             "'--clear' flag to first clear it. This "
                             "will delete all existing entities.\nOr "
                             "use the '--merge' flag to merge the "
                             "entities in the export with the existing"
                             " ones. The results of this are not always"
                             " predictable though." << std::endl << std::flush;
                return -1;
            }
        }

        if (clear) {
            log(NOTICE, "Clearing world first.");
            //Tell the world to clear itself
            Anonymous deleteArg;
            deleteArg->setId("0");
            deleteArg->setAttr("force", 1);
            Atlas::Objects::Operation::Delete deleteOp;
            deleteOp->setTo("0");
            deleteOp->setFrom(mind_id);
            deleteOp->setArgs1(deleteArg);

            bridge.send(deleteOp);

            log(NOTICE, "Waiting for world to be cleared.");
            //Wait for the agent to be deleted.
            bridge.runTask(std::make_shared<WaitForDeletionTask>(agent_id), "");
            if (bridge.pollUntilTaskComplete() != 0) {
                std::cerr << "Error when waiting for world to be cleared." << std::endl
                          << std::flush;
                return -1;
            }

            log(NOTICE, "World is cleared; creating new agent.");

            //Once the world has been cleared we need to create a new agent,
            //since the first one got deleted
            agentCreationTask = std::make_shared<AgentCreationTask>(accountId, accountName, "creator");
            bridge.runTask(agentCreationTask, "");
            if (bridge.pollUntilTaskComplete() != 0) {
                std::cerr << "Could not create agent." << std::endl
                          << std::flush;
                return -1;
            }
            if (!agentCreationTask->m_agent_id || !agentCreationTask->m_mind_id) {
                std::cerr << "Could not create agent; no id received."
                          << std::endl << std::flush;
                return -1;
            }
            agent_id = *agentCreationTask->m_agent_id;
            mind_id = *agentCreationTask->m_mind_id;
        }

        log(NOTICE, "Starting import.");

        auto importer = std::make_shared<EntityImporter>(accountId, mind_id);

        importer->setResume(resume);
        importer->setSuspend(suspend);
        importer->setAlwaysCreateNewEntities(clear);

        bridge.runTask(importer, filename);
        if (bridge.pollUntilTaskComplete() != 0) {
            std::cerr << "Could not import." << std::endl << std::flush;
            return -1;
        }

        log(INFO, "Import done.");
        return 0;
    }

    return 0;

}

