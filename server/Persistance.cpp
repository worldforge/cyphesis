// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <fstream.h>
// #include <strstream>

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Codecs/XML.h>

#include <server/Admin.h>
#include <server/Player.h>
#include <rulesets/Entity.h>

#include <common/config.h>
#include <common/stringstream.h>

#include "Persistance.h"

using Atlas::Message::Object;

bool Persistance::restricted = false;

Persistance * Persistance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Persistance();
    }
    return (Persistance *)m_instance;
}

void Persistance::saveAdminAccount(Account & adm)
{
    std::ofstream adm_file("/tmp/admin.xml", std::ios::out);
    adm_file << "<atlas>" << endl << "<map>" << endl;
    adm_file << "    <string name=\"password\">" << adm.password << "</string>" << endl;
    adm_file << "    <string name=\"id\">" << adm.fullid << "</string>" << endl;
    adm_file << "    <list name=\"parents\">" << endl;
    adm_file << "    <string>admin</string>" << endl;
    adm_file << "    </list>" << endl;
    adm_file << "</map>" << endl << "</atlas>" << endl << flush;
    adm_file.close();
}

#ifdef CYPHESIS_USE_DB3

// This is the version of the persistance code which is enabled if 
// there is db support.

// DB_CXX_NO_EXCEPTIONS is set for now to enable easier debugging. Later
// once the code is padded out, this should be removed to allow exceptions.

bool Persistance::init()
{
    Persistance * p = instance();
    bool i = p->initAccount(true);
    bool j = p->initWorld(true);
    bool k = p->initMind(true);
    bool l = p->initServer(true);
    return (i && j && k && l);
}

void Persistance::shutdown()
{
    Persistance * p = instance();
    p->shutdownAccount();
    p->shutdownWorld();
    p->shutdownMind();
    p->shutdownServer();
}

Account * Persistance::loadAdminAccount()
{
    Persistance * p = instance();
    Account * adm = p->getAccount("admin");
    if (adm == NULL) {
        adm = new Admin(NULL, "admin", "zjvspoehrgopes");
        p->putAccount(*adm);
    }
    saveAdminAccount(*adm);
    return adm;
}

bool Persistance::findAccount(const std::string & name)
{
    Object::MapType account;
    return getObject(account_db, name.c_str(), account);
}

Account * Persistance::getAccount(const std::string & name)
{
    Object::MapType account;
    if (!getObject(account_db, name.c_str(), account)) {
        return NULL;
    }
    Object::MapType::const_iterator I = account.find("id"),
                                    J = account.find("password");
    if ((I == account.end()) || (J == account.end())){
        cerr << "WARNING: Database account entry " << name
             << " is missing essential fields." << endl << flush;
        return NULL;
    }
    const Object & acn = I->second, & acp = J->second;
    if (!acn.IsString() || !acp.IsString()) {
        cerr << "WARNING: Database account entry " << name << " is corrupt."
             << endl << flush;
        return NULL;
    }
    if (acn.AsString() == "admin") {
        return new Admin(NULL, acn.AsString(), acp.AsString());
    }
    return new Player(NULL, acn.AsString(), acp.AsString());
}

void Persistance::putAccount(const Account & ac)
{
    putObject(account_db, ac.asObject().AsMap(), ac.fullid.c_str());
}

bool Persistance::getEntity(const std::string & id, Object::MapType & entity)
{
    return getObject(world_db, id.c_str(), entity);
}

void Persistance::putEntity(const Entity & be)
{
    putObject(world_db, be.asObject().AsMap(), be.fullid.c_str());
}

bool Persistance::getMind(const std::string & id, Object::MapType & entity)
{
    return getObject(mind_db, id.c_str(), entity);
}

void Persistance::putMind(const std::string & id, const Object::MapType & be)
{
    putObject(mind_db, be, id.c_str());
}

#else // HAVE_LIBDB_CXX

Persistance::Persistance() { }

Account * Persistance::loadAdminAccount()
{
    // Eventually this should actually load the account. For now it just
    // creates it.
    Account * adm = new Admin(NULL, "admin", "test");
    saveAdminAccount(adm);
    return adm;
}

bool Persistance::findAccount(const std::string &) { return false; }

Account * Persistance::getAccount(const std::string & name) { return NULL; }

void Persistance::putAccount(const Account *) { }

void Persistance::putEntity(const BaseEntity *) { }

bool Persistance::init() { return true; }

void Persistance::shutdown() { }

#endif // HAVE_LIBDB_CXX
