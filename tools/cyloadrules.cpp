// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001-2004 Alistair Riddoch
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


/// \page cyloadrules_index
///
/// \section Introduction
///
/// cyloadrules is a non-interactive commandline tool to copy rule data to
/// the database from a file. For information on the usage, please see the unix
/// manual page. The manual page is generated from docbook sources, so can
/// also be converted into other formats.


#include "common/Storage.h"
#include "common/globals.h"
#include "common/log.h"

#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Codecs/XML.h>

#include <string>
#include <fstream>
#include <iostream>

#include <sys/types.h>
// #ifdef HAVE_DIRENT_H
#include <dirent.h>
// #endif // HAS_DIRENT_H

using Atlas::Message::Element;
using Atlas::Message::MapType;

/// \brief Class that handles reading in an Atlas file, and loading the
/// contents into the rules database.
class DatabaseFileLoader : public Atlas::Message::DecoderBase {
    std::fstream m_file;
    Storage & m_db;
    Atlas::Codecs::XML m_codec;
    int m_count;

    virtual void messageArrived(MapType omap) {
        MapType::const_iterator I = omap.find("id");
        if (I == omap.end()) {
            std::cerr << "Found rule with no id" << std::endl << std::flush;
            return;
        }
        if (!I->second.isString()) {
            std::cerr << "Found rule with non string id" << std::endl << std::flush;
            return;
        }
        m_count++;
        m_db.storeInRules(omap, I->second.asString());
    }
  public:
    DatabaseFileLoader(const std::string & filename, Storage & db) :
                m_file(filename.c_str(), std::ios::in), m_db(db),
                m_codec(m_file, m_file, *this), m_count(0)
    {
    }

    void read() {
        while (!m_file.eof()) {
            m_codec.poll();
        }
    }

    void report(const std::string & set) {
        std::cout << m_count << " classes stored in rule database from "
                  << set << "."
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
    int config_status = loadConfig(argc, argv, USAGE_DBASE);
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            reportVersion(argv[0]);
            return 0;
        } else if (config_status == CONFIG_HELP) {
            showUsage(argv[0], USAGE_DBASE, "[<rulesetname> <atlas-xml-file>]");
            return 0;
        } else if (config_status != CONFIG_ERROR) {
            log(ERROR, "Unknown error reading configuration.");
        }
        // Fatal error loading config file
        return 1;
    }

    int optind = config_status;

    Storage * storage = new Storage;

    if (storage->init() != 0) {
        std::cerr << argv[0] << ": Could not make database connection."
                  << std::endl << std::flush;
        return 1;
    }

    if (optind == (argc - 2)) {
        DatabaseFileLoader f(argv[optind + 1], *storage);
        if (!f.isOpen()) {
            std::cerr << "ERROR: Unable to open file " << argv[optind + 1]
                      << std::endl << std::flush;
            return 1;
        }
        storage->setRuleset(argv[optind]);
        f.read();
        f.report(argv[optind]);
    } else if (optind == argc) {
        storage->clearRules();
        std::cout << "Reading rules from " << ruleset_name << std::endl << std::flush;
        std::string filename;

        std::string dirname = etc_directory + "/cyphesis/" + ruleset_name + ".d";
        DIR * rules_dir = ::opendir(dirname.c_str());
        if (rules_dir == 0) {
            filename = etc_directory + "/cyphesis/" + ruleset_name + ".xml";
            DatabaseFileLoader f(filename, *storage);
            if (f.isOpen()) {
                std::cerr << "WARNING: Reading legacy rule data from \""
                          << filename << "\""
                          << std::endl << std::flush;
                storage->setRuleset(ruleset_name);
                f.read();
                f.report(ruleset_name);
            }
        } else {
            while (struct dirent * rules_entry = ::readdir(rules_dir)) {
                if (rules_entry->d_name[0] == '.') {
                    continue;
                }
                filename = dirname + "/" + rules_entry->d_name;
            
                DatabaseFileLoader f(filename, *storage);
                if (!f.isOpen()) {
                    std::cerr << "ERROR: Unable to open file " << filename
                              << std::endl << std::flush;
                } else {
                    storage->setRuleset(rules_entry->d_name);
                    f.read();
                    f.report(rules_entry->d_name);
                }
            }
        }
    } else {
        usage(argv[0]);
        return 1;
    }

    delete storage;
}
