// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef COMMON_ACCOUNT_BASE_H
#define COMMON_ACCOUNT_BASE_H

#include <Atlas/Message/Object.h>

#include <common/Database.h>

using Atlas::Message::Object;

class AccountBase {
  protected:
    AccountBase() : m_connection(*Database::instance()) { }

    Database & m_connection;
    static AccountBase * m_instance;
  public:
    static AccountBase * instance(bool create = false) {
        if (m_instance == NULL) {
            m_instance = new AccountBase();
            m_instance->m_connection.initConnection(create);
            m_instance->m_connection.initAccount(create);
        }
        return m_instance;
    }

    static void del() {
        if (m_instance != NULL) {
            m_instance->m_connection.shutdownConnection();
            delete m_instance;
            m_instance = NULL;
        }
    }

    bool putAccount(const Object::MapType & o, const std::string & account) {
        return m_connection.putObject(m_connection.account(), account, o);
    }
    bool modAccount(const Object::MapType & o, const std::string & account) {
        return m_connection.updateObject(m_connection.account(), account, o);
    }
    bool delAccount(const std::string & account) {
        return m_connection.delObject(m_connection.account(), account);
    }
    bool getAccount(const std::string & account, Object::MapType & o) {
        return m_connection.getObject(m_connection.account(), account, o);
    }

};

#endif // COMMON_ACCOUNT_BASE_H
