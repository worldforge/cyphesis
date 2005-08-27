// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001-2004 Alistair Riddoch

#include "accountbase.h"

#include "system.h"

#include <Atlas/Message/Element.h>

int AccountBase::init()
{
    return m_connection.initConnection(false);
}

bool AccountBase::putAccount(const Atlas::Message::MapType & o)
{
    Atlas::Message::MapType::const_iterator I = o.find("username");
    if ((I == o.end()) || (!I->second.isString())) {
        return false;
    }
    const std::string & username = I->second.asString();
    
    I = o.find("password");
    if ((I == o.end()) || (!I->second.isString())) {
        return false;
    }
    const std::string & password = I->second.asString();
    std::string hash;
    encrypt_password(password, hash);
    
    std::string type = "player";
    I = o.find("type");
    if ((I != o.end()) && (I->second.isString())) {
        type = I->second.asString();
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
    m_connection.newId(id);
    return m_connection.createSimpleRow("accounts", id, columns, values);
}

bool AccountBase::modAccount(const Atlas::Message::MapType & o,
                             const std::string & accountId)
{
    std::string columns;
    bool empty = true;

    Atlas::Message::MapType::const_iterator I = o.find("type");
    if ((I != o.end()) && (I->second.isString())) {
        empty = false;
        columns += "type = '";
        columns += I->second.asString();
        columns += "'";
    }

    I = o.find("password");
    if ((I != o.end()) && (I->second.isString())) {
        if (!empty) { columns += ", "; }
        std::string hash;
        encrypt_password(I->second.asString(), hash);
        columns += "password = '";
        columns += hash;
        columns += "'";
    }
    return m_connection.updateSimpleRow("accounts", "username",
                                        accountId, columns);
}

bool AccountBase::delAccount(const std::string & account)
{
    return false;
}

bool AccountBase::getAccount(const std::string & username,
                             Atlas::Message::MapType & o)
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

    o["id"] = id;
    o["username"] = username;
    o["password"] = password;
    o["type"] = type;

    return true;
}
