// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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

// $Id: cydumprules.cpp,v 1.15 2007-12-07 17:42:59 alriddoch Exp $

/// \page cydumprules_index
///
/// \section Introduction
///
/// cydumprules is a non-interactive commandline tool to copy rule data from
/// the database to a file. For information on the usage, please see the unix
/// manual page. The manual page is generated from docbook sources, so can
/// also be converted into other formats.

#include "common/Database.h"
#include "common/globals.h"
#include "common/log.h"

#include <Atlas/Formatter.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>
#include <Atlas/Message/MEncoder.h>
#include <Atlas/Message/QueuedDecoder.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;

/// \brief Class to handle reading rules from the Database rules table.
class RuleReader {
  protected:
    /// \brief RuleReader constructor
    RuleReader() : m_connection(*Database::instance()) { }

    /// \brief Connection to the database.
    Database & m_connection;

    /// \brief Singleton instance of RuleReader
    static RuleReader * m_instance;
  public:
    ~RuleReader() {
        m_connection.shutdownConnection();
    }

    /// \brief Return a singleton instance of RuleReader
    static RuleReader * instance() {
        if (m_instance == NULL) {
            m_instance = new RuleReader();
            if (m_instance->m_connection.initConnection() != 0) {
                delete m_instance;
                m_instance = 0;
            } else if (!m_instance->m_connection.initRule(true)) {
                delete m_instance;
                m_instance = 0;
            }
        }
        return m_instance;
    }

    /// \brief Read all the rules sets from the rules table
    ///
    void readRuleTableSets(std::set<std::string> & sets) {
        std::stringstream query;
        query << "SELECT ruleset FROM " << m_connection.rule();
        DatabaseResult res = m_connection.runSimpleSelectQuery(query.str());
        DatabaseResult::const_iterator I = res.begin();
        DatabaseResult::const_iterator Iend = res.end();
        for (; I != Iend; ++I) {
            std::string ruleset_name = I.column("ruleset");
            if (sets.find(ruleset_name) == sets.end()) {
                sets.insert(ruleset_name);
            }
        }
    }

    /// \brief Read all the rules in one ruleset from the rules table.
    ///
    /// @param ruleset the name of the ruleset to be read.
    /// @param o Atlas map to store the rules in.
    void readRuleTable(const std::string & ruleset, MapType & o) {
        std::stringstream query;
        query << "SELECT * FROM " << m_connection.rule() << " WHERE "
              << " ruleset = '" << ruleset << "'";
        DatabaseResult res = m_connection.runSimpleSelectQuery(query.str());
        DatabaseResult::const_iterator I = res.begin();
        DatabaseResult::const_iterator Iend = res.end();
        for (; I != Iend; ++I) {
            MapType & data = (o[I.column("id")] = MapType()).asMap();
            m_connection.decodeMessage(I.column("contents"), data);
        }
    }
};

RuleReader * RuleReader::m_instance = NULL;

int main(int argc, char ** argv)
{
    int config_status = loadConfig(argc, argv, USAGE_DBASE);
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            reportVersion(argv[0]);
            return 0;
        } else if (config_status == CONFIG_HELP) {
            showUsage(argv[0], USAGE_DBASE);
            return 0;
        } else if (config_status != CONFIG_ERROR) {
            log(ERROR, "Unknown error reading configuration.");
        }
        // Fatal error loading config file
        return 1;
    } 

    if (config_status != argc) {
        showUsage(argv[0], USAGE_DBASE);
        return 1;
    }

    RuleReader * db = RuleReader::instance();

    if (db == 0) {
        std::cerr << argv[0] << ": Could not make database connection."
                  << std::endl << std::flush;
        return 1;
    }

    Atlas::Message::QueuedDecoder decoder;

    MapType rule_store;
    std::set<std::string> rulesets;

    db->readRuleTableSets(rulesets);

    std::set<std::string>::const_iterator I = rulesets.begin();
    std::set<std::string>::const_iterator Iend = rulesets.end();
    for(; I != Iend; ++I) {
        const std::string & ruleset = *I;

        std::cout << "Dumping rules from " << ruleset
                  << std::endl << std::flush;

        db->readRuleTable(ruleset, rule_store);

        std::fstream file;
   
        file.open(ruleset.c_str(), std::ios::out);
    
        Atlas::Codecs::XML codec(file, decoder);
        Atlas::Formatter formatter(file, codec);
        Atlas::Message::Encoder encoder(formatter);

        formatter.streamBegin();

        MapType::const_iterator J = rule_store.begin();
        MapType::const_iterator Jend = rule_store.end();
        for (; J != Jend; ++J) {
            if (!J->second.isMap()) {
                std::cerr << "WARNING: Non map rule found in database"
                          << std::endl << std::flush;
                continue;
            }
            encoder.streamMessageElement(J->second.asMap());
        }

        formatter.streamEnd();

        std::cout << rule_store.size() << " classes stores in " << ruleset
                  << std::endl << std::flush;
    }

    delete db;
    return 0;
}
