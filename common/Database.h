// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef COMMON_DATABSE_H
#define COMMON_DATABSE_H

#include <Atlas/Message/DecoderBase.h>

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
class PgDatabase;

class Database {
  protected:
    static Database * m_instance;

    std::string account_db;
    std::string world_db;
    std::string mind_db;
    std::string server_db;
    Decoder m_d;

    PgDatabase * m_connection;

    Database();

    bool command(const std::string & cmd);

    bool decodeObject(const std::string & data,
                      Atlas::Message::Object::MapType &);
    bool putObject(const std::string & table,
                   const std::string &,
                   const Atlas::Message::Object::MapType &);
    bool getObject(const std::string & table,
                   const std::string & key,
                   Atlas::Message::Object::MapType &);
    bool delObject(const std::string &, const std::string & key);

    void reportError();
  public:
    static Database * instance();

    bool initConnection(bool createDatabase = false);
    bool initAccount(bool createTables = false);
    bool initWorld(bool createTables = false);
    bool initMind(bool createTables = false);
    bool initServer(bool createTables = false);
    void shutdownConnection();

    friend class DatabaseIterator;
};

#endif // COMMON_DATABSE_H
