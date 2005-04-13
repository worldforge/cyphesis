// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "common/Database.h"
#include "common/globals.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>

#include <string>
#include <fstream>

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

    void storeInRules(const MapType & o, const std::string & key) {
        if (m_connection.hasKey(m_connection.rule(), key)) {
            return;
        }
        m_connection.putObject(m_connection.rule(), key, o, StringVector(1, m_rulesetName));
        if (!m_connection.clearPendingQuery()) {
            std::cerr << "Failed" << std::endl << std::flush;
        }
    }
    bool clearRules() {
        return (m_connection.clearTable(m_connection.rule()) &&
                m_connection.clearPendingQuery());
    }
    void setRuleset(const std::string & n) {
        m_rulesetName = n;
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

    if (optind == argc) {
        std::vector<std::string>::const_iterator I = rulesets.begin();
        std::vector<std::string>::const_iterator Iend = rulesets.end();
        for (; I != Iend; ++I) {
        }
    } else {
        usage(argv[0]);
        return 1;
    }

    delete db;
    return 0;
}
