// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#error This file has been removed from the build

#ifndef COMMON_PERSISTANCE_H
#define COMMON_PERSISTANCE_H

class Admin;
class Account;
class Entity;

namespace Atlas { namespace Message { class Object; } }

#include "database.h"

class Persistance : public Database {
  protected:
    Persistance() { }

  public:
    static Account * loadAdminAccount();
    static void saveAdminAccount(Account &);
    static Persistance * instance();
    static bool init();
    static void shutdown();

    static bool restricted;

    bool findAccount(const std::string &);
    Account * getAccount(const std::string &);
    void putAccount(const Account &);
    
    //bool findEntity(const std::string &);
    bool getEntity(const std::string &, Atlas::Message::Object &);
    void putEntity(const Entity &);

    bool getMind(const std::string &, Atlas::Message::Object &);
    void putMind(const std::string &, const Atlas::Message::Object &);
};

#endif // COMMON_PERSISTANCE_H
