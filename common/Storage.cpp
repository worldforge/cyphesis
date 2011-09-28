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

// $Id$

#include "Storage.h"

#include "system.h"

#include <Atlas/Message/Element.h>

#include <iostream>

/// \brief Initialise a connection to the accounts database
int Storage::init()
{
    if (m_connection.initConnection() != 0) {
        return -1;
    }
    if (!m_connection.initRule(true)) {
        return -1;
    }
    return 0;
}

/// \brief Store a new Account in the database
///
/// @param account Atlas description of Account to be stored
bool Storage::putAccount(const Atlas::Message::MapType & account)
{
    Atlas::Message::MapType::const_iterator I = account.find("username");
    if (I == account.end() || !I->second.isString()) {
        return false;
    }
    const std::string & username = I->second.String();
    
    I = account.find("password");
    if (I == account.end() || !I->second.isString()) {
        return false;
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

    std::string id;
    if (m_connection.newId(id) < 0) {
        return false;
    }
    return m_connection.createSimpleRow("accounts", id, columns, values);
}

/// \brief Modify the attributes of an Account in the database
///
/// @param account Atlas description of the Account to be modified
/// @param accountId String identifier of the Account to be modified
bool Storage::modAccount(const Atlas::Message::MapType & account,
                         const std::string & accountId)
{
    std::string columns;
    bool empty = true;

    Atlas::Message::MapType::const_iterator I = account.find("type");
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
bool Storage::delAccount(const std::string & account)
{
    return false;
}

/// \brief Retrieve an Account from the accounts database
///
/// @param username Username of the Account to be found
/// @param account Account description returned here
bool Storage::getAccount(const std::string & username,
                         Atlas::Message::MapType & account)
{
    std::string namestr = "'" + username + "'";
    DatabaseResult dr = m_connection.selectSimpleRowBy("accounts", "username", namestr);
    if (dr.error()) {
        return false;
    }
    if (dr.empty()) {
        dr.clear();
        return false;
    }
    if (dr.size() > 1) {
        return false;
    }
    const char * c = dr.field("id");
    if (c == 0) {
        dr.clear();
        return false;
    }
    std::string id = c;

    c = dr.field("password");
    if (c == 0) {
        dr.clear();
        return false;
    }
    std::string password = c;

    c = dr.field("type");
    if (c == 0) {
        dr.clear();
        return false;
    }
    std::string type = c;

    dr.clear();

    account["id"] = id;
    account["username"] = username;
    account["password"] = password;
    account["type"] = type;

    return true;
}

void Storage::storeInRules(const Atlas::Message::MapType & rule,
                           const std::string & key)
{
    if (m_connection.hasKey(m_connection.rule(), key)) {
        return;
    }
    m_connection.putObject(m_connection.rule(), key, rule, StringVector(1, m_rulesetName));
    if (m_connection.clearPendingQuery() != 0) {
        // FIXME NO cerr
        std::cerr << "Failed" << std::endl << std::flush;
    }
}

bool Storage::clearRules()
{
    return (m_connection.clearTable(m_connection.rule()) &&
            m_connection.clearPendingQuery() == 0);
}
