// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_PERSISTANCE_H
#define SERVER_PERSISTANCE_H

#include <Atlas/Message/Object.h>

#include <string>

class Admin;
class Account;
class Entity;
class Database;

class Persistance {
  private:
    Persistance();

    Database & m_connection;
    static Persistance * m_instance;
  public:
    static Account * loadAdminAccount();
    static Persistance * instance();
    static bool init();
    static void shutdown();

    static bool restricted;

    bool findAccount(const std::string &);
    Account * getAccount(const std::string &);
    void putAccount(const Account &);
    
    bool getRules(Atlas::Message::Object::MapType & m);
    bool clearRules();
};

#endif // SERVER_PERSISTANCE_H
