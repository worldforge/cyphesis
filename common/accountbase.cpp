// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "accountbase.h"

#include "system.h"

bool AccountBase::putAccount(const Atlas::Message::Element::MapType & o)
{
    Atlas::Message::Element::MapType::const_iterator I = o.find("username");
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
    m_connection.getEntityId(id);
    return m_connection.createSimpleRow("accounts", id, columns, values);
}

bool AccountBase::modAccount(const Atlas::Message::Element::MapType & o,
                             const std::string & accountId)
{
    std::string columns;
    bool empty = true;

    Atlas::Message::Element::MapType::const_iterator I = o.find("type");
    if ((I != o.end()) && (!I->second.isString())) {
        empty = false;
        columns += "type = '";
        columns += I->second.asString();
        columns += "'";
    }

    I = o.find("password");
    if ((I != o.end()) && (!I->second.isString())) {
        if (!empty) { columns += ", "; }
        columns += "password = '";
        columns += I->second.asString();
        columns += "'";
    }
    m_connection.updateSimpleRow("accounts", accountId, columns);
}

bool AccountBase::delAccount(const std::string & account)
{
    return false;
}

bool AccountBase::getAccount(const std::string & username,
                             Atlas::Message::Element::MapType & o)
{
    std::string namestr = "'" + username + "'";
    DatabaseResult dr = m_connection.selectSimpleRowBy("accounts", "username", namestr);
    if (dr.error()) {
        return 0;
    }
    if (dr.empty()) {
        dr.clear();
        return 0;
    }
    if (dr.size() > 1) {
        return 0;
    }
    const char * c = dr.field("id");
    if (c == 0) {
        dr.clear();
        return 0;
    }
    std::string id = c;

    c = dr.field("password");
    if (c == 0) {
        dr.clear();
        return 0;
    }
    std::string password = c;

    c = dr.field("type");
    if (c == 0) {
        dr.clear();
        return 0;
    }
    std::string type = c;

    dr.clear();

    o["id"] = id;
    o["username"] = username;
    o["password"] = password;
    o["type"] = type;
}
