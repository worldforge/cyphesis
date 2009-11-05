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

// $Id$

#ifndef SERVER_PERSISTENCE_H
#define SERVER_PERSISTENCE_H

#include <Atlas/Objects/ObjectsFwd.h>

#include <string>
#include <map>

class Account;
class Database;
class Entity;

typedef std::map<long, Entity *> EntityDict;

/// \brief Class for managing the required database tables for persisting
/// in-game entities and server accounts
class Persistence {
  private:
    Persistence();

    std::string m_characterRelation;

    static Persistence * m_instance;
  public:
    Database & m_connection;

    static Persistence * instance();
    int init();
    void shutdown();

    bool findAccount(const std::string &);
    Account * getAccount(const std::string &);
    void putAccount(const Account &);
    void registerCharacters(Account &, const EntityDict & worldObjects);
    void addCharacter(const Account &, const Entity &);
    void delCharacter(const std::string &);
    
    bool getRules(std::map<std::string, Atlas::Objects::Root> & m);
    bool storeRule(const Atlas::Objects::Root & rule,
                   const std::string & key);
    bool clearRules();
};

#endif // SERVER_PERSISTENCE_H
