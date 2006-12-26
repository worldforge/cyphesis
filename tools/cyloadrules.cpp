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

// $Id: cyloadrules.cpp,v 1.34 2006-12-26 18:24:26 alriddoch Exp $

/// \page cyloadrules_index
///
/// \section Introduction
///
/// cyloadrules is a non-interactive commandline tool to copy rule data to
/// the database from a file. For information on the usage, please see the unix
/// manual page. The manual page is generated from docbook sources, so can
/// also be converted into other formats.

#include "common/Database.h"
#include "common/globals.h"

#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Codecs/XML.h>

#include <string>
#include <fstream>
#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;

/// \brief Handle the database access to the rules table only.
class RuleBase {
  protected:
    /// \brief RuleBase constructor
    RuleBase() : m_connection(*Database::instance()) { }

    /// \brief Connection to the Database
    Database & m_connection;

    /// \brief Singleton instance of RuleBase
    static RuleBase * m_instance;

    /// \brief Name of the ruleset to be read from file
    std::string m_rulesetName;
  public:
    ~RuleBase() {
        m_connection.shutdownConnection();
    }

    /// \brief Return a singleton instance of RuleBase
    static RuleBase * instance() {
        if (m_instance == NULL) {
            m_instance = new RuleBase();
            if (m_instance->m_connection.initConnection(true) != 0) {
                delete m_instance;
                m_instance = 0;
            } else if (!m_instance->m_connection.initRule(true)) {
                delete m_instance;
                m_instance = 0;
            }
        }
        return m_instance;
    }

    /// \brief Store a rule in the database
    ///
    /// Check if the rules table contains a rule which this name, identifier.
    /// If so return without doing anything, otherwise install the rule
    /// into the database.
    /// @param rule Atlas message data describing the rule to be stored
    /// @param key identifier or name of the rule to be stored
    void storeInRules(const MapType & rule, const std::string & key) {
        if (m_connection.hasKey(m_connection.rule(), key)) {
            return;
        }
        m_connection.putObject(m_connection.rule(), key, rule, StringVector(1, m_rulesetName));
        if (!m_connection.clearPendingQuery()) {
            std::cerr << "Failed" << std::endl << std::flush;
        }
    }

    /// \brief Clear the rules table in the database, leaving it empty.
    bool clearRules() {
        return (m_connection.clearTable(m_connection.rule()) &&
                m_connection.clearPendingQuery());
    }

    /// \brief Set the ruleset name to be used when installing rules in
    /// the database.
    ///
    /// @param n name to be set.
    void setRuleset(const std::string & n) {
        m_rulesetName = n;
    }
};

RuleBase * RuleBase::m_instance = NULL;

/// \brief Class that handles reading in an Atlas file, and loading the
/// contents into the rules database.
class DatabaseFileLoader : public Atlas::Message::DecoderBase {
    std::fstream m_file;
    RuleBase & m_db;
    Atlas::Codecs::XML m_codec;
    int m_count;

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
        m_count++;
        m_db.storeInRules(omap, I->second.asString());
    }
  public:
    DatabaseFileLoader(const std::string & filename, RuleBase & db) :
                m_file(filename.c_str(), std::ios::in), m_db(db),
                m_codec(m_file, *this), m_count(0)
    {
    }

    void read() {
        while (!m_file.eof()) {
            m_codec.poll();
        }
    }

    void report() {
        std::cout << m_count << " classes stored in rule database."
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

    RuleBase * db = RuleBase::instance();

    if (db == 0) {
        std::cerr << argv[0] << ": Could not make database connection."
                  << std::endl << std::flush;
        return 1;
    }

    if (optind == (argc - 2)) {
        DatabaseFileLoader f(argv[optind + 1], *db);
        if (!f.isOpen()) {
            std::cerr << "ERROR: Unable to open file " << argv[optind + 1]
                      << std::endl << std::flush;
            return 1;
        }
        db->setRuleset(argv[optind]);
        f.read();
        f.report();
    } else if (optind == argc) {
        db->clearRules();
        std::vector<std::string>::const_iterator I = rulesets.begin();
        std::vector<std::string>::const_iterator Iend = rulesets.end();
        for (; I != Iend; ++I) {
            std::cout << "Reading rules from " << *I << std::endl << std::flush;
            std::string filename = etc_directory + "/cyphesis/" + *I + ".xml";
            DatabaseFileLoader f(filename, *db);
            if (!f.isOpen()) {
                std::cerr << "ERROR: Unable to open file " << filename
                          << std::endl << std::flush;
                return 1;
            }
            db->setRuleset(*I);
            f.read();
            f.report();
        }
    } else {
        usage(argv[0]);
        return 1;
    }

    delete db;
}
