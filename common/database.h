// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef COMMON_DATABSE_H
#define COMMON_DATABSE_H

#error This file has been removed from the build

#include <Atlas/Message/DecoderBase.h>

class Db;
class Dbc;
class Dbt;

class Decoder : public Atlas::Message::DecoderBase {
  private:
    virtual void ObjectArrived(const Atlas::Message::Object& obj) {
        m_check = true;
        m_obj = obj.AsMap();
    }
    bool m_check;
    Atlas::Message::Object::MapType m_obj;
  public:
    Decoder() : m_check (false) { }
    bool check() const { return m_check; }
    const Atlas::Message::Object::MapType & get() {
        m_check = false; return m_obj;
    }
};

class DatabaseIterator;

class Database {
  protected:
    static Database * m_instance;

    Db & account_db;
    Db & world_db;
    Db & mind_db;
    Db & server_db;
    std::string db_file;
    Decoder m_d;

    Database();

    bool decodeObject(Dbt & data, Atlas::Message::Object::MapType &);
    bool putObject(Db &, const Atlas::Message::Object::MapType &, const char *);
    bool getObject(Db &, const char * key, Atlas::Message::Object::MapType &);
    bool delObject(Db &, const char * key);
  public:
    static Database * instance();

    bool initAccount(bool create = false);
    bool initWorld(bool create = false);
    bool initMind(bool create = false);
    bool initServer(bool create = false);
    void shutdownAccount();
    void shutdownWorld();
    void shutdownMind();
    void shutdownServer();

    Db & getWorldDb() { return world_db; }
    Db & getMindDb() { return mind_db; }

    const std::string & getFilename() const { return db_file; }

    friend class DatabaseIterator;
};

class DatabaseIterator {
  protected:
    Dbc * m_cursor;
    Db & m_db;

  public:
    DatabaseIterator(Db & db );

    bool get(Atlas::Message::Object::MapType & o);
    bool del();
};

#endif // COMMON_DATABSE_H
