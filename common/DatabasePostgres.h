// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2018 Erik Ogenvik
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


#ifndef COMMON_DATABASEPOSTGRES_H
#define COMMON_DATABASEPOSTGRES_H

#include "Database.h"

#include <libpq-fe.h>

typedef std::pair<std::string, ExecStatusType> DatabaseQuery;
typedef std::deque<DatabaseQuery> QueryQue;

class DatabasePostgres : public Database
{
    protected:

        QueryQue pendingQueries;
        PGconn* m_connection;


        bool tuplesOk() override;

        int commandOk() override;

    public:
        static const unsigned int MAINTAIN_VACUUM = 0x0100;
        static const unsigned int MAINTAIN_VACUUM_FULL = 0x0001;
        static const unsigned int MAINTAIN_VACUUM_ANALYZE = 0x0002;
        static const unsigned int MAINTAIN_REINDEX = 0x0200;

        DatabasePostgres();

        ~DatabasePostgres() override;

        int initConnection() override;

        void shutdownConnection() override;

        PGconn* getConnection() const
        {
            return m_connection;
        }


        size_t queryQueueSize() const override
        {
            return pendingQueries.size();
        }


        int getObject(const std::string& table,
                      const std::string& key,
                      Atlas::Message::MapType&) override;

        int encodeObject(const Atlas::Message::MapType&,
                         std::string&) override;

        void reportError() override;

        int connect(const std::string& context, std::string& error_msg) override;


        DatabaseResult runSimpleSelectQuery(const std::string& query) override;

        int runCommandQuery(const std::string& query) override;


        int registerRelation(std::string& tablename,
                             const std::string& sourcetable,
                             const std::string& targettable,
                             RelationType kind) override;

        int registerThoughtsTable() override;

        int registerEntityTable(const std::map<std::string, int>& chunks) override;

        int registerPropertyTable() override;


        /// Creates a new unique id for the database.
        /// Note that this method will access the database, so it's a fairly expensive method.
        long newId(std::string& id) override;

        int registerEntityIdGenerator() override;


        int registerSimpleTable(const std::string& name,
                                const Atlas::Message::MapType& row) override;

        void queryResult(ExecStatusType status);

        void queryComplete() override;

        int launchNewQuery() override;

        int scheduleCommand(const std::string& query) override;

        int clearPendingQuery() override;

        int runMaintainance(unsigned int command = MAINTAIN_VACUUM);

};

/// \brief Class to encapsulate a result from the database.
///
/// This allows the result to be used in the upper layers in a database
/// independant way.
struct DatabaseResultWorkerPostgres : public DatabaseResult::DatabaseResultWorker
{

    struct PGresultDeleter
    {
        void operator()(PGresult* p) const
        {
            PQclear(p);
        }
    };

    static PGresultDeleter deleter;

    std::shared_ptr<PGresult> m_res;

    explicit DatabaseResultWorkerPostgres(PGresult* r) : m_res(r, deleter)
    {}

    struct const_iterator_worker_postgres : public DatabaseResult::const_iterator_worker
    {
        const DatabaseResultWorkerPostgres& m_dr;

        const_iterator_worker_postgres(const DatabaseResultWorkerPostgres& dr, int row)
            : const_iterator_worker(row),
              m_dr(dr)
        {
        }

        const char* column(int column) const override
        {
            if (m_row == -1) {
                return nullptr;
            }
            return PQgetvalue(m_dr.m_res.get(), m_row, column);
        }

        const char* column(const char* column) const override
        {
            int col_num = PQfnumber(m_dr.m_res.get(), column);
            if (col_num == -1) {
                return "";
            }
            return PQgetvalue(m_dr.m_res.get(), m_row, col_num);
        }

    };


    int size() const override
    { return PQntuples(m_res.get()); }

    int columns() const override
    { return PQnfields(m_res.get()); }

    bool error() const override
    { return (m_res == nullptr); }

    DatabaseResult::const_iterator begin() const override
    {
        return DatabaseResult::const_iterator(std::unique_ptr<const_iterator_worker_postgres>(new const_iterator_worker_postgres{*this, 0}), *this);
    }

    DatabaseResult::const_iterator end() const override
    {
        return DatabaseResult::const_iterator(std::unique_ptr<const_iterator_worker_postgres>(new const_iterator_worker_postgres{*this, -1}), *this);
    }

    // const_iterator find() perhaps

    const char* field(int column, int row) const override
    {
        return PQgetvalue(m_res.get(), row, column);
    }

    const char* field(const char* column, int row) const override
    {
        int col_num = PQfnumber(m_res.get(), column);
        if (col_num == -1) {
            return "";
        }
        return PQgetvalue(m_res.get(), row, col_num);
    }

};

#endif // COMMON_DATABASEPOSTGRES_H
