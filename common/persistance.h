// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef PERSISTANCE_H
#define PERSISTANCE_H

class Admin;
class Account;

#ifdef HAVE_LIBDB_CXX
#include <db3/db_cxx.h>

#include <Atlas/Message/DecoderBase.h>

class Decoder : public Atlas::Message::DecoderBase {
    virtual void ObjectArrived(const Atlas::Message::Object& obj) {
        cout << "GOT OBJECT" << endl << flush;
        m_check = true;
        m_obj = obj;
    }
    bool m_check;
    Atlas::Message::Object m_obj;
  public:
    Decoder() : m_check (false) { }
    bool check() const { return m_check; }
    const Atlas::Message::Object & get() { m_check = false; return m_obj; }
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

    bool putObject(Db &, const Atlas::Message::Object &, const char * key);
    bool getObject(Db &, const char * key, Atlas::Message::Object &);
#endif
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
