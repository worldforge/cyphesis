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

class DatabaseResult;

class Database {
  private:
    static Database * m_instance;

    std::string account_db;
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
    bool initRule(bool createTables = false);

    void shutdownConnection();

    bool registerRelation(const std::string & name);

    bool registerEntityIdGenerator();
    std::string getEntityId();

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
    const DatabaseResult selectEntityRow(const std::string & id,
                                         const std::string & classname = "");
    const DatabaseResult selectClassByLoc(const std::string & loc);
    const DatabaseResult selectOnlyByLoc(const std::string & loc,
                                         const std::string & classname);
                                   

};

// This class encapsulate a result from the database so we can use it in the
// upper layers in a database independant way.
// Perhaps an iterator model might be nice later?
class DatabaseResult {
  private:
    PGresult * m_res;
  public:
    explicit DatabaseResult(PGresult * r) : m_res(r) { }
    DatabaseResult(const DatabaseResult & dr) : m_res(dr.m_res) { }

    DatabaseResult & operator=(const DatabaseResult & other) {
        m_res = other.m_res;
        return *this;
    }

    class const_iterator {
      private:
        const DatabaseResult & m_dr;
        int m_row;
        
        const_iterator(const DatabaseResult & dr, int r = 0) : m_dr(dr),
                                                               m_row(r) { }
      public:
        const_iterator(const const_iterator & ci) : m_dr(ci.m_dr),
                                                    m_row(ci.m_row) { }

        bool operator==(const const_iterator & other) {
            return (m_row == other.m_row);
        }

        bool operator!=(const const_iterator & other) {
            return (m_row != other.m_row);
        }

        const_iterator operator++() {
            if (m_row != -1) {
                if (++m_row >= m_dr.size()) {
                    m_row = -1;
                }
            }
            return *this;
        }

        const char * column(int column) const {
            if (m_row == -1) {
                return 0;
            }
            return PQgetvalue(m_dr.m_res, m_row, column);
        }
        const char * column(const char *) const;

        friend class DatabaseResult;
    };

    int size() const { return PQntuples(m_res); }
    int empty() const { return (size() == 0); }
    int columns() const { return PQnfields(m_res); }
    bool error() const { return (m_res == NULL); }

    const_iterator begin() const {
        return const_iterator(*this);
    }

    const_iterator end() const {
        return const_iterator(*this, -1);
    }

    // const_iterator find() perhaps

    const char * field(int column,  int row = 0) const {
        return PQgetvalue(m_res, row, column);
    }
    const char * field(const char * column, int row = 0) const;
};

#endif // COMMON_DATABSE_H
