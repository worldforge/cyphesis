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
    bool i = p->m_connection.initRule(true);

    Fragment::MapType tableDesc;
    tableDesc["username"] = "                                                                                ";
    tableDesc["password"] = "                                                                                ";
    tableDesc["type"] = "          ";
    bool j = p->m_connection.registerSimpleTable("accounts", tableDesc);
    bool k = p->m_connection.registerRelation("character");

    return (i && j && k);
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
    DatabaseResult dr = m_connection.selectSimpleRowBy("accounts", "username", name);
    if (dr.error()) {
        log(ERROR, "Failure while find account.");
        return false;
    }
    if (dr.empty()) {
        return false;
    }
    if (dr.size() > 1) {
        log(ERROR, "Duplicate username in accounts database.");
    }
    return true;
}

Account * Persistance::getAccount(const std::string & name)
{
    std::string namestr = "'" + name + "'";
    DatabaseResult dr = m_connection.selectSimpleRowBy("accounts", "username", namestr);
    if (dr.error()) {
        log(ERROR, "Failure while find account.");
        return 0;
    }
    if (dr.empty()) {
        return 0;
    }
    if (dr.size() > 1) {
        log(ERROR, "Duplicate username in accounts database.");
    }
    const char * c = dr.field("id");
    if (c == 0) {
        log(ERROR, "Unable to find id field in accounts database.");
        return 0;
    }
    std::string id = c;
    c = dr.field("password");
    if (c == 0) {
        log(ERROR, "Unable to find password field in accounts database.");
        return 0;
    }
    std::string passwd = c;
    c = dr.field("type");
    if (c == 0) {
        log(ERROR, "Unable to find type field in accounts database.");
        return 0;
    }
    std::string type = c;
    if (type == "admin") {
        return new Admin(0, name, passwd, id);
    } else {
        return new Player(0, name, passwd, id);
    }
}

void Persistance::putAccount(const Account & ac)
{
    std::string columns = "username, type, password";
    std::string values = "'";
    values += ac.username;
    values += "', '";
    values += ac.getType();
    values += "', '";
    values += ac.password;
    values += "'";
    m_connection.createSimpleRow("accounts", ac.getId(), columns, values);
}

bool Persistance::getRules(Fragment::MapType & m)
{
    return m_connection.getTable(m_connection.rule(), m);
}

bool Persistance::clearRules()
{
    return m_connection.clearTable(m_connection.rule());
}

