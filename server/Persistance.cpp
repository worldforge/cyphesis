// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Persistance.h"

#include "Admin.h"
#include "Player.h"

#include <rulesets/Entity.h>

#include <common/const.h>
#include <common/Database.h>

#include <fstream>

using Atlas::Message::Object;

bool Persistance::restricted = false;
Persistance * Persistance::m_instance = NULL;

Persistance::Persistance() : m_connection(*Database::instance())
{
}

Persistance * Persistance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Persistance();
    }
    return m_instance;
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

bool Persistance::init()
{
    Persistance * p = instance();
    if (!p->m_connection.initConnection(false)) {
        return false;
    }
    bool i = p->m_connection.initAccount(true);
    bool j = p->m_connection.initWorld(true);
    bool k = p->m_connection.initMind(true);
    bool l = p->m_connection.initServer(true);
    bool m = p->m_connection.initRule(true);
    return (i && j && k && l && m);
}

void Persistance::shutdown()
{
    Persistance * p = m_instance;
    if (p == NULL) { return; }
    p->m_connection.shutdownConnection();
    delete &p->m_connection;
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
    return m_connection.getObject(m_connection.account(), name, account);
}

Account * Persistance::getAccount(const std::string & name)
{
    Object::MapType account;
    if (!m_connection.getObject(m_connection.account(), name, account)) {
        return NULL;
    }
    Object::MapType::const_iterator I = account.find("id"),
                                    J = account.find("password");
    if ((I == account.end()) || (J == account.end())){
        std::cerr << "WARNING: Database account entry " << name
                  << " is missing fields." << std::endl << std::flush;
        return NULL;
    }
    const Object & acn = I->second, & acp = J->second;
    if (!acn.IsString() || !acp.IsString()) {
        std::cerr << "WARNING: Database account entry " << name
                  << " is corrupt." << std::endl << std::flush;
        return NULL;
    }
    if (acn.AsString() == "admin") {
        return new Admin(NULL, acn.AsString(), acp.AsString());
    }
    return new Player(NULL, acn.AsString(), acp.AsString());
}

void Persistance::putAccount(const Account & ac)
{
    m_connection.putObject(m_connection.account(), ac.getId(), ac.asObject().AsMap());
}

bool Persistance::getEntity(const std::string & id, Object::MapType & entity)
{
    return m_connection.getObject(m_connection.world(), id, entity);
}

void Persistance::putEntity(const Entity & be)
{
    m_connection.putObject(m_connection.world(), be.getId(), be.asObject().AsMap());
}

bool Persistance::getMind(const std::string & id, Object::MapType & entity)
{
    return m_connection.getObject(m_connection.mind(), id, entity);
}

void Persistance::putMind(const std::string & id, const Object::MapType & be)
{
    m_connection.putObject(m_connection.mind(), id, be);
}

bool Persistance::getRules(Atlas::Message::Object::MapType & m)
{
    return m_connection.getTable(m_connection.rule(), m);
}

bool Persistance::clearRules()
{
    return m_connection.clearTable(m_connection.rule());
}

