// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Message/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>

#include <common/Database.h>
#include <common/globals.h>

#include <string>
#include <fstream>

using Atlas::Message::Object;

class WorldBase {
  protected:
    WorldBase() : m_connection(*Database::instance()) { }

    Database & m_connection;
    static WorldBase * m_instance;
  public:
    ~WorldBase() {
        m_connection.shutdownConnection();
    }

    static WorldBase * instance() {
        if (m_instance == NULL) {
            m_instance = new WorldBase();
            m_instance->m_connection.initConnection(true);
            m_instance->m_connection.initWorld(true);
        }
        return m_instance;
    }

    void storeInWorld(const Object::MapType & o, const std::string & key) {
        m_connection.putObject(m_connection.world(), key, o);
    }

    void updateInWorld(const Object::MapType & o, const std::string & key) {
        m_connection.updateObject(m_connection.world(), key, o);
    }

    bool getWorld(Object::MapType & o) {
        return m_connection.getObject(m_connection.world(), "world_0", o);
    }
};

WorldBase * WorldBase::m_instance = NULL;

class FileDecoder : public Atlas::Message::DecoderBase {
    std::ifstream m_file;
    WorldBase & m_db;
    Atlas::Codecs::XML m_codec;
    Object::MapType m_world;
    int m_count;
    bool m_worldMerge;

    virtual void ObjectArrived(const Object & obj) {
        const Object::MapType & omap = obj.AsMap();
        Object::MapType::const_iterator I;
        if ((I = omap.find("id")) == omap.end()) {
            std::cerr << "WARNING: Object in file has no id. Not stored."
                      << std::endl << std::flush;
            return;
        }
        m_count++;
        const std::string & id = I->second.AsString();
        if (m_worldMerge && (id == "world_0")) {
            std::cout << "Merging into existing world object" << std::endl << std::flush;
            if (((I = omap.find("contains")) != omap.end()) &&
                (I->second.IsList())) {
                const Object::ListType & contlist = I->second.AsList();
                Object::ListType & worldlist = m_world.find("contains")->second.AsList();
                Object::ListType::const_iterator J = contlist.begin();
                for (;J != contlist.end(); ++J) {
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
    FileDecoder(const std::string & filename, WorldBase & db) :
                m_file(filename.c_str()), m_db(db),
                m_codec((std::iostream&)m_file, this), m_count(0)
    {
        m_worldMerge = db.getWorld(m_world);
        if (m_worldMerge && !m_world.find("contains")->second.IsList()) {
            std::cout << "WARNING: Current database world object has no contains list, so so it is being replaced" << std::endl << std::flush;
            m_worldMerge = false;
            
        }
    }

    void read() {
        while (!m_file.eof()) {
            m_codec.Poll();
        }
    }

    void report() {
        std::cout << m_count << " objects stored in world database."
                  << std::endl << std::flush;
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

    WorldBase & db = *WorldBase::instance();

    FileDecoder f(argv[1], db);
    f.read();
    f.report();
    delete &db;
}
