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

    bool putAccount(const Atlas::Message::Element::MapType & o);
    bool modAccount(const Atlas::Message::Element::MapType & o,
                    const std::string & accountId);
    bool delAccount(const std::string & account);
    bool getAccount(const std::string & username,
                    Atlas::Message::Element::MapType & o);

};

#endif // COMMON_ACCOUNT_BASE_H
