// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef COMMON_DATABSE_H
#define COMMON_DATABSE_H

#include <Atlas/Message/DecoderBase.h>

#include <libpq-fe.h>

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

typedef std::map<std::string, std::string> TableDict;

class Database {
  private:
    static Database * m_instance;

    std::string account_db;
    std::string world_db;
    std::string mind_db;
    std::string server_db;
    std::string rule_db;

    TableDict entityTables;

    Decoder m_d;

    PGconn * m_connection;

    Database();

    // bool command(const std::string & cmd);

    bool tuplesOk();
    bool commandOk();

  public:
    const std::string & account() const { return account_db; }
    const std::string & world() const { return world_db; }
    const std::string & mind() const { return mind_db; }
    const std::string & server() const { return server_db; }
    const std::string & rule() const { return rule_db; }

    bool decodeObject(const std::string & data,
                      Atlas::Message::Object::MapType &);
    bool putObject(const std::string & table,
                   const std::string &,
                   const Atlas::Message::Object::MapType &);
    bool getObject(const std::string & table,
                   const std::string & key,
                   Atlas::Message::Object::MapType &);
    bool updateObject(const std::string & table,
                      const std::string & key,
                      const Atlas::Message::Object::MapType&);
    bool delObject(const std::string &, const std::string & key);
    bool getTable(const std::string & table,
                  Atlas::Message::Object::MapType &);
    bool clearTable(const std::string & table);

    void reportError();

    static Database * instance();

    bool initConnection(bool createDatabase = false);
    bool initAccount(bool createTables = false);
    bool initWorld(bool createTables = false);
    bool initMind(bool createTables = false);
    bool initServer(bool createTables = false);
    bool initRule(bool createTables = false);

    void shutdownConnection();

    bool registerEntityTable(const std::string & classname,
                             const Atlas::Message::Object::MapType & row,
                             const std::string & parent = "");
    bool createEntityRow(const std::string & classname,
                         const std::string & id,
                         const std::string & columns,
                         const std::string & values);
    bool updateEntityRow(const std::string & classname,
                         const std::string & id,
                         const std::string & columns);
    bool removeEntityRow(const std::string & classname,
                         const std::string & id);

};

#endif // COMMON_DATABSE_H
