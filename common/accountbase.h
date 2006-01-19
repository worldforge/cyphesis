// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001-2004 Alistair Riddoch
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

#ifndef COMMON_ACCOUNT_BASE_H
#define COMMON_ACCOUNT_BASE_H

#include "Database.h"

/// \brief Class to handle connecting to the cyphesis database in order to
/// access the account table
class AccountBase {
  protected:
    Database & m_connection;
  public:
    AccountBase() : m_connection(*Database::instance()) { }

    ~AccountBase() {
        m_connection.shutdownConnection();
    }

    int init();

    bool putAccount(const Atlas::Message::MapType & o);
    bool modAccount(const Atlas::Message::MapType & o,
                    const std::string & accountId);
    bool delAccount(const std::string & account);
    bool getAccount(const std::string & username,
                    Atlas::Message::MapType & o);

};

#endif // COMMON_ACCOUNT_BASE_H
