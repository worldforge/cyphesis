// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_PERSISTANCE_H
#define SERVER_PERSISTANCE_H

#include "common/types.h"

#include <Atlas/Message/Element.h>

#include <string>

class Account;
class Database;

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
    
    bool getRules(MapType & m);
    bool clearRules();
};

#endif // SERVER_PERSISTANCE_H
