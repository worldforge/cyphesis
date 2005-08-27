// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "common/Database.h"
#include "common/globals.h"

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
using Atlas::Message::ListType;

class RuleReader {
  protected:
    RuleReader() : m_connection(*Database::instance()) { }

    Database & m_connection;
    static RuleReader * m_instance;
    std::string m_rulesetName;
  public:
    ~RuleReader() {
        m_connection.shutdownConnection();
    }

    static RuleReader * instance() {
        if (m_instance == NULL) {
            m_instance = new RuleReader();
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

    void readRuleTable(const std::string & ruleset, MapType & o) {
        std::stringstream query;
        query << "SELECT * FROM " << m_connection.rule() << " WHERE "
              << " ruleset = '" << ruleset << "'";
        DatabaseResult res = m_connection.runSimpleSelectQuery(query.str());
        DatabaseResult::const_iterator I = res.begin();
        DatabaseResult::const_iterator Iend = res.end();
        for (; I != Iend; ++I) {
            MapType & data = (o[I.column("id")] = MapType()).asMap();
            m_connection.decodeObject(I.column("contents"), data);
        }
    }
};

RuleReader * RuleReader::m_instance = NULL;

static void usage(char * prgname)
{
    std::cerr << "usage: " << prgname << std::endl << std::flush;
}

int main(int argc, char ** argv)
{
    int optind;

    if ((optind = loadConfig(argc, argv)) < 0) {
        // Fatal error loading config file
        return 1;
    }

    RuleReader * db = RuleReader::instance();

    if (db == 0) {
        std::cerr << argv[0] << ": Could not make database connection."
                  << std::endl << std::flush;
        return 1;
    }

    Atlas::Message::QueuedDecoder decoder;

    if (optind == argc) {
        std::vector<std::string>::const_iterator I = rulesets.begin();
        std::vector<std::string>::const_iterator Iend = rulesets.end();
        for (; I != Iend; ++I) {
            std::cout << "Dumping rules from " << *I << std::endl << std::flush;

            MapType rule_store;

            db->readRuleTable(*I, rule_store);

            std::fstream file;
            std::string filename = *I + ".xml";
           
            file.open(filename.c_str(), std::ios::out);
            
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

            std::cout << rule_store.size() << " classes stores in " << filename
                      << std::endl << std::flush;
        }
    } else {
        usage(argv[0]);
        return 1;
    }

    delete db;
    return 0;
}
