// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <fstream.h>
// #include <strstream>

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Codecs/XML.h>

#include <server/Admin.h>
#include <server/Player.h>

#include <config.h>

#include "persistance.h"

// It seems that gcc does not yet include sstream.h
// Included is a version written explicitly for gcc, which will be used
// if the header is not present.
#ifdef HAVE_SSTREAM_H
#include <sstream.h>
#else
#include "sstream.h"
#endif

Persistance * Persistance::m_instance = NULL;
bool Persistance::restricted = false;

Persistance * Persistance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Persistance();
    }
    return m_instance;
}

void Persistance::save_admin_account(Account * adm)
{
    std::ofstream adm_file("/tmp/admin.xml", ios::out, 0600);
    adm_file << "<atlas>" << endl << "<map>" << endl;
    adm_file << "    <string name=\"password\">" << adm->password << "</string>" << endl;
    adm_file << "    <string name=\"id\">" << adm->fullid << "</string>" << endl;
    adm_file << "    <list name=\"parents\">" << endl;
    adm_file << "    <string>admin</string>" << endl;
    adm_file << "    </list>" << endl;
    adm_file << "</map>" << endl << "</atlas>" << endl << flush;
    adm_file.close();
}

#ifdef HAVE_LIBDB_CXX

// This is the version of the persistance code which is enabled if 
// there is db support.

// DB_CXX_NO_EXCEPTIONS is set for now to enable easier debugging. Later
// once the code is padded out, this should be removed to allow exceptions.

Persistance::Persistance() : account_db(NULL, DB_CXX_NO_EXCEPTIONS),
                             world_db(NULL, DB_CXX_NO_EXCEPTIONS) { }

bool Persistance::init()
{
    Persistance * p = instance();
    int i = p->account_db.open("/var/forge/cyphesis/db", "account",
                               DB_BTREE, DB_CREATE, 0600);
    int j = p->world_db.open("/var/forge/cyphesis/db", "world",
                             DB_BTREE, 0, 0600);
    return ((i == 0) && (j == 0));
}

void Persistance::shutdown()
{
    Persistance * p = instance();
    p->account_db.close(0);
    p->world_db.close(0);
}

Account * Persistance::load_admin_account()
{
    Persistance * p = instance();
    Account * adm;
    if ((adm = p->getAccount("admin")) == NULL) {
        adm = new Admin(NULL, "admin", "test");
        p->putAccount(adm);
    }
    save_admin_account(adm);
    return adm;
}

bool Persistance::findAccount(const std::string & name)
{
    Atlas::Message::Object account;
    return getObject(account_db, name.c_str(), account);
}

Account * Persistance::getAccount(const std::string & name)
{
    Atlas::Message::Object account;
    if (!getObject(account_db, name.c_str(), account)) {
        return NULL;
    }
    Atlas::Message::Object::MapType acmap = account.AsMap();
    if ((acmap.find("id")==acmap.end())||(acmap.find("password")==acmap.end())){
        cerr << "Database account entry " << name
             << " is missing essential fields." << endl << flush;
        return NULL;
    }
    Atlas::Message::Object & acn = acmap["id"], acp = acmap["password"];
    if (!acn.IsString() || !acp.IsString()) {
        cerr << "Database account entry " << name << " is corrupt."
             << endl << flush;
        return NULL;
    }
    if (acn.AsString() == "admin") {
        return new Admin(NULL, acn.AsString(), acp.AsString());
    }
    return new Player(NULL, acn.AsString(), acp.AsString());
}

void Persistance::putAccount(const Account * ac)
{
    putObject(account_db, ac->asObject(), ac->fullid.c_str());
}

bool Persistance::getObject(Db & db, const char * keystr,
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

bool Persistance::putObject(Db & db, const Atlas::Message::Object & o,
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

#else // HAVE_LIBDB_CXX

Persistance::Persistance() { }

Account * Persistance::load_admin_account()
{
    // Eventually this should actually load the account. For now it just
    // creates it.
    Account * adm = new Admin(NULL, "admin", "test");
    save_admin_account(adm);
    return(adm);
}

Account * Persistance::getAccount(const std::string & name) { return NULL; }

void Persistance::putAccount(const Account * ac) { }

bool Persistance::init() { return true; }

void Persistance::shutdown() { }

#endif // HAVE_LIBDB_CXX
