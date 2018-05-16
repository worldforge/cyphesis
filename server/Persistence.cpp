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


#include "Persistence.h"

#include "Admin.h"
#include "Player.h"
#include "ServerAccount.h"

#include "rulesets/LocatedEntity.h"

#include "common/id.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/Database.h"
#include "common/Shaker.h"

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Objects::Root;

static const bool debug_flag = false;

template<> Persistence* Singleton<Persistence>::ms_Singleton = nullptr;

Persistence::Persistence(Database& database) : m_db(database)
{
}
int Persistence::init()
{
    if (m_db.initConnection() != 0) {
        if (::instance == CYPHESIS) {
            return DATABASE_CONERR;
        }
        if (m_db.createInstanceDatabase() != 0) {
            log(ERROR, "Database creation failed.");
            return DATABASE_CONERR;
        }
        if (m_db.initConnection() != 0) {
            log(ERROR, "Still couldn't connect.");
            return DATABASE_CONERR;
        }
        log(INFO, String::compose("Auto created database for new instance "
                                  "\"%1\".", ::instance));
    }

    if (m_db.registerEntityIdGenerator() != 0) {
        log(ERROR, "Failed to register Id generator in database.");
        return DATABASE_TABERR;
    }

    std::map<std::string, int> chunks;
    chunks["location"] = 0;

    if (m_db.registerEntityTable(chunks) != 0) {
        log(ERROR, "Failed to create Entity in database.");
        return DATABASE_TABERR;
    }

    if (m_db.registerPropertyTable() != 0) {
        log(ERROR, "Failed to create Property in database.");
        return DATABASE_TABERR;
    }

    if (m_db.registerThoughtsTable() != 0) {
        log(ERROR, "Failed to create Thought in database.");
        return DATABASE_TABERR;
    }

    bool i = (m_db.initRule(true) == 0);

    MapType tableDesc;
    tableDesc["username"] = "                                                                                ";
    tableDesc["password"] = "                                                                                ";
    tableDesc["type"] = "          ";
    bool j = m_db.registerSimpleTable("accounts", tableDesc) == 0;
    bool k = m_db.registerRelation(m_characterRelation,
                                           "accounts",
                                           "entities") == 0;

    if (!findAccount("admin")) {
        debug(std::cout << "Bootstraping admin account."
                        << std::endl << std::flush;);
        std::string adminAccountId;
        long adminAccountIntId = m_db.newId(adminAccountId);
        if (adminAccountIntId < 0) {
            log(CRITICAL, "Unable to create admin account ID from Database");
            return -2;
        }

        //The shaker isn't meant for this, but it will suffice. A password of 32
        //characters length should be safe enough.
        Shaker shaker;
        std::string password = shaker.generateSalt(32);

        Admin dummyAdminAccount(0, "admin", password,
                                adminAccountId, adminAccountIntId);

        log(INFO, "Created 'admin' account with randomized password.\n"
                "In order to use it, use the 'cypasswd' tool from the "
                "command line to alter the password.");

        putAccount(dummyAdminAccount);
    }

    return (i && j && k) ? 0 : DATABASE_TABERR;
}

void Persistence::shutdown()
{
    m_db.shutdownConnection();
}

bool Persistence::findAccount(const std::string & name)
{
    std::string namestr = "'" + name + "'";
    DatabaseResult dr = m_db.selectSimpleRowBy("accounts", "username", namestr);
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

Account * Persistence::getAccount(const std::string & name)
{
    std::string namestr = "'" + name + "'";
    DatabaseResult dr = m_db.selectSimpleRowBy("accounts", "username", namestr);
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
    long intId = integerId(id);
    if (intId == -1) {
        log(ERROR, String::compose("Invalid ID \"%1\" for account from database.", id));
        return 0;
    }
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
        return new Admin(0, name, passwd, id, intId);
    } else if (type == "server") {
        return new ServerAccount(0, name, passwd, id, intId);
    } else {
        return new Player(0, name, passwd, id, intId);
    }
}

void Persistence::putAccount(const Account & ac)
{
    std::string columns = "username, type, password";
    std::string values = "'";
    values += ac.username();
    values += "', '";
    values += ac.getType();
    values += "', '";
    values += ac.password();
    values += "'";
    m_db.createSimpleRow("accounts", ac.getId(), columns, values);
}

void Persistence::registerCharacters(Account & ac,
                                     const EntityDict & worldObjects)
{
    DatabaseResult dr = m_db.selectRelation(m_characterRelation,
                                                    ac.getId());
    if (dr.error()) {
        log(ERROR, "Database query failed while looking for characters for account.");
    }
    DatabaseResult::const_iterator Iend = dr.end();
    for (DatabaseResult::const_iterator I = dr.begin(); I != Iend; ++I) {
        const char * id = I.column(0);
        if (id == nullptr) {
            log(ERROR, "No ID data in relation when examing characters");
            continue;
        }

        long intId = integerId(id);

        auto J = worldObjects.find(intId);
        if (J == worldObjects.end()) {
            log(WARNING, String::compose("Persistence: Got character id \"%1\" "
                                         "from database which does not exist "
                                         "in world.", id));
            continue;
        }
        ac.addCharacter(J->second);
    }
}

void Persistence::addCharacter(const Account & ac, const LocatedEntity & e)
{
    //We can't insert the connection directly into the database, since the entity row
    //might not have been created. We'll instead emit a signal and rely on the StorageManager doing this for us.
    AddCharacterData data;
    data.account_id = ac.getId();
    data.entity_id = e.getId();

    bool handled = characterAdded(data);
    if (!handled) {
        log(WARNING, String::compose("Nothing handled the character with id %1 connected to account with id %2.", e.getId(), ac.getId()));
    }

}

void Persistence::delCharacter(const std::string & id)
{
    bool handled = characterDeleted(id);
    if (!handled) {
        log(WARNING, String::compose("Nothing handled the character with id %1 being deleted.", id));
    }
}

const std::string& Persistence::getCharacterAccountRelationName() const
{
    return m_characterRelation;
}


