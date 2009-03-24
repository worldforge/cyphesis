// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001-2004 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: 1c80a2af70622b49664b79a93016a89541e79196 $

#ifndef COMMON_DATABSE_H
#define COMMON_DATABSE_H

#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Message/Element.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <libpq-fe.h>

#include <set>

/// \brief Class to handle decoding Atlas encoded database records
class Decoder : public Atlas::Message::DecoderBase {
  private:
    virtual void messageArrived(const Atlas::Message::MapType & msg) {
        m_check = true;
        m_msg = msg;
    }

    bool m_check;
    Atlas::Message::MapType m_msg;
  public:
    Decoder() : m_check (false) {
    }

    bool check() const {
        return m_check;
    }

    const Atlas::Message::MapType & get() {
        m_check = false;
        return m_msg;
    }
};

/// \brief Class to handle decoding Atlas encoded database records
class ObjectDecoder : public Atlas::Objects::ObjectsDecoder {
  private:
    virtual void objectArrived(const Atlas::Objects::Root & obj) {
        m_check = true;
        m_obj = obj;
    }

    bool m_check;
    Atlas::Objects::Root m_obj;
  public:
    ObjectDecoder() : m_check (false) {
    }

    bool check() const {
        return m_check;
    }

    const Atlas::Objects::Root & get() {
        m_check = false;
        return m_obj;
    }
};

class DatabaseResult;

typedef std::vector<std::string> StringVector;
typedef std::set<std::string> TableSet;
typedef std::pair<std::string, ExecStatusType> DatabaseQuery;
typedef std::deque<DatabaseQuery> QueryQue;

/// \brief Class to provide interface to Database connection
///
/// Most SQL is generated from here, including queries for handling all
/// table creation, queries to simple non-inherited tables and more
class Database {
  protected:
    static Database * m_instance;

    std::string m_rule_db;

    TableSet allTables;
    QueryQue pendingQueries;
    bool m_queryInProgress;

    Decoder m_d;
    ObjectDecoder m_od;

    PGconn * m_connection;

    Database();

    // bool command(const std::string & cmd);

    bool tuplesOk();
    bool commandOk();

  public:
    static const int MAINTAIN_VACUUM = 0x0100;
    static const int MAINTAIN_VACUUM_FULL = 0x0001;
    static const int MAINTAIN_VACUUM_ANALYZE = 0x0002;
    static const int MAINTAIN_REINDEX = 0x0200;

    typedef enum { OneToMany, ManyToMany, ManyToOne, OneToOne } RelationType;

    typedef std::map<std::string, std::string> KeyValues;

    PGconn * getConnection() const { return m_connection; }
    const std::string & rule() const { return m_rule_db; }
    bool queryInProgress() const { return m_queryInProgress; }

    size_t queryQueueSize() const {
        return pendingQueries.size();
    }

    bool decodeObject(const std::string & data,
                      Atlas::Objects::Root &);

    bool decodeMessage(const std::string & data,
                       Atlas::Message::MapType &);
    bool encodeObject(const Atlas::Message::MapType &,
                      std::string &);
    bool putObject(const std::string & table,
                   const std::string &,
                   const Atlas::Message::MapType &,
                   const StringVector & = StringVector());
    bool getObject(const std::string & table,
                   const std::string & key,
                   Atlas::Message::MapType &);
    bool updateObject(const std::string & table,
                      const std::string & key,
                      const Atlas::Message::MapType&);
    bool delObject(const std::string &, const std::string & key);
    bool hasKey(const std::string &, const std::string & key);
    bool getTable(const std::string & table,
                  std::map<std::string, Atlas::Objects::Root> &);
    bool clearTable(const std::string & table);

    void reportError();

    int connect(const std::string & context, std::string & error_msg);

    static Database * instance();
    static void cleanup();

    int initConnection();
    int createInstanceDatabase();
    bool initRule(bool createTables = false);

    void shutdownConnection();

    const DatabaseResult runSimpleSelectQuery(const std::string & query);
    bool runCommandQuery(const std::string & query);

    // Interface for relations between tables.

    bool registerRelation(std::string & tablename,
                          const std::string & sourcetable,
                          const std::string & targettable,
                          RelationType kind = OneToMany);
    const DatabaseResult selectRelation(const std::string & name,
                                        const std::string & id);
    bool createRelationRow(const std::string & name,
                           const std::string & id,
                           const std::string & other);
    bool removeRelationRow(const std::string & name,
                           const std::string & id);
    bool removeRelationRowByOther(const std::string & name,
                                  const std::string & other);

    // Interface for simple tables that mainly just store Atlasish data.

    bool registerSimpleTable(const std::string & name,
                             const Atlas::Message::MapType & row);
    const DatabaseResult selectSimpleRow(const std::string & name,
                                         const std::string & id);
    const DatabaseResult selectSimpleRowBy(const std::string & name,
                                           const std::string & column,
                                           const std::string & value);
    bool createSimpleRow(const std::string & name,
                         const std::string & id,
                         const std::string & columns,
                         const std::string & values);
    bool updateSimpleRow(const std::string & name,
                         const std::string & key,
                         const std::string & value,
                         const std::string & columns);

    // Interface for the ID generation sequence.

    int registerEntityIdGenerator();
    long newId(std::string & id);

    // Interface for Entity and Property tables.

    int registerEntityTable(const std::map<std::string, int> & chunks);
    int insertEntity(const std::string & id,
                     const std::string & loc,
                     const std::string & type,
                     int seq,
                     const std::string & value);
    int updateEntity(const std::string & id,
                     int seq,
                     const std::string & value);
    const DatabaseResult selectEntities(const std::string & loc);
    int dropEntity(long id);

    int registerPropertyTable();
    int insertProperties(const std::string & id,
                         const KeyValues & tuples);
    const DatabaseResult selectProperties(const std::string & loc);
    int updateProperties(const std::string & id,
                         const KeyValues & tuples);

    // Interface for CommPSQLSocket, so it can give us feedback
    
    void queryResult(ExecStatusType);
    void queryComplete();
    bool launchNewQuery();
    bool scheduleCommand(const std::string & query);
    bool clearPendingQuery();
    bool runMaintainance(int command = MAINTAIN_VACUUM);

};

/// \brief Class to encapsulate a result from the database.
///
/// This allows the result to be used in the upper layers in a database
/// independant way.
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

    /// \brief Iterator for DatabaseResult
    ///
    /// Minics STL iterator API
    class const_iterator {
      private:
        const DatabaseResult & m_dr;
        int m_row;
        
        const_iterator(const DatabaseResult & dr, int r = 0) : m_dr(dr),
                                                               m_row(r) {
            if (m_row != -1) {
                if (m_row >= m_dr.size()) {
                    m_row = -1;
                }
            }
        }
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

        void readColumn(const char *, int &) const;
        void readColumn(const char *, float &) const;
        void readColumn(const char *, double &) const;
        void readColumn(const char *, std::string &) const;
        void readColumn(const char *, Atlas::Message::MapType &) const;

        friend class DatabaseResult;
    };

    int size() const { return PQntuples(m_res); }
    int empty() const { return (size() == 0); }
    int columns() const { return PQnfields(m_res); }
    bool error() const { return (m_res == NULL); }
    void clear() { PQclear(m_res); }

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
