// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef PERSISTANCE_H
#define PERSISTANCE_H

class Admin;
class Account;

#ifdef HAVE_LIBDB_CXX
#include <db_cxx.h>

class Decoder : public Atlas::Message::DecoderBase {
    virtual void ObjectArrived(const Atlas::Message::Object& obj) { }
};
#endif

class Persistance {
  protected:
    Persistance();

    static Persistance * m_instance;
#ifdef HAVE_LIBDB_CXX
    Db account_db;
    Db world_db;
    Decoder m_d;

    void putObject(Db &, Atlas::Message::Object &, char * key);
#endif
  public:
    static Admin * load_admin_account();
    static void save_admin_account(Admin *);
    static Persistance * instance();
    static bool init();

    Account * getAccount(const std::string &);
    void putAccount(const Account *);
};

#endif /* PERSISTANCE_H */
