// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Persistance.h"

#include "Admin.h"
#include "Player.h"

#include <rulesets/Entity.h>

#include <common/log.h>
#include <common/const.h>
#include <common/Database.h>

#include <fstream>

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
    return adm;
}

bool Persistance::findAccount(const std::string & name)
{
    Fragment::MapType account;
    return m_connection.getObject(m_connection.account(), name, account);
}

Account * Persistance::getAccount(const std::string & name)
{
    Fragment::MapType account;
    if (!m_connection.getObject(m_connection.account(), name, account)) {
        return NULL;
    }
    Fragment::MapType::const_iterator I = account.find("id"),
                                    J = account.find("password");
    if ((I == account.end()) || (J == account.end())){
        std::string msg  = std::string("Database account entry ") + name
                         + " is missing fields.";
        log(ERROR, msg.c_str());
        return NULL;
    }
    const Fragment & acn = I->second, & acp = J->second;
    if (!acn.IsString() || !acp.IsString()) {
        std::string msg = std::string("Database account entry ") + name
                        + " is corrupt.";
        log(ERROR, msg.c_str());
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

bool Persistance::getEntity(const std::string & id, Fragment::MapType & entity)
{
    return m_connection.getObject(m_connection.world(), id, entity);
}

void Persistance::putEntity(const Entity & be)
{
    m_connection.putObject(m_connection.world(), be.getId(), be.asObject().AsMap());
}

bool Persistance::getMind(const std::string & id, Fragment::MapType & entity)
{
    return m_connection.getObject(m_connection.mind(), id, entity);
}

void Persistance::putMind(const std::string & id, const Fragment::MapType & be)
{
    m_connection.putObject(m_connection.mind(), id, be);
}

bool Persistance::getRules(Fragment::MapType & m)
{
    return m_connection.getTable(m_connection.rule(), m);
}

bool Persistance::clearRules()
{
    return m_connection.clearTable(m_connection.rule());
}

