// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#ifndef SERVER_PERSISTENCE_H
#define SERVER_PERSISTENCE_H

#include "common/Singleton.h"
#include "modules/Ref.h"
#include <Atlas/Objects/ObjectsFwd.h>

#include <sigc++/signal.h>

#include <string>
#include <map>

class Account;

class Database;

class LocatedEntity;

typedef std::map<long, Ref<LocatedEntity>> EntityRefDict;

/// \brief Class for managing the required database tables for persisting
/// in-game entities and server accounts
class Persistence : public Singleton<Persistence>
{
    private:

    public:
        explicit Persistence(Database& database);

        Database& m_db;

        bool findAccount(const std::string&);

        std::unique_ptr<Account> getAccount(const std::string&);

        void putAccount(const Account&);

};

#endif // SERVER_PERSISTENCE_H
