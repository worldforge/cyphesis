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


#ifndef COMMON_STORAGE_H
#define COMMON_STORAGE_H

#include "Database.h"

/// \brief Class to handle connecting to the cyphesis database in order to
/// access the account table
class Storage {
  protected:
    /// \brief Database connection used to change the accounts table
    Database & m_connection;

  public:
    explicit Storage(Database& database);

    int putAccount(const Atlas::Message::MapType & o);
    int modAccount(const Atlas::Message::MapType & o,
                   const std::string & accountId);
    int delAccount(const std::string & account);
    int getAccount(const std::string & username,
                   Atlas::Message::MapType & o);

};


#endif // COMMON_STORAGE_H
