// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef PERSISTANCE_H
#define PERSISTANCE_H

class Admin;
class Account;

#include "database.h"

class Persistance : public Database {
  protected:
    Persistance() { }

  public:
    static Account * load_admin_account();
    static void save_admin_account(Account *);
    static Persistance * instance();
    static bool init();
    static void shutdown();

    static bool restricted;

    bool findAccount(const std::string &);
    Account * getAccount(const std::string &);
    void putAccount(const Account *);
};

#endif /* PERSISTANCE_H */
