// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <fstream.h>
// #include <strstream>

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Codecs/XML.h>

#include <common/config.h>

#include "database.h"

// It seems that gcc does not yet include sstream.h
// Included is a version written explicitly for gcc, which will be used
// if the header is not present.
#ifdef HAVE_SSTREAM_H
#include <sstream.h>
#else
#include "sstream.h"
#endif

Database * Database::m_instance = NULL;

bool Database::initAccount()
{
    int i = account_db.open("/var/forge/cyphesis/db", "account",
                               DB_BTREE, DB_CREATE, 0600);
    return (i == 0);
}

bool Database::initWorld(bool create)
{
    int j = world_db.open("/var/forge/cyphesis/db", "world",
                             DB_BTREE, create ? DB_CREATE : 0, 0600);
    return (j == 0);
}

void Database::shutdownAccount()
{
    account_db.close(0);
}

void Database::shutdownWorld()
{
    world_db.close(0);
}

Database * Database::instance()
{
    if (m_instance == NULL) {
        m_instance = new Database();
    }
    return m_instance;
}

bool Database::getObject(Db & db, const char * keystr,
                         Atlas::Message::Object & o)
{
    Dbt key, data;

    key.set_data((void*)keystr);
    key.set_size(strlen(keystr) + 1);

    int res = db.get(NULL, &key, &data, 0);

    if (res == DB_NOTFOUND) {
        cout << "No entry for " << keystr << " in database" << endl << flush;
        return false;
    } else if (res != 0) {
        cerr << "Error accessing database" << endl << flush;
        return false;
    }


    char * entry = (char *)data.get_data();

    cout << "Got record " << keystr << " from database, length "
         << strlen(entry) << "," << data.get_size() << "\"" 
         << entry << "\"" << endl << flush;

    if (strlen(entry) != (data.get_size() - 1)) {
        cerr << "Database may be corrupt, aborting fetch" << endl << flush;
        return false;
    }
    
    std::stringstream str(std::string () + std::string(entry));

    Atlas::Codecs::XML codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

    m_d.get();

    codec.Poll();

    if (!m_d.check()) {
        cerr << "Database entry for " << keystr
             << " does not appear to be decodable" << endl << flush;
        return false;
    }
    
    o = m_d.get();
    return true;
}

bool Database::putObject(Db & db, const Atlas::Message::Object & o,
                            const char * keystr)
{
    std::stringstream str;

    Atlas::Codecs::XML codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

    codec.StreamBegin();
    enc.StreamMessage(o);
    codec.StreamEnd();

    Dbt key, data;

    key.set_data((void*)keystr);
    key.set_size(strlen(keystr) + 1);

    data.set_data((void*)str.str().c_str());
    data.set_size(str.str().size() + 1);

    int err;
    if ((err = db.put(NULL, &key, &data, 0)) != 0) {
        cout << "db.put.ERROR! " << err << endl << flush;
        return false;
    }
    return true;
}
