// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001-2005 Alistair Riddoch
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


#include "Storage.h"

#include "system.h"
#include "globals.h"
#include "log.h"
#include "compose.hpp"
#include "debug.h"
#include "Shaker.h"

#include <iostream>

static const bool debug_flag = false;

Storage::Storage(Database& database) : m_connection(database) {
    if (m_connection.initConnection() != 0) {
        if (::instance == CYPHESIS) {
            throw std::runtime_error("Could not create database connection.");
        }
        if (m_connection.createInstanceDatabase() != 0) {
            throw std::runtime_error("Database creation failed.");
        }
        if (m_connection.initConnection() != 0) {
            throw std::runtime_error("Still couldn't connect.");
        }
        log(INFO, String::compose("Auto created database for new instance "
                                  "\"%1\".", ::instance));
    }

    std::map<std::string, int> chunks;
    chunks["location"] = 0;

    if (m_connection.registerEntityTable(chunks) != 0) {
        throw std::runtime_error("Failed to create Entity in database.");
    }

    if (m_connection.registerPropertyTable() != 0) {
        throw std::runtime_error("Failed to create Property in database.");
    }

    if (m_connection.registerThoughtsTable() != 0) {
        throw std::runtime_error("Failed to create Thought in database.");
    }

    Atlas::Message::MapType tableDesc;
    tableDesc["username"] = "                                                                                ";
    tableDesc["password"] = "                                                                                ";
    tableDesc["type"] = "          ";
    m_connection.registerSimpleTable("accounts", tableDesc);

    if (m_connection.registerEntityIdGenerator() != 0) {
        throw std::runtime_error("Failed to register Id generator in database.");
    }

    Atlas::Message::MapType data;
    if (getAccount("admin", data) != 0) {
        debug_print("Bootstrapping admin account.")


        //The shaker isn't meant for this, but it will suffice. A password of 32
        //characters length should be safe enough.
        Shaker shaker;
        std::string password = shaker.generateSalt(32);


        log(INFO, "Created 'admin' account with randomized password.\n"
                  "In order to use it, use the 'cypasswd' tool from the "
                  "command line to alter the password.");

        putAccount({{"username", "admin"}, {"password",password}, {"type", "admin"}});
    }
    m_connection.blockUntilAllQueriesComplete();
}


/// \brief Store a new Account in the database
///
/// @param account Atlas description of Account to be stored
int Storage::putAccount(const Atlas::Message::MapType & account)
{
    auto I = account.find("username");
    if (I == account.end() || !I->second.isString()) {
        return -1;
    }
    const std::string & username = I->second.String();
    
    I = account.find("password");
    if (I == account.end() || !I->second.isString()) {
        return -1;
    }
    const std::string & password = I->second.String();
    std::string hash;
    encrypt_password(password, hash);
    
    std::string type = "player";
    I = account.find("type");
    if (I != account.end() && I->second.isString()) {
        type = I->second.String();
    }
    
    std::string columns = "username, type, password";
    std::string values = "'";
    values += username;
    values += "', '";
    values += type;
    values += "', '";
    values += hash;
    values += "'";

    auto id = m_connection.newId();
    if (id < 0) {
        return -1;
    }
    return m_connection.createSimpleRow("accounts", std::to_string(id), columns, values);
}

/// \brief Modify the attributes of an Account in the database
///
/// @param account Atlas description of the Account to be modified
/// @param accountId String identifier of the Account to be modified
int Storage::modAccount(const Atlas::Message::MapType & account,
                        const std::string & accountId)
{
    std::string columns;
    bool empty = true;

    auto I = account.find("type");
    if (I != account.end() && I->second.isString()) {
        empty = false;
        columns += "type = '";
        columns += I->second.String();
        columns += "'";
    }

    I = account.find("password");
    if (I != account.end() && I->second.isString()) {
        if (!empty) { columns += ", "; }
        std::string hash;
        encrypt_password(I->second.String(), hash);
        columns += "password = '";
        columns += hash;
        columns += "'";
    }
    return m_connection.updateSimpleRow("accounts", "username",
                                        accountId, columns);
}

/// \brief Remove an Account from the accounts database
///
/// @param account String identifier of the Account to be removed.
int Storage::delAccount(const std::string & account)
{
    return -1;
}

/// \brief Retrieve an Account from the accounts database
///
/// @param username Username of the Account to be found
/// @param account Account description returned here
int Storage::getAccount(const std::string & username,
                        Atlas::Message::MapType & account)
{
    std::string namestr = "'" + username + "'";
    DatabaseResult dr = m_connection.selectSimpleRowBy("accounts", "username", namestr);
    if (dr.error()) {
        return -1;
    }
    if (dr.empty()) {
        return -1;
    }
    if (dr.size() > 1) {
        return -1;
    }
    auto result = dr.begin();
    const char * c = result.column("id");
    if (!c) {
        return -1;
    }
    std::string id = c;

    c = result.column("password");
    if (!c) {
        return -1;
    }
    std::string password = c;

    c = result.column("type");
    if (!c) {
        return -1;
    }
    std::string type = c;

    account["id"] = id;
    account["username"] = username;
    account["password"] = password;
    account["type"] = type;

    return 0;
}
