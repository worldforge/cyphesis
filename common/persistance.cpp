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

#include <common/config.h>

#include "persistance.h"

// It seems that gcc does not yet include sstream.h
// Included is a version written explicitly for gcc, which will be used
// if the header is not present.
#ifdef HAVE_SSTREAM_H
#include <sstream.h>
#else
#include "sstream.h"
#endif

bool Persistance::restricted = false;

Persistance * Persistance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Persistance();
    }
    return (Persistance *)m_instance;
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

bool Persistance::init()
{
    Persistance * p = instance();
    bool i = p->initAccount(true);
    bool j = p->initWorld();
    return (i && j);
}

void Persistance::shutdown()
{
    Persistance * p = instance();
    p->shutdownAccount();
    p->shutdownWorld();
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

bool Persistance::findAccount(const std::string &) { return false; }

Account * Persistance::getAccount(const std::string & name) { return NULL; }

void Persistance::putAccount(const Account * ac) { }

bool Persistance::init() { return true; }

void Persistance::shutdown() { }

#endif // HAVE_LIBDB_CXX
