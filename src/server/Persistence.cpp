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

#include "rules/LocatedEntity.h"

#include "common/id.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/Database.h"
#include "common/Shaker.h"

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Objects::Root;

static const bool debug_flag = false;

Persistence::Persistence(Database& database)
        : m_db(database)
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

    MapType tableDesc;
    tableDesc["username"] = "                                                                                ";
    tableDesc["password"] = "                                                                                ";
    tableDesc["type"] = "          ";
    bool j = m_db.registerSimpleTable("accounts", tableDesc) == 0;

    if (m_db.registerEntityIdGenerator() != 0) {
        log(ERROR, "Failed to register Id generator in database.");
        return DATABASE_TABERR;
    }

    if (!findAccount("admin")) {
        debug_print("Bootstrapping admin account.")
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

        Admin dummyAdminAccount(nullptr, "admin", password,
                                adminAccountId, adminAccountIntId);

        log(INFO, "Created 'admin' account with randomized password.\n"
                  "In order to use it, use the 'cypasswd' tool from the "
                  "command line to alter the password.");

        putAccount(dummyAdminAccount);
    }

    return j ? 0 : DATABASE_TABERR;
}

bool Persistence::findAccount(const std::string& name)
{
    std::string namestr = "'" + name + "'";
    DatabaseResult dr = m_db.selectSimpleRowBy("accounts", "username", namestr);
    if (dr.error()) {
        log(ERROR, String::compose("Failure while finding account '%1'.", name));
        return false;
    }
    if (dr.empty()) {
        return false;
    }
    if (dr.size() > 1) {
        log(ERROR, String::compose("Duplicate username in accounts database for name '%1'.", name));
    }
    return true;
}

std::unique_ptr<Account> Persistence::getAccount(const std::string& name)
{
    std::string namestr = "'" + name + "'";
    DatabaseResult dr = m_db.selectSimpleRowBy("accounts", "username", namestr);
    if (dr.error()) {
        log(ERROR, String::compose("Failure while finding account '%1'.", name));
        return nullptr;
    }
    if (dr.empty()) {
        return nullptr;
    }
    if (dr.size() > 1) {
        log(ERROR, String::compose("Duplicate username in accounts database for name '%1'.", name));
    }
    auto first = dr.begin();
    const char* c = first.column("id");
    if (c == nullptr) {
        log(ERROR, "Unable to find id field in accounts database.");
        return nullptr;
    }
    std::string id = c;
    long intId = integerId(id);
    if (intId == -1) {
        log(ERROR, String::compose(R"(Invalid ID "%1" for account "%2" from database.)", id, name));
        return nullptr;
    }
    c = first.column("password");
    if (c == nullptr) {
        log(ERROR, "Unable to find password field in accounts database.");
        return nullptr;
    }
    std::string passwd = c;
    c = first.column("type");
    if (c == nullptr) {
        log(ERROR, "Unable to find type field in accounts database.");
        return nullptr;
    }
    std::string type = c;
    if (type == "admin") {
        return std::make_unique<Admin>(nullptr, name, passwd, id, intId);
    } else if (type == "server") {
        return std::make_unique<ServerAccount>(nullptr, name, passwd, id, intId);
    } else {
        return std::make_unique<Player>(nullptr, name, passwd, id, intId);
    }
}

void Persistence::putAccount(const Account& ac)
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



