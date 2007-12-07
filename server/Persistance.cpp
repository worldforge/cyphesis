// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: Persistance.cpp,v 1.52 2007-12-07 00:44:08 alriddoch Exp $

#include "Persistance.h"

#include "Admin.h"
#include "Player.h"

#include "rulesets/Entity.h"

#include "common/id.h"
#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/Database.h"
#include "common/compose.hpp"

using Atlas::Message::MapType;

static const bool debug_flag = false;

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

int Persistance::init()
{
    assert(this != 0);

    if (m_connection.initConnection() != 0) {
        if (::instance == CYPHESIS) {
            return DATABASE_CONERR;
        }
        if (m_connection.createInstanceDatabase() != 0) {
            log(ERROR, "Database creation failed.");
            return DATABASE_CONERR;
        }
        if (m_connection.initConnection() != 0) {
            log(ERROR, "Still couldn't connect.");
            return DATABASE_CONERR;
        }
        log(INFO, String::compose("Auto created database for new instance "
                                  "\"%1\".", ::instance));
    }

    if (!m_connection.registerEntityIdGenerator()) {
        log(ERROR, "Faled to register Id generator in database.");
        return DATABASE_TABERR;
    }

    bool i = m_connection.initRule(true);

    MapType tableDesc;
    tableDesc["username"] = "                                                                                ";
    tableDesc["password"] = "                                                                                ";
    tableDesc["type"] = "          ";
    bool j = m_connection.registerSimpleTable("accounts", tableDesc);
    bool k = m_connection.registerRelation(m_characterRelation,
                                              "accounts",
                                              "entity_ent");

    if (!findAccount("admin")) {
        debug(std::cout << "Bootstraping admin account."
                        << std::endl << std::flush;);
        std::string adminAccountId;
        long adminAccountIntId = m_connection.newId(adminAccountId);
        if (adminAccountIntId < 0) {
            log(CRITICAL, "Unable to get admin account ID from Database");
            return -2;
        }

        Admin dummyAdminAccount(0, "admin", consts::defaultAdminPasswordHash,
                                adminAccountId, adminAccountIntId);
        
        putAccount(dummyAdminAccount);
    }

    return (i && j && k) ? 0 : -2;
}

void Persistance::shutdown()
{
    m_connection.shutdownConnection();
    delete &m_connection;
    assert(this == m_instance);
    delete m_instance;
    m_instance = NULL;
}

bool Persistance::findAccount(const std::string & name)
{
    std::string namestr = "'" + name + "'";
    DatabaseResult dr = m_connection.selectSimpleRowBy("accounts", "username", namestr);
    if (dr.error()) {
        log(ERROR, "Failure while find account.");
        return false;
    }
    if (dr.empty()) {
        dr.clear();
        return false;
    }
    if (dr.size() > 1) {
        log(ERROR, "Duplicate username in accounts database.");
    }
    dr.clear();
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
        dr.clear();
        return 0;
    }
    if (dr.size() > 1) {
        log(ERROR, "Duplicate username in accounts database.");
    }
    const char * c = dr.field("id");
    if (c == 0) {
        dr.clear();
        log(ERROR, "Unable to find id field in accounts database.");
        return 0;
    }
    std::string id = c;
    long intId = integerId(id);
    if (intId == -1) {
        dr.clear();
        log(ERROR, String::compose("Invalid ID \"%1\" for account from database.", id));
        return 0;
    }
    c = dr.field("password");
    if (c == 0) {
        dr.clear();
        log(ERROR, "Unable to find password field in accounts database.");
        return 0;
    }
    std::string passwd = c;
    c = dr.field("type");
    if (c == 0) {
        dr.clear();
        log(ERROR, "Unable to find type field in accounts database.");
        return 0;
    }
    std::string type = c;
    dr.clear();
    if (type == "admin") {
        return new Admin(0, name, passwd, id, intId);
    } else {
        return new Player(0, name, passwd, id, intId);
    }
}

void Persistance::putAccount(const Account & ac)
{
    std::string columns = "username, type, password";
    std::string values = "'";
    values += ac.m_username;
    values += "', '";
    values += ac.getType();
    values += "', '";
    values += ac.m_password;
    values += "'";
    m_connection.createSimpleRow("accounts", ac.getId(), columns, values);
}

void Persistance::registerCharacters(Account & ac,
                                     const EntityDict & worldObjects)
{
    DatabaseResult dr = m_connection.selectRelation(m_characterRelation,
                                                    ac.getId());
    if (dr.error()) {
        log(ERROR, "Database query failed while looking for characters for account.");
    }
    DatabaseResult::const_iterator Iend = dr.end();
    for (DatabaseResult::const_iterator I = dr.begin(); I != Iend; ++I) {
        const char * id = I.column(0);
        if (id == 0) {
            log(ERROR, "No ID data in relation when examing characters");
            continue;
        }

        long intId = integerId(id);

        EntityDict::const_iterator J = worldObjects.find(intId);
        if (J == worldObjects.end()) {
            log(WARNING, String::compose("Persistance: Got character id \"%1\" "
                                         "from database which does not exist "
                                         "in world.", id));
            continue;
        }
        ac.addCharacter(J->second);
    }
    dr.clear();
}

void Persistance::addCharacter(const Account & ac, const Entity & e)
{
    m_connection.createRelationRow(m_characterRelation, ac.getId(), e.getId());
}

void Persistance::delCharacter(const std::string & id)
{
    m_connection.removeRelationRowByOther(m_characterRelation, id);
}

bool Persistance::getRules(MapType & m)
{
    return m_connection.getTable(m_connection.rule(), m);
}

bool Persistance::clearRules()
{
    return m_connection.clearTable(m_connection.rule());
}
