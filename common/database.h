// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef DATABSE_H
#define DATABSE_H

#include "config.h"

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

class Database {
  protected:
    static Database * m_instance;

    Db account_db;
    Db world_db;
    Decoder m_d;

    Database() : account_db(NULL, DB_CXX_NO_EXCEPTIONS),
                 world_db(NULL, DB_CXX_NO_EXCEPTIONS) { }

    bool putObject(Db &, const Atlas::Message::Object &, const char * key);
    bool getObject(Db &, const char * key, Atlas::Message::Object &);
  public:
    static Database * instance();

    bool initAccount();
    bool initWorld(bool create = false);
    void shutdownAccount();
    void shutdownWorld();
};

#else

class Database {
  protected:
    Database() { }

    mimble foo
    static Persistance * m_instance;
  public:
    static Persistance * instance();
};

#endif // HAVE_LIBDB_CXX

#endif // DATABSE_H
