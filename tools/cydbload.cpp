// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

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

    void storeInWorld(const Atlas::Message::Object & o, const char * key) {
        putObject(world_db, o, key);
    }
};

class FileDecoder : public Atlas::Message::DecoderBase {
    ifstream m_file;
    WorldBase * m_db;
    Atlas::Codecs::XML m_codec;
    int m_count;

    virtual void ObjectArrived(const Atlas::Message::Object& obj) {
        if (!obj.IsMap()) {
            cerr << "ERROR: Not map object in file" << endl << flush;
            return;
        }
        Atlas::Message::Object::MapType omap = obj.AsMap();
        if (omap.find("id") == omap.end()) {
            cerr << "WARNING: Object in file has no id. Not stored."
                 << endl << flush;
            return;
        }
        m_count++;
        const string & id = omap["id"].AsString();
        m_db->storeInWorld(obj, id.c_str());
    }
  public:
    FileDecoder(const string & filename, WorldBase * db) :
                m_file(filename.c_str()), m_db(db),
                m_codec((iostream&)m_file, this), m_count(0) {
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

int main(int argc, char ** argv)
{
    WorldBase * db = WorldBase::instance();
    db->initWorld(true);

    FileDecoder f("foo", db);
    f.read();
    f.report();
    db->shutdownWorld();
}

#else // HAVE_LIBDB_CXX

int main(int argc, char ** argv)
{
    std::cerr << "This version of cyphesis was built without persistant world support" << endl << flush;
    exit(0);
}

#endif // HAVE_LIBDB_CXX
