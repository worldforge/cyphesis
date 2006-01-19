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

#ifndef SERVER_PERSISTANCE_H
#define SERVER_PERSISTANCE_H

#include <Atlas/Message/Element.h>

#include <string>

class Entity;
class Account;
class Database;

typedef std::map<long, Entity *> EntityDict;

/// \brief Class for managing the required database tables for persisting
/// in-game entities and server accounts
class Persistance {
  private:
    Persistance();

    std::string m_characterRelation;

    static Persistance * m_instance;
  public:
    Database & m_connection;

    static Persistance * instance();
    static int init();
    static void shutdown();

    bool findAccount(const std::string &);
    Account * getAccount(const std::string &);
    void putAccount(const Account &);
    void registerCharacters(Account &, const EntityDict & worldObjects);
    void addCharacter(const Account &, const Entity &);
    void delCharacter(const std::string &);
    
    bool getRules(Atlas::Message::MapType & m);
    bool clearRules();
};

#endif // SERVER_PERSISTANCE_H
