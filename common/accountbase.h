// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef COMMON_ACCOUNT_BASE_H
#define COMMON_ACCOUNT_BASE_H

#include "Database.h"

class AccountBase {
  protected:
    Database & m_connection;
  public:
    AccountBase() : m_connection(*Database::instance()) { }

    ~AccountBase() {
        m_connection.shutdownConnection();
    }

    bool init();

    bool putAccount(const Atlas::Message::MapType & o);
    bool modAccount(const Atlas::Message::MapType & o,
                    const std::string & accountId);
    bool delAccount(const std::string & account);
    bool getAccount(const std::string & username,
                    Atlas::Message::MapType & o);

};

#endif // COMMON_ACCOUNT_BASE_H
