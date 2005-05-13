// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001-2005 Alistair Riddoch

#include "AdminClient.h"

#include "common/Database.h"
#include "common/globals.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>

#include <string>
#include <fstream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

class FileDecoder : public Atlas::Message::DecoderBase
{
    std::fstream m_file;
    std::string m_ruleset;
    AdminClient & m_client;
    Atlas::Codecs::XML m_codec;
    int m_count;
    int m_total;

    virtual void objectArrived(const Element & obj) {
        const MapType & omap = obj.asMap();
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
        if (m_client.uploadRule(I->second.asString(), m_ruleset, omap) == 0) {
            m_count++;
        }
    }
  public:
    FileDecoder(const std::string & filename, const std::string & ruleset,
                AdminClient & client) :
                m_file(filename.c_str(), std::ios::in),
                m_ruleset(ruleset), m_client(client),
                m_codec(m_file, this), m_count(0), m_total(0)
    {
    }

    void read() {
        while (!m_file.eof()) {
            m_codec.poll();
        }
    }

    void report() {
        std::cout << m_count << " new classes uploaded out of "
                  << m_total << " loaded from file."
                  << std::endl << std::flush;
    }

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
    int optind;

    if ((optind = loadConfig(argc, argv)) < 0) {
        // Fatal error loading config file
        return 1;
    }

    std::string server;
    if (global_conf->findItem("client", "serverhost")) {
        server = global_conf->getItem("client", "serverhost").as_string();
    }

    int useslave = 0;
    if (global_conf->findItem("client", "useslave")) {
        useslave = global_conf->getItem("client", "useslave");
    }

    AdminClient bridge;

    if (server.empty()) {
        std::string localSocket = var_directory + "/tmp/";
        if (useslave != 0) {
            localSocket += slave_socket_name;
        } else {
            localSocket += client_socket_name;
        }

        if (bridge.connect_unix(localSocket) == 0) {
            bridge.setUsername("admin");

            if (bridge.login() != 0) {
                std::cerr << "Login failed." << std::endl << std::flush;
                bridge.getLogin();

                if (!bridge.login()) {
                    std::cerr << "Login failed." << std::endl << std::flush;
                    return 1;
                }
            }
        } else {
            // FIXME This won't work.
            server = "localhost";
        }
    }

    if (optind == (argc - 2)) {
        FileDecoder f(argv[optind + 1], argv[optind], bridge);
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
            FileDecoder f(filename, *I, bridge);
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
}
