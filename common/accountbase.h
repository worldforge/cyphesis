// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef COMMON_ACCOUNT_BASE_H
#define COMMON_ACCOUNT_BASE_H

#include "Database.h"

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

    bool putAccount(const Atlas::Message::Element::MapType & o,
                    const std::string & account);
    bool modAccount(const Atlas::Message::Element::MapType & o,
                    const std::string & account);
    bool delAccount(const std::string & account);
    bool getAccount(const std::string & account,
                    Atlas::Message::Element::MapType & o);

};

#endif // COMMON_ACCOUNT_BASE_H
