// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include <fstream.h>
// #include <strstream>

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Codecs/XML.h>

#include <common/config.h>
#include <common/debug.h>
#include <common/globals.h>

#include "database.h"

// It seems that GNU libstdc++ does not yet include sstream.h
// Included is a version written explicitly for gcc, which will be used
// if the header is not present.
#include <common/stringstream.h>

static const bool debug_flag = false;

Database * Database::m_instance = NULL;

Database::Database() : account_db(NULL, DB_CXX_NO_EXCEPTIONS),
                         world_db(NULL, DB_CXX_NO_EXCEPTIONS),
                          mind_db(NULL, DB_CXX_NO_EXCEPTIONS),
                        server_db(NULL, DB_CXX_NO_EXCEPTIONS)
{
    db_file = std::string(VARDIR) + "/cyphesis/db";
}


bool Database::initAccount(bool create)
{
    int i = account_db.open(db_file.c_str(), "account",
                               DB_BTREE, create ? DB_CREATE : DB_RDONLY, 0600);
    return (i == 0);
}

bool Database::initWorld(bool create)
{
    int j = world_db.open(db_file.c_str(), "world",
                             DB_BTREE, create ? DB_CREATE : DB_RDONLY, 0600);
    return (j == 0);
}

bool Database::initMind(bool create)
{
    int k = mind_db.open(db_file.c_str(), "mind",
                             DB_BTREE, create ? DB_CREATE : DB_RDONLY, 0600);
    return (k == 0);
}

bool Database::initServer(bool create)
{
    int l = server_db.open(db_file.c_str(), "server",
                             DB_BTREE, create ? DB_CREATE : DB_RDONLY, 0600);
    return (l == 0);
}

void Database::shutdownAccount()
{
    account_db.close(0);
}

void Database::shutdownWorld()
{
    world_db.close(0);
}

void Database::shutdownMind()
{
    mind_db.close(0);
}

void Database::shutdownServer()
{
    server_db.close(0);
}

Database * Database::instance()
{
    if (m_instance == NULL) {
        m_instance = new Database();
    }
    return m_instance;
}

bool Database::decodeObject(Dbt & data, Atlas::Message::Object::MapType &o)
{
    char * entry = (char *)data.get_data();

    debug(cout << " length " << strlen(entry) << "," << data.get_size() << "\"" 
               << entry << "\"" << endl << flush;);

    if (strlen(entry) != (data.get_size() - 1)) {
        cerr << "WARNING: Database may be corrupt, aborting fetch" << endl << flush;
        return false;
    }
    
    std::stringstream str(std::string () + std::string(entry), std::ios::in);

    Atlas::Codecs::XML codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

    // Clear the decoder
    m_d.get();

    codec.Poll();

    if (!m_d.check()) {
        cerr << "WARNING: Database entry does not appear to be decodable"
             << endl << flush;
        return false;
    }
    
    o = m_d.get();
    return true;
}

bool Database::getObject(Db & db, const char * keystr,
                         Atlas::Message::Object::MapType & o)
{
    Dbt key, data;

    key.set_data((void*)keystr);
    key.set_size(strlen(keystr) + 1);

    int res = db.get(NULL, &key, &data, 0);

    if (res == DB_NOTFOUND) {
        debug(cout << "No entry for " << keystr << " in database"
                   << endl << flush;);
        return false;
    } else if (res != 0) {
        cerr << "WARNING: Error accessing database" << endl << flush;
        return false;
    }
    debug(cout << "Got record " << keystr << " from database,";);
    return decodeObject(data, o);
}

bool Database::putObject(Db & db, const Atlas::Message::Object::MapType & o,
                            const char * keystr)
{
    std::stringstream str;

    Atlas::Codecs::XML codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

    codec.StreamBegin();
    enc.StreamMessage(o);
    codec.StreamEnd();

    debug(cout << "Encoded to: " << str.str().c_str() << " "
               << str.str().size() << endl << flush;);
    const std::string & s = str.str();

    Dbt key, data;

    key.set_data((void*)keystr);
    key.set_size(strlen(keystr) + 1);

    data.set_data((void*)s.c_str());
    data.set_size(s.size() + 1);

    int err;
    if ((err = db.put(NULL, &key, &data, 0)) != 0) {
        debug(cout << "db.put.ERROR! " << err << endl << flush;);
        return false;
    }
    return true;
}

bool Database::delObject(Db & db, const char * keystr)
{
    Dbt key, data;

    key.set_data((void*)keystr);
    key.set_size(strlen(keystr) + 1);

    int err;
    if ((err = db.del(NULL, &key, 0)) != 0) {
        debug(cout << "db.del.ERROR! " << err << endl << flush;);
        return false;
    }
    return true;
}

bool DatabaseIterator::get(Atlas::Message::Object::MapType & o)
{
    Dbt key, data;

    int res = m_cursor->get(&key, &data, DB_NEXT);
    if (res == DB_NOTFOUND) {
        debug(cout << "No entries remain in database" << endl << flush;);
        return false;
    } else if (res != 0) {
        cerr << "WARNING: Error accessing database" << endl << flush;
        return false;
    }
    return Database::instance()->decodeObject(data, o);
}

bool DatabaseIterator::del()
{
    int res = m_cursor->del(0);
    if (res == 0) {
        return true;
    }
    cerr << "WARNING: Error deleting from database" << endl << flush;
    return false;
}
