// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Message/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>

#include <common/config.h>
#include <common/database.h>

#include <string>
#include <fstream>

#ifdef HAVE_LIBDB_CXX

using Atlas::Message::Object;

class WorldBase : public Database {
  protected:
    WorldBase() { }

  public:
    static WorldBase * instance() {
        if (m_instance == NULL) {
            m_instance = new WorldBase();
        }
        return (WorldBase *)m_instance;
    }

    void storeInWorld(const Object::MapType & o, const char * key) {
        putObject(world_db, o, key);
    }
    bool getWorld(Object::MapType & o) {
        return getObject(world_db, "world_0", o);
    }
};

class FileDecoder : public Atlas::Message::DecoderBase {
    ifstream m_file;
    WorldBase * m_db;
    Atlas::Codecs::XML m_codec;
    Object::MapType m_world;
    int m_count;
    bool m_worldMerge;

    virtual void ObjectArrived(const Object & obj) {
        const Object::MapType & omap = obj.AsMap();
        Object::MapType::const_iterator I;
        if ((I = omap.find("id")) == omap.end()) {
            std::cerr << "WARNING: Object in file has no id. Not stored."
                      << endl << flush;
            return;
        }
        m_count++;
        const std::string & id = I->second.AsString();
        if (m_worldMerge && (id == "world_0")) {
            std::cout << "Merging into existing world object" << endl << flush;
            if (((I = omap.find("contains")) != omap.end()) &&
                (I->second.IsList())) {
                const Object::ListType & contlist = I->second.AsList();
                Object::ListType & worldlist = m_world.find("contains")->second.AsList();
                Object::ListType::const_iterator J = contlist.begin();
                for (;J != contlist.end(); ++J) {
                    worldlist.push_back(*J);
                }
                m_db->storeInWorld(m_world, id.c_str());
            } else {
                std::cout << "WARNING: New world object has no contains list, so no ids are being merged" << endl << flush;
            }
        } else {
            m_db->storeInWorld(omap, id.c_str());
        }
    }
  public:
    FileDecoder(const std::string & filename, WorldBase * db) :
                m_file(filename.c_str()), m_db(db),
                m_codec((iostream&)m_file, this), m_count(0)
    {
        m_worldMerge = db->getWorld(m_world);
        if (m_worldMerge && !m_world.find("contains")->second.IsList()) {
            std::cout << "WARNING: Current database world object has no contains list, so so it is being replaced" << endl << flush;
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
                  << endl << flush;
    }
};

static void usage(char * prgname)
{
    std::cout << "usage: " << prgname << " <atlas map file>" << endl << flush;
    return;
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    WorldBase * db = WorldBase::instance();
    db->initWorld(true);

    FileDecoder f(argv[1], db);
    f.read();
    f.report();
    db->shutdownWorld();
    delete db;
}

#else // HAVE_LIBDB_CXX

int main(int argc, char ** argv)
{
    std::cerr << "This version of cyphesis was built without persistant world support" << endl << flush;
    exit(0);
}

#endif // HAVE_LIBDB_CXX
