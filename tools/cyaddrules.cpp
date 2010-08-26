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

// $Id$

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

#include "common/compose.hpp"
#include "common/Database.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/sockets.h"
#include "common/system.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>

#include <varconf/config.h>

#include <string>
#include <fstream>
#include <cstdlib>

#include <dirent.h>

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
    void report(const std::string & source) {
        std::cout << m_count << " new classes uploaded out of "
                  << m_total << " loaded from " << source << "."
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

STRING_OPTION(server, "", "client", "serverhost",
              "Hostname of the server to connect to");

STRING_OPTION(username, "", "client", "account",
              "Account name to use to authenticate to the server");

STRING_OPTION(password, "", "client", "password",
              "Password to use to authenticate to the server");

INT_OPTION(useslave, 0, "client", "useslave",
           "Flag to control connecting to an AI slave server, not master world server");

int main(int argc, char ** argv)
{
    int config_status = loadConfig(argc, argv, USAGE_CYCMD);
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

    bridge.setUsername(username);

    bridge.setPassword(password);

    if (server.empty()) {
        std::string localSocket;
        if (useslave != 0) {
            localSocket = slave_socket_name;
        } else {
            localSocket = client_socket_name;
        }

        if (bridge.connectLocal(localSocket) != 0) {
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

    if (username.empty()) {
        if (bridge.create("sys",
                          create_session_username(),
                          String::compose("%1%2", ::rand(), ::rand())) != 0) {
            std::cerr << "Creation failed." << std::endl << std::flush;
            return 1;
        }
    } else {
        if (bridge.login() != 0) {
            std::cerr << "Login failed." << std::endl << std::flush;
            bridge.getLogin();

            if (!bridge.login()) {
                std::cerr << "Login failed." << std::endl << std::flush;
                return 1;
            }
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
        f.report(argv[optind]);
    } else if (optind == argc) {
        std::cout << "Reading rules from " << ruleset << std::endl << std::flush;
        std::string filename;

        std::string dirname = etc_directory + "/cyphesis/" + ruleset + ".d";
        DIR * rules_dir = ::opendir(dirname.c_str());
        if (rules_dir == 0) {
            filename = etc_directory + "/cyphesis/" + ruleset + ".xml";
            ServerRulesFileLoader f(filename, ruleset, bridge);
            if (f.isOpen()) {
                std::cerr << "WARNING: Reading legacy rule data from \""
                          << filename << "\""
                          << std::endl << std::flush;
                f.read();
                f.report(ruleset);
            }
        } else {
            while (struct dirent * rules_entry = ::readdir(rules_dir)) {
                if (rules_entry->d_name[0] == '.') {
                    continue;
                }
                filename = dirname + "/" + rules_entry->d_name;
    
                ServerRulesFileLoader f(filename, ruleset, bridge);
                if (!f.isOpen()) {
                    std::cerr << "ERROR: Unable to open file " << filename
                              << std::endl << std::flush;
                } else {
                    f.read();
                    f.report(rules_entry->d_name);
                }
            }
        }
    } else {
        usage(argv[0]);
        return 1;
    }

    bridge.report();

    return 0;
}
