// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001-2005 Alistair Riddoch
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

// $Id: cyaddrules.cpp,v 1.11 2007-04-29 13:32:31 alriddoch Exp $

/// \page cyaddrules_index
///
/// \section Introduction
///
/// cyaddrules is a non-interactive commandline tool to upload rules files
/// into a running server. For information on the usage, please see the unix
/// manual page. The manual page is generated from docbook sources, so can
/// also be converted into other formats.
///
/// The code to interact with the server is encapsulated in the AdminClient
/// class, which is shared with other tools.

#include "AdminClient.h"

#include "common/Database.h"
#include "common/globals.h"
#include "common/log.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>

#include <varconf/Config.h>

#include <string>
#include <fstream>

using Atlas::Message::MapType;

/// \brief Class that handles reading in a rules file, and loading the
/// contents to the server via the AdminClient.
class ServerRulesFileLoader : public Atlas::Message::DecoderBase
{
    /// \brief iostream for accessing the rules file.
    std::fstream m_file;

    /// \brief Name of the ruleset to be loaded.
    std::string m_ruleset;

    /// \brief Client object to handle the connection to the server.
    AdminClient & m_client;

    /// \brief Atlas codec used to decode the contents of the rules file.
    Atlas::Codecs::XML m_codec;

    /// \brief Count of classes uploaded to the server.
    int m_count;

    /// \brief Count of classes read from the file.
    int m_total;

    /// \brief Method called from the base class when a complete message
    /// is read from the file.
    virtual void messageArrived(const MapType & omap) {
        MapType::const_iterator I = omap.find("id");
        if (I == omap.end()) {
            std::cerr << "Found rule with no id" << std::endl << std::flush;
            return;
        }
        if (!I->second.isString()) {
            std::cerr << "Found rule with non string id" << std::endl << std::flush;
            return;
        }
        m_total++;
        // m_rules[I->second.asString()] = obj.asMap();
        int ret = m_client.uploadRule(I->second.asString(), m_ruleset, omap);
        if (ret > 0) {
            m_count += ret;
        }
    }
  public:
    /// \brief ServerRulesFileLoader constructor
    ///
    /// @param filename name of the rules file to be loaded
    /// @param ruleset name of the ruleset the file represents
    /// @param client client object that uploads rules to the server
    ServerRulesFileLoader(const std::string & filename,
                          const std::string & ruleset,
                          AdminClient & client) :
                          m_file(filename.c_str(), std::ios::in),
                          m_ruleset(ruleset), m_client(client),
                          m_codec(m_file, *this), m_count(0), m_total(0)
    {
    }

    /// \brief Read the contents of the file to the end
    void read() {
        while (!m_file.eof()) {
            m_codec.poll();
        }
    }

    /// \brief Send a report of rules laoded and uploaded to standard out
    void report() {
        std::cout << m_count << " new classes uploaded out of "
                  << m_total << " loaded from file."
                  << std::endl << std::flush;
    }

    /// \brief Indicate whether the file has been opened successfully
    bool isOpen() {
        return m_file.is_open();
    }
};

static void usage(char * prgname)
{
    std::cerr << "usage: " << prgname << " [<rulesetname> <atlas-xml-file>]" << std::endl << std::flush;
}

int main(int argc, char ** argv)
{
    int config_status = loadConfig(argc, argv, true);
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            reportVersion(argv[0]);
            return 0;
        } else if (config_status == CONFIG_HELP) {
            showUsage(argv[0], USAGE_CYCMD, "[<rulesetname> <atlas-xml-file>]");
            return 0;
        } else if (config_status != CONFIG_ERROR) {
            log(ERROR, "Unknown error reading configuration.");
        }
        // Fatal error loading config file
        return 1;
    }

    int optind = config_status;

    AdminClient bridge;

    std::string server;
    if (global_conf->findItem("client", "serverhost")) {
        server = global_conf->getItem("client", "serverhost").as_string();
    }

    int useslave = 0;
    if (global_conf->findItem("client", "useslave")) {
        useslave = global_conf->getItem("client", "useslave");
    }

    if (global_conf->findItem("client", "account")) {
        bridge.setUsername(global_conf->getItem("client", "account").as_string());
    } else {
        bridge.setUsername("admin");
    }


    if (global_conf->findItem("client", "password")) {
        bridge.setPassword(global_conf->getItem("client", "password").as_string());
    }

    if (server.empty()) {
        std::string localSocket = var_directory + "/tmp/";
        if (useslave != 0) {
            localSocket += slave_socket_name;
        } else {
            localSocket += client_socket_name;
        }

        if (bridge.connect_unix(localSocket) != 0) {
            std::cerr << "Failed to connect to local server"
                      << std::endl << std::flush;
            return 1;
        }
    } else {
        if (bridge.connect(server) != 0) {
            std::cerr << "Failed to connect to remote server"
                      << std::endl << std::flush;
            return 1;
        }
    }

    if (bridge.login() != 0) {
        std::cerr << "Login failed." << std::endl << std::flush;
        bridge.getLogin();

        if (!bridge.login()) {
            std::cerr << "Login failed." << std::endl << std::flush;
            return 1;
        }
    }

    if (optind == (argc - 2)) {
        ServerRulesFileLoader f(argv[optind + 1], argv[optind], bridge);
        if (!f.isOpen()) {
            std::cerr << "ERROR: Unable to open file " << argv[optind + 1]
                      << std::endl << std::flush;
            return 1;
        }
        f.read();
        f.report();
    } else if (optind == argc) {
        std::vector<std::string>::const_iterator I = rulesets.begin();
        std::vector<std::string>::const_iterator Iend = rulesets.end();
        for (; I != Iend; ++I) {
            std::cout << "Reading rules from " << *I << std::endl << std::flush;
            std::string filename = etc_directory + "/cyphesis/" + *I + ".xml";
            ServerRulesFileLoader f(filename, *I, bridge);
            if (!f.isOpen()) {
                std::cerr << "ERROR: Unable to open file " << filename
                          << std::endl << std::flush;
                return 1;
            }
            f.read();
            f.report();
        }
    } else {
        usage(argv[0]);
        return 1;
    }

    bridge.report();

    return 0;
}
