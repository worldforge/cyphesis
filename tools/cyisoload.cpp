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
#include "sstream.h"

#include <coal/isoloader.h>

#if defined(CYPHESIS_USE_DB3) && defined(HAVE_LIBCOALISO)

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

class TemplatesLoader : public Atlas::Message::DecoderBase {
    ifstream m_file;
    Atlas::Message::Object::MapType m_db;
    Atlas::Codecs::XML m_codec;
    int m_count;

    virtual void ObjectArrived(const Atlas::Message::Object& o) {
        Atlas::Message::Object obj(o);
        if (!obj.IsMap()) {
            cerr << "ERROR: Non map object in file" << endl << flush;
            return;
        }
        Atlas::Message::Object::MapType & omap = obj.AsMap();
        if (omap.find("graphic") == omap.end()) {
            cerr<<"WARNING: Template Object in file has no graphic. Not stored."
                << endl << flush;
            return;
        }
        m_count++;
        const string & id = omap["graphic"].AsString();
        m_db[id] = obj;
    }
  public:
    TemplatesLoader(const string & filename) :
                m_file(filename.c_str()),
                m_codec((iostream&)m_file, this), m_count(0) {
    }

    void read() {
        while (!m_file.eof()) {
            m_codec.Poll();
        }
    }

    void report() {
        std::cout << m_count << " template objects loaded."
                  << endl << flush;
    }

    Atlas::Message::Object::MapType & db() {
        return m_db;
    }

    const Atlas::Message::Object & get(const string & graphic) {
        return m_db[graphic];
    }
};

static void usage(char * progname)
{
    cout << "usage: " << progname << " filename" << endl << flush;
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

    TemplatesLoader f("templates.xml");
    f.read();
    f.report();

    int id_no = 0;

    CoalDatabase map_database;
    CoalIsoLoader loader (map_database);
    if (!loader.LoadMap(argv[1])) {
        cout << "Map load failed." << endl << flush;
    } else {
        int count = map_database.GetObjectCount();
        for(int i = 0; i < count; i++) {
            CoalObject * object = (CoalObject*)map_database.GetObject (i);
            if (object != NULL) {
                const string & graphic = object->graphic->filename;
                size_t b = graphic.rfind('/') + 1;
                size_t e = graphic.rfind('.');
                string key(graphic, b, e - b);
                Atlas::Message::Object::MapType & tdb = f.db();
                Atlas::Message::Object::MapType::iterator t = tdb.find(key);
                if (t != tdb.end()) {
                    Atlas::Message::Object o = t->second;
                    Atlas::Message::Object::MapType & omap = o.AsMap();
                    omap.erase("graphic");
                    stringstream id;
                    id << omap["name"].AsString() << "_" << ++id_no << "_m";
                    omap["id"] = id.str();
                    omap["loc"] = "world_0";
                    Atlas::Message::Object::ListType c(3);
                    c[0] = object->anchor.GetX();
                    c[1] = object->anchor.GetY();
                    c[2] = object->anchor.GetZ();
                    omap["pos"] = c;
                    db->storeInWorld(o, id.str().c_str());
                }
                // Get basename, lookup custumise and load into database
                cout << graphic << " " << key << endl;
            }
        }
    }
    
    db->shutdownWorld();
    delete db;
}

#else // defined(CYPHESIS_USE_DB3) && defined(HAVE_LIBCOALISO)

int main(int argc, char ** argv)
{
#ifndef CYPHESIS_USE_DB3
    std::cerr << "This version of cyphesis was built without persistant world support" << endl << flush;
#endif
#ifndef HAVE_LIBCOALISO
    std::cerr << "This version of cyphesis was built without map loader support" << endl << flush;
#endif
    exit(0);
}

#endif // defined(CYPHESIS_USE_DB3) && defined(HAVE_LIBCOALISO)
