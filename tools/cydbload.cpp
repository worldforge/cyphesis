// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#error This file has been removed from the build.

#include "common/Database.h"
#include "common/globals.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>

#include <fstream>

using Atlas::Message::MapType;
using Atlas::Message::ListType;

class WorldAccessor {
  protected:
    WorldAccessor() : m_connection(*Database::instance()) { }

    Database & m_connection;
    static WorldAccessor * m_instance;
  public:
    ~WorldAccessor() {
        m_connection.shutdownConnection();
    }

    static WorldAccessor * instance() {
        if (m_instance == NULL) {
            m_instance = new WorldAccessor();
            m_instance->m_connection.initConnection(true);
            m_instance->m_connection.initWorld(true);
        }
        return m_instance;
    }

    void storeInWorld(const MapType & o, const std::string & key) {
        m_connection.putObject(m_connection.world(), key, o);
    }

    void updateInWorld(const MapType & o, const std::string & key) {
        m_connection.updateObject(m_connection.world(), key, o);
    }

    bool getWorld(MapType & o) {
        return m_connection.getObject(m_connection.world(), "world_0", o);
    }
};

WorldAccessor * WorldAccessor::m_instance = NULL;

class FileDecoder : public Atlas::Message::DecoderBase {
    std::fstream m_file;
    WorldAccessor & m_db;
    Atlas::Codecs::XML m_codec;
    MapType m_world;
    int m_count;
    bool m_worldMerge;

    virtual void objectArrived(const Object & obj) {
        const MapType & omap = obj.asMap();
        MapType::const_iterator I;
        if ((I = omap.find("id")) == omap.end()) {
            std::cerr << "WARNING: Object in file has no id. Not stored."
                      << std::endl << std::flush;
            return;
        }
        m_count++;
        const std::string & id = I->second.asString();
        if (m_worldMerge && (id == "world_0")) {
            std::cout << "Merging into existing world object" << std::endl << std::flush;
            if (((I = omap.find("contains")) != omap.end()) &&
                (I->second.isList())) {
                const ListType & contlist = I->second.asList();
                ListType & worldlist = m_world.find("contains")->second.asList();
                ListType::const_iterator J = contlist.begin();
                ListType::const_iterator Jend = contlist.end();
                for (; J != Jend; ++J) {
                    worldlist.push_back(*J);
                }
                m_db.updateInWorld(m_world, id);
            } else {
                std::cout << "WARNING: New world object has no contains list, so no ids are being merged" << std::endl << std::flush;
            }
        } else {
            m_db.storeInWorld(omap, id);
        }
    }
  public:
    FileDecoder(const std::string & filename, WorldAccessor & db) :
                m_file(filename.c_str(), std::ios::in), m_db(db),
                m_codec(m_file, this), m_count(0)
    {
        m_worldMerge = db.getWorld(m_world);
        if (m_worldMerge && !m_world.find("contains")->second.isList()) {
            std::cout << "WARNING: Current database world object has no contains list, so so it is being replaced" << std::endl << std::flush;
            m_worldMerge = false;
            
        }
    }

    void read() {
        while (!m_file.eof()) {
            m_codec.poll();
        }
    }

    void report() {
        std::cout << m_count << " objects stored in world database."
                  << std::endl << std::flush;
    }

    bool isOpen() {
        return m_file.is_open();
    }
};

static void usage(char * prgname)
{
    std::cout << "usage: " << prgname << " <atlas map file>" << std::endl << std::flush;
    return;
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    int cargc = 0;
    char * cargv[0];

    if (loadConfig(cargc, cargv)) {
        // Fatal error loading config file
        return 1;
    }

    WorldAccessor & db = *WorldAccessor::instance();

    FileDecoder f(argv[1], db);
    if (!f.isOpen()) {
        std::cerr << "ERROR: Unable to open file " << argv[1]
                  << std::endl << std::flush;
        return 1;
    }
    f.read();
    f.report();
    delete &db;
}
