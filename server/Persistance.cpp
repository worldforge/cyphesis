// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <fstream.h>

#include <server/Admin.h>
#include <server/Player.h>
#include <rulesets/Entity.h>

#include <common/const.h>

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
    adm_file << "<atlas>" << std::endl << "<map>" << std::endl;
    adm_file << "    <string name=\"password\">" << adm.password << "</string>" << std::endl;
    adm_file << "    <string name=\"id\">" << adm.getId() << "</string>" << std::endl;
    adm_file << "    <list name=\"parents\">" << std::endl;
    adm_file << "    <string>admin</string>" << std::endl;
    adm_file << "    </list>" << std::endl;
    adm_file << "</map>" << std::endl << "</atlas>" << std::endl << std::flush;
    adm_file.close();
}

// This is the version of the persistance code which is enabled if 
// there is db support.

// DB_CXX_NO_EXCEPTIONS is set for now to enable easier debugging. Later
// once the code is padded out, this should be removed to allow exceptions.

bool Persistance::init()
{
    Persistance * p = instance();
    if (!p->initConnection(false)) {
        return false;
    }
    bool i = p->initAccount(true);
    bool j = p->initWorld(true);
    bool k = p->initMind(true);
    bool l = p->initServer(true);
    bool m = p->initRule(true);
    return (i && j && k && l && m);
}

void Persistance::shutdown()
{
    Persistance * p = (Persistance *)m_instance;
    if (p == NULL) { return; }
    p->shutdownConnection();
    delete p;
    m_instance = NULL;
}

Account * Persistance::loadAdminAccount()
{
    Persistance * p = instance();
    Account * adm = p->getAccount("admin");
    if (adm == NULL) {
        adm = new Admin(NULL, "admin", consts::defaultAdminPassword);
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
        std::cerr << "WARNING: Database account entry " << name
             << " is missing essential fields." << std::endl << std::flush;
        return NULL;
    }
    const Object & acn = I->second, & acp = J->second;
    if (!acn.IsString() || !acp.IsString()) {
        std::cerr << "WARNING: Database account entry " << name << " is corrupt."
             << std::endl << std::flush;
        return NULL;
    }
    if (acn.AsString() == "admin") {
        return new Admin(NULL, acn.AsString(), acp.AsString());
    }
    return new Player(NULL, acn.AsString(), acp.AsString());
}

void Persistance::putAccount(const Account & ac)
{
    putObject(account_db, ac.getId().c_str(), ac.asObject().AsMap());
}

bool Persistance::getEntity(const std::string & id, Object::MapType & entity)
{
    return getObject(world_db, id.c_str(), entity);
}

void Persistance::putEntity(const Entity & be)
{
    putObject(world_db, be.getId().c_str(), be.asObject().AsMap());
}

bool Persistance::getMind(const std::string & id, Object::MapType & entity)
{
    return getObject(mind_db, id.c_str(), entity);
}

void Persistance::putMind(const std::string & id, const Object::MapType & be)
{
    putObject(mind_db, id.c_str(), be);
}
